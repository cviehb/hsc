#ifndef SLAVE_CORDIC_H_
#define SLAVE_CORDIC_H_

#include <systemc.h>
using namespace sc_core;
using namespace sc_dt;
using namespace std;

#include <tlm.h>
#include <tlm_utils/simple_initiator_socket.h>
#include <tlm_utils/simple_target_socket.h>

// #define DEBUG
// #define FLOATING_POINT_CORDIC
#define GOLDEN_MODELL_MATH

#include "bus_spec.h"
#ifdef FLOATING_POINT_CORDIC
#include "cc_cordicModule.h"
#else
#include "cc_cordicModule_fixed.h"
#endif
#include "cordicModule.h"

/* Slave_Cordic module: */
struct Slave_Cordic : sc_module {
public:
  // target socket using default settings
  tlm_utils::simple_target_socket<Slave_Cordic> socket;

  // ## IRQ
  sc_out<bool> sig_irq;

  // ## CORDIC_GLOBAL
  sc_signal<bool> sig_start;
  bus_spec::phi_t sig_phi;

  // ## MODULE::CORDIC_MODULE
  // signals to connect ports
  bus_spec::sincos_t sig_xcos;
  bus_spec::sincos_t sig_ysin;
  sc_signal<bool> sig_rdy;

  cordicModule *myCordic;

  // ## MODULE::CC_CORDIC_MODULE prefix cc_
  // signals to connect ports
  sc_in<bool> cc_sig_clk;
  sc_in<bool> cc_sig_nrst;

  bus_spec::phi_t cc_sig_phi;
  bus_spec::sincos_t cc_sig_xcos;
  bus_spec::sincos_t cc_sig_ysin;

  sc_signal<bool> cc_sig_rdy;

#ifdef FLOATING_POINT_CORDIC
  cc_cordicModule *ccCordic;
#else
  cc_cordicModule_fixed *ccCordic;
#endif

  // CTOR
  SC_HAS_PROCESS(Slave_Cordic);
  Slave_Cordic(sc_module_name instance)
      : socket("wishbone_bus_rw"), LATENCY(10, SC_NS) {
    // register callback for incoming b_transport interface method call
    socket.register_b_transport(this, &Slave_Cordic::b_transport);
    // register callback for incoming transport_dbg interface method call
    socket.register_transport_dbg(this, &Slave_Cordic::transport_dbg);

    SC_THREAD(process);
    sensitive << sig_rdy << cc_sig_rdy;
    dont_initialize();

    myCordic = new cordicModule("mycordic");
    myCordic->phi(sig_phi);
    myCordic->xcos(sig_xcos);
    myCordic->ysin(sig_ysin);
    myCordic->start(sig_start);
    myCordic->rdy(sig_rdy);

#ifdef FLOATING_POINT_CORDIC
    ccCordic = new cc_cordicModule("cccordic");
#else
    ccCordic = new cc_cordicModule_fixed("cccordic");
#endif
    ccCordic->phi(sig_phi);
    ccCordic->xcos(cc_sig_xcos);
    ccCordic->ysin(cc_sig_ysin);
    ccCordic->clk(cc_sig_clk);
    ccCordic->nrst(cc_sig_nrst);
    ccCordic->start(sig_start);
    ccCordic->rdy(cc_sig_rdy);

    // Initialize memory with zeros
    for (unsigned i = 0; i < mem.size(); i++)
      mem[i] = 0;
  }

  bool checker(float cordic_sin, float cc_cordic_sin, float cordic_cos,
               float cc_cordic_cos) {
    float cos_diff = cordic_cos - cc_cordic_cos;
    float sin_diff = cordic_sin - cc_cordic_sin;

    if (cos_diff < 0.0) {
      cos_diff = cos_diff * (-1.0);
    }
    if (sin_diff < 0.0) {
      sin_diff = sin_diff * (-1.0);
    }

#ifdef DEBUG
    printf("diff: sin%f cos%f\n", cos_diff, sin_diff);
    return true;
#endif

#ifndef DEBUG
    return (std::abs(cos_diff) < EPSYLON && std::abs(sin_diff) < EPSYLON);
#endif
  }

  void process() {
    while (true) {

      // update memory when finished
      if (sig_rdy.read() && cc_sig_rdy.read() && !check_flag) {
        float cordic_sin = sig_ysin.read().to_float();
        float cordic_cos = sig_xcos.read().to_float();

        float cc_cordic_sin = cc_sig_ysin.read().to_float();
        float cc_cordic_cos = cc_sig_xcos.read().to_float();

#ifdef GOLDEN_MODELL_MATH
        if (!checker(std::sin(sig_phi.read().to_double()), cc_cordic_sin,
                     std::cos(sig_phi.read().to_double()), cc_cordic_cos))
          printf("Error Precision: sin:%f-%f, cos:%f-%f, pre:%f\n",
                 std::sin(sig_phi.read().to_double()), cc_cordic_sin,
                 std::cos(sig_phi.read().to_double()), cc_cordic_cos, EPSYLON);
#else
        if (!checker(cordic_sin, cc_cordic_sin, cordic_cos, cc_cordic_cos))
          printf("Error Precision: sin:%f-%f, cos:%f-%f, pre:%f\n", cordic_sin,
                 cc_cordic_sin, cordic_cos, cc_cordic_cos, EPSYLON);
#endif

        /* we use the clocked values as in real hw */
        uint16_t sin = (float)(cc_sig_ysin.read()) * (1 << 15);
        uint16_t cos = (float)(cc_sig_xcos.read()) * (1 << 15);
        char *ptr = (char *)&mem[bus_spec::offset_xy];
        memcpy(ptr, (char *)&cos, 2);
        memcpy(ptr + 2, (char *)&sin, 2);
        mem[bus_spec::offset_ctrl] = 1;
        check_flag = true;
        sig_irq.write(true);
        wait(SC_ZERO_TIME);
      }
      // break to receive tlm reads
      wait(1, SC_NS);
      if (sig_irq.read() == true) {
        sig_irq.write(false);
      }
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
      sig_start.write(true);
      // mem[adr] = *ptr;
      memcpy(&mem[adr], ptr, len);
      // clear rdy flag
      mem[bus_spec::offset_ctrl] = 0;
      float phi;
      memcpy(&phi, &mem[adr], sizeof(float));
      sig_phi.write(phi);
      /* at least on clk cycle */
      wait(2, SC_NS);
      check_flag = false;
      sig_start.write(false);
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
  const float EPSYLON = std::pow(2, -14);
  // latency for direct memory access
  const sc_time LATENCY;
  bool check_flag;
  // memory
  memory_t mem;
};

#endif
