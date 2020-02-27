#ifndef MASTER_H_
#define MASTER_H_

#include <systemc.h>
using namespace sc_core;
using namespace sc_dt;
using namespace std;

#include <tlm.h>
#include <tlm_utils/peq_with_cb_and_phase.h>
#include <tlm_utils/simple_initiator_socket.h>
#include <tlm_utils/simple_target_socket.h>

#include <time.h>

#include "memory_manager.h"

// define to enable several messages
//#define DEBUG

// **************************************************************************************
// Initiator module generating multiple pipelined generic payload transactions
// **************************************************************************************
struct Master : sc_module {
  // initiator socket -> using default settings (32-bits wide, base protocol,
  // etc.)
public:
  // interface
  sc_in<bool> tb_start;
  sc_in<bool> tb_write;
  sc_in<int> tb_adr_i;
  sc_in<int> tb_data_i;
  sc_out<int> tb_data_o;
  sc_in<int> tb_delay_i;
  sc_in<bool> tb_cleanup_i;
  sc_out<bool> tb_rdy;

  tlm_utils::simple_initiator_socket<Master> socket;

  // CTOR
  SC_CTOR(Master)
      : socket("bus_rw"), request_in_progress(0), // pointer to a transaction
        m_peq(this, &Master::peq_cb) { // event queue for pending notifications
    // Register callbacks for incoming interface method calls
    socket.register_nb_transport_bw(this, &Master::nb_transport_bw);

    // process -> doing some r/w accesses
    SC_THREAD(thread_process);
    sensitive << tb_start;
  }

  /*********************************************************
              Process of the module
      *********************************************************/
  void thread_process() {
    while (true) {
      wait(tb_start.posedge_event());

      tb_rdy.write(false);
      // payload object
      tlm::tlm_generic_payload *trans;
      // TLM phase
      tlm::tlm_phase phase;
      // delay
      sc_time delay;
      // init random function
      srand(time(NULL));

      int adr = tb_adr_i.read();
      tlm::tlm_command cmd = tlm::TLM_READ_COMMAND;
      m_data = 0;

      if (tb_write.read()) {
        cmd = tlm::TLM_WRITE_COMMAND;
        m_data = tb_data_i.read();
      }

      // Grab a new transaction from the memory manager
      trans = m_mm.allocate();
      trans->acquire();

      // Set all attributes except byte_enable_length and extensions (unused)
      trans->set_command(cmd);
      trans->set_address(adr);
      trans->set_data_ptr(reinterpret_cast<unsigned char *>(&m_data));
      trans->set_data_length(4);
      trans->set_streaming_width(4); // = data_length to indicate no streaming
      trans->set_byte_enable_ptr(0); // 0 indicates unused
      trans->set_dmi_allowed(false); // Mandatory initial value
      trans->set_response_status(
          tlm::TLM_INCOMPLETE_RESPONSE); // Mandatory initial value

      // Initiator must honor BEGIN_REQ/END_REQ exclusion rule
      // so if pointer to transaction in request phase is not 0,
      // wait until ongoing transaction enters response phase
      if (request_in_progress) {
        wait(end_request_event);
      }
      // transaction in request phase
      request_in_progress = trans;
      // set phase to BEGIN_REQ
      phase = tlm::BEGIN_REQ;

      // Timing annotation models processing time of initiator prior to call
      delay = sc_time(tb_delay_i.read(), SC_PS);

      // print status message of new transaction
#ifdef DEBUG
      cout << "new, trans = { " << (cmd ? 'W' : 'R') << ", " << hex << adr
           << " } , data = " << hex << m_data << " at time " << sc_time_stamp()
           << " delay = " << delay << endl;
#endif

      // Non-blocking transport call on the forward path
      tlm::tlm_sync_enum status;
      status = socket->nb_transport_fw(*trans, phase, delay);

      // Check value returned from nb_transport_fw
      // if status == tlm::TLM_ACCEPTED, we must do nothing because
      // cb function will be triggered
      if (status == tlm::TLM_UPDATED) {
        // slave used return path -> initiator must
        // set event in order to trigger cb function
        // The timing annotation must be honored
        m_peq.notify(*trans, phase, delay);
      } else if (status == tlm::TLM_COMPLETED) {
        // The completion of the transaction necessarily ends the BEGIN_REQ
        // phase
        request_in_progress = 0;
        // The target has terminated the transaction
        check_transaction(*trans);
      }

      if (tb_cleanup_i.read()) {
        CleanUp(trans);
      }

      wait(100, SC_NS);
      tb_rdy.write(true);
    }
  }

