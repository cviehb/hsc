#ifndef SLAVE_CORDIC_H_
#define SLAVE_CORDIC_H_

#include <systemc.h>
using namespace sc_core;
using namespace sc_dt;
using namespace std;

#include <tlm.h>
#include <tlm_utils/simple_initiator_socket.h>
#include <tlm_utils/simple_target_socket.h>

#include "bus_spec.h"
#include "cordicModule.h"

/* Slave_Cordic module: */
struct Slave_Cordic : sc_module {
public:
  // target socket using default settings
  tlm_utils::simple_target_socket<Slave_Cordic> socket;

  // signals to connect ports
  sc_signal<sc_dt::sc_ufixed<phi_size, 1, sc_dt::sc_q_mode::SC_RND,
                             sc_dt::sc_o_mode::SC_SAT>>
      sig_phi;
  sc_signal<sc_dt::sc_ufixed<xcos_size, 0, sc_dt::sc_q_mode::SC_RND,
                             sc_dt::sc_o_mode::SC_SAT>>
      sig_xcos;
  sc_signal<sc_dt::sc_ufixed<ysin_size, 0, sc_dt::sc_q_mode::SC_RND,
                             sc_dt::sc_o_mode::SC_SAT>>
      sig_ysin;
  sc_signal<bool> sig_start;
  sc_signal<bool> sig_rdy;

  cordicModule *myCordic;

  // CTOR
  SC_HAS_PROCESS(Slave_Cordic);
  Slave_Cordic(sc_module_name instance)
      : socket("wishbone_bus_rw"), LATENCY(10, SC_NS) {
    // register callback for incoming b_transport interface method call
    socket.register_b_transport(this, &Slave_Cordic::b_transport);
    // register callback for incoming transport_dbg interface method call
    socket.register_transport_dbg(this, &Slave_Cordic::transport_dbg);

    SC_THREAD(process);
    sensitive << sig_rdy << sig_xcos << sig_ysin;
    dont_initialize();

    myCordic = new cordicModule("mycordic");
    myCordic->phi(sig_phi);
    myCordic->xcos(sig_xcos);
    myCordic->ysin(sig_ysin);
    myCordic->start(sig_start);
    myCordic->rdy(sig_rdy);

    // Initialize memory with zeros
    for (unsigned i = 0; i < mem.size(); i++)
      mem[i] = 0;
  }

  void process() {
    while (true) {
      // update memory when finished
      // if (sig_rdy.read() == 1) {
      if (sig_rdy.read()) {
        uint16_t sin = (float)(sig_ysin.read()) * (1 << 15);
        uint16_t cos = (float)(sig_xcos.read()) * (1 << 15);
        char *ptr = (char *)&mem[bus_spec::offset_xy];
        memcpy(ptr, (char *)&cos, 2);
        memcpy(ptr + 2, (char *)&sin, 2);
        mem[bus_spec::offset_ctrl] = 1;
      }
      // break to receive tlm reads
      wait(1, SC_NS);
    }
  }

  /*********************************************************
              TLM 2 blocking transport method
      *********************************************************/
  virtual void b_transport(tlm::tlm_generic_payload &trans, sc_time &delay) {
    // read parameters of transaction object
    tlm::tlm_command cmd = trans.get_command();
    sc_dt::uint64 adr = trans.get_address();
    // unsigned int *  ptr = reinterpret_cast<unsigned int
    // *>(trans.get_data_ptr());
    unsigned char *ptr = trans.get_data_ptr();
    unsigned int len = trans.get_data_length();
    unsigned char *byt = trans.get_byte_enable_ptr();
    unsigned int wid = trans.get_streaming_width();

    // decode transaction and check parameters
    if (adr >= sc_dt::uint64(mem.size())) {
      trans.set_response_status(tlm::TLM_ADDRESS_ERROR_RESPONSE);
      SC_REPORT_ERROR(
          "TLM-2", "Target does not support given generic payload transaction");
      return;
    }

    if (byt != 0) {
      trans.set_response_status(tlm::TLM_BYTE_ENABLE_ERROR_RESPONSE);
      return;
    }

    if (len > 4 || wid < len) {
      trans.set_response_status(tlm::TLM_BURST_ERROR_RESPONSE);
      return;
    }

    // read or write data -> can be copied by memcpy or direct access via
    // pointer
    if (cmd == tlm::TLM_READ_COMMAND) {
      // *ptr = mem[adr];				// in this case we need
      // the typecast when reading the parameter
      memcpy(ptr, &mem[adr], len);
    } else if (cmd == tlm::TLM_WRITE_COMMAND) {
      sig_start.write(false);
      wait(1, SC_NS);
      // mem[adr] = *ptr;
      memcpy(&mem[adr], ptr, len);
      // clear rdy flag
      mem[bus_spec::offset_ctrl] = 0;
      float phi;
      memcpy(&phi, &mem[adr], sizeof(float));
      sig_phi.write(phi);
      sig_start.write(true);
      wait(SC_ZERO_TIME);
    }

    // set random wait states from slave_cordic
    delay = sc_time(rand() % 10, SC_NS);

    // finaly set the response status attribute of the gerneric payload to
    // indicate successful and complete transaction
    trans.set_response_status(tlm::TLM_OK_RESPONSE);
  }

  /*********************************************************
          TLM 2: blocking transport debug method
  **********************************************************/
  virtual unsigned int transport_dbg(tlm::tlm_generic_payload &trans) {
    tlm::tlm_command cmd = trans.get_command();
    sc_dt::uint64 adr = trans.get_address() / 4;
    unsigned char *ptr = trans.get_data_ptr();
    unsigned int len = trans.get_data_length();

    // check for address and size
    unsigned int num_bytes = (len < mem.size() - adr) ? len : mem.size() - adr;

    if (cmd == tlm::TLM_READ_COMMAND) {
      memcpy(ptr, &mem[adr], num_bytes);
    } else if (cmd == tlm::TLM_WRITE_COMMAND) {
      memcpy(&mem[adr], ptr, num_bytes);
    }

    return num_bytes;
  }

private:
  // latency for direct memory access
  const sc_time LATENCY;
  // memory
  memory_t mem;
};

#endif
