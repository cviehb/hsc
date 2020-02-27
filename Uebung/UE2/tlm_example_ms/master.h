#ifndef MASTER_H_
#define MASTER_H_

#include <systemc.h>
using namespace sc_core;
using namespace sc_dt;
using namespace std;

#include <tlm.h>
#include <tlm_utils/simple_initiator_socket.h>
#include <tlm_utils/simple_target_socket.h>

/* Master module:*/
struct Master : sc_module {
public:
  // initiator soket -> using default settings
  tlm_utils::simple_initiator_socket<Master> socket;

  // CTOR
  SC_CTOR(Master) : socket("bus_rw") {
    // register process
    SC_THREAD(master_process);
    // register invalidate_direct_mem_ptr method
    socket.register_invalidate_direct_mem_ptr(
        this, &Master::invalidate_direct_mem_ptr);
  }

  /*********************************************************
          Process of the module
  *********************************************************/
  void master_process() {
    // payload object
    tlm::tlm_generic_payload *trans = new tlm::tlm_generic_payload;
    // set delay of transaction
    sc_time delay = sc_time(10, SC_NS);
    // TLM command object
    tlm::tlm_command cmd;

    // do some read/write opertions
    for (int i = 0; i < 256; i++) {
      data = 0xFF000000 | i;
      // prepare transaction -> set parameter
      doPrepareTransaction(trans);
      // set specific parameters
      cmd = tlm::TLM_WRITE_COMMAND;
      trans->set_command(cmd); // write cmd
      trans->set_address(i);   // address for access
      // pointer to a data buffer
      trans->set_data_ptr(reinterpret_cast<unsigned char *>(&data));

      // call b_transport function of target ->
      // implemented by the target, executed by the initiator
      socket->b_transport(*trans, delay);

      // Initiator obliged to check response status and delay
      if (trans->is_response_error()) {
        char txt[100];
        sprintf(txt, "Error from b_transport, response status = %s",
                trans->get_response_string().c_str());
        SC_REPORT_ERROR("TLM-2", "Response error from b_transport");
      }

      cout << "trans = { " << (cmd ? 'W' : 'R') << ", " << hex << i
           << " } , data = " << hex << data << " at time " << sc_time_stamp()
           << " delay = " << delay << endl;

      // Realize the delay annotated onto the transport call
      wait(delay);

      // Read back written data
      // prepare transaction -> set parameter
      doPrepareTransaction(trans);
      // set specific parameter for read operation
      cmd = tlm::TLM_READ_COMMAND;
      trans->set_command(cmd); // read cmd
      trans->set_address(i);   // address for access
      // pointer to a data buffer
      trans->set_data_ptr(reinterpret_cast<unsigned char *>(&data));

      // call b_transport function of target ->
      // implemented by the target, executed by the initiator
      socket->b_transport(*trans, delay);

      // Initiator obliged to check response status and delay
      if (trans->is_response_error()) {
        char txt[100];
        sprintf(txt, "Error from b_transport, response status = %s",
                trans->get_response_string().c_str());
        SC_REPORT_ERROR("TLM-2", "Response error from b_transport");
      }

      cout << "trans = { " << (cmd ? 'W' : 'R') << ", " << hex << i
           << " } , data = " << hex << data << " at time " << sc_time_stamp()
           << " delay = " << delay << endl;

      // Realize the delay annotated onto the transport call
      wait(delay);
    }

    // do direct memory access
    // but check dmi access permission first
    if (trans->is_dmi_allowed()) {
      dmi_ptr_valid = socket->get_direct_mem_ptr(*trans, dmi_data);

      // perform some accesses via dmi interface
      for (int i = 0; i < 32; i++) {
        // Bypass transport interface and use direct memory interface
        // Implement target latency

        // set TLM command for read access
        cmd = tlm::TLM_READ_COMMAND;
        // check if read via dmi is allowed
        assert(dmi_data.is_read_allowed());
        // copy data
        memcpy(&data, dmi_data.get_dmi_ptr() + i, 4);
        // wait the specified delay
        wait(dmi_data.get_read_latency());

        cout << "DMI   = { " << (cmd ? 'W' : 'R') << ", " << hex << i
             << " } , data = " << hex << data << " at time " << sc_time_stamp()
             << endl;

        // set TLM command for read access
        cmd = tlm::TLM_WRITE_COMMAND;
        // check if write access is allowed
        assert(dmi_data.is_write_allowed());
        // copy data
        memcpy(dmi_data.get_dmi_ptr() + i, &data, 4);
        // wait the specified delay
        wait(dmi_data.get_write_latency());

        cout << "DMI   = { " << (cmd ? 'W' : 'R') << ", " << hex << i
             << " } , data = " << hex << data << " at time " << sc_time_stamp()
             << endl;
      }
    }

    // *********************************************
    // Use debug transaction interface to dump memory contents, reusing same
    // transaction object
    // *********************************************

    trans->set_address(0);
    trans->set_read();
    trans->set_data_length(128);

    unsigned char *data = new unsigned char[128];
    trans->set_data_ptr(data);

    unsigned int n_bytes = socket->transport_dbg(*trans);

    for (unsigned int i = 0; i < n_bytes; i += 4) {
      cout << "mem[" << i
           << "] = " << *(reinterpret_cast<unsigned int *>(&data[i])) << endl;
    }
  }

  /*********************************************************
          TLM 2: method to invalidate direct memory access
  *********************************************************/
  virtual void invalidate_direct_mem_ptr(sc_dt::uint64 start_range,
                                         sc_dt::uint64 end_range) {
    dmi_ptr_valid = false;
  }

private:
  void doPrepareTransaction(tlm::tlm_generic_payload *trans) {
    // configure standard set of attributes
    // Initialize 8 out of the 10 attributes, byte_enable_length and
    // extensions being unused
    trans->set_data_length(4);        // length of data in bytes
    trans->set_streaming_width(4);    // width of streaming burst, for
                                      // non streaming set value equal
                                      // to data length
    trans->set_byte_enable_length(0); // must not be set because the
                                      // ptr is set to 0
    trans->set_byte_enable_ptr(0);    // set to 0 to indicate that byte
                                      // enables are unused
    trans->set_dmi_allowed(false);    // will may be set by the target
                                      // to indicate a DMI (direct
                                      // memory interface)
    // status may be set by the target
    trans->set_response_status(tlm::TLM_INCOMPLETE_RESPONSE);
  }

  // Internal data buffer used by initiator with generic payload
  int data;
  bool dmi_ptr_valid;
  tlm::tlm_dmi dmi_data;
};

#endif