  void CleanUp(tlm::tlm_generic_payload *trans) {
    // Allocate a transaction for one final, nominal call to b_transport
    trans = m_mm.allocate();
    trans->acquire();
    trans->set_command(tlm::TLM_WRITE_COMMAND);
    trans->set_address(0);
    trans->set_data_ptr(reinterpret_cast<unsigned char *>(&m_data));
    trans->set_data_length(4);
    trans->set_streaming_width(4); // == data_length to indicate no streaming
    trans->set_byte_enable_ptr(0); // 0 indicates unused
    trans->set_dmi_allowed(false); // Mandatory initial value
    trans->set_response_status(
        tlm::TLM_INCOMPLETE_RESPONSE); // Mandatory initial value
    sc_time delay;
    delay = sc_time(tb_delay_i.read(), SC_PS);

#ifdef DEBUG
    cout << "Calling b_transport at " << sc_time_stamp()
         << " with delay = " << delay << endl;
#endif
    // Call b_transport to demonstrate the b/nb conversion by the
    // simple_target_socket
    // -> call ends in the target's nb_transport, phases are handled internal
    socket->b_transport(*trans, delay);
    check_transaction(*trans);
    tb_rdy.write(true);
  }

  /*********************************************************
          TLM-2 backward non-blocking transport method
  *********************************************************/
  virtual tlm::tlm_sync_enum nb_transport_bw(tlm::tlm_generic_payload &trans,
                                             tlm::tlm_phase &phase,
                                             sc_time &delay) {
    // honor timing annotation
    m_peq.notify(trans, phase, delay);
    return tlm::TLM_ACCEPTED;
  }

  /**************************************************************************
          Payload event queue callback to handle transactions from target
          Transaction could have arrived through return path or backward path
  **************************************************************************/
  void peq_cb(tlm::tlm_generic_payload &trans, const tlm::tlm_phase &phase) {
#ifdef DEBUG
    if (phase == tlm::END_REQ)
      cout << hex << trans.get_address() << " END_REQ at " << sc_time_stamp()
           << endl;
    else if (phase == tlm::BEGIN_RESP)
      cout << hex << trans.get_address() << " BEGIN_RESP at " << sc_time_stamp()
           << endl;
#endif

    if (phase == tlm::END_REQ ||
        (&trans == request_in_progress && phase == tlm::BEGIN_RESP)) {
      // the end of the BEGIN_RESP phase
      request_in_progress = 0;
      end_request_event.notify();
    } else if (phase == tlm::BEGIN_REQ || phase == tlm::END_RESP) {
      SC_REPORT_FATAL("TLM-2",
                      "Illegal transaction phase received by initiator");
    }

    if (phase == tlm::BEGIN_RESP) {
      check_transaction(trans);

      if (trans.get_command() == tlm::TLM_READ_COMMAND) {
        tb_data_o.write(m_data);
      }
      tlm::tlm_phase fw_phase = tlm::END_RESP;
      sc_time delay = sc_time(tb_delay_i.read(), SC_PS);
      socket->nb_transport_fw(trans, fw_phase, delay);
      // return value will be ignored
    }
  }

  /**************************************************************************
          Called on receiving BEGIN_RESP or TLM_COMPLETED
  **************************************************************************/
  void check_transaction(tlm::tlm_generic_payload &trans) {
    // check if an error occurs
    if (trans.is_response_error()) {
      char txt[100];
      sprintf(txt, "Transaction returned with error, response status = %s",
              trans.get_response_string().c_str());
      SC_REPORT_ERROR("TLM-2", txt);
    }

    // otherwise read parameters and print status message
    
#ifdef DEBUG
    tlm::tlm_command cmd = trans.get_command();
    sc_dt::uint64 adr = trans.get_address();
    int *ptr = reinterpret_cast<int *>(trans.get_data_ptr());
    cout << "check, trans = { " << (cmd ? 'W' : 'R') << ", " << hex << adr
         << " } , data = " << hex << m_data << " at time " << sc_time_stamp()
         << endl;
#endif
    // Allow the memory manager to free the transaction object
    trans.release();
  }

private:
  int rand_ps() {
    int n = rand() % 100;
    n = n * n * n;
    return n / 100;
  }

  mm m_mm;
  tlm::tlm_generic_payload *request_in_progress;
  sc_event end_request_event;
  tlm_utils::peq_with_cb_and_phase<Master> m_peq;

  int m_data = 0;
};

#endif
