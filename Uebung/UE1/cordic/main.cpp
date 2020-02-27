#include <systemc.h>

#include "cordicModule.h"
#include "testbench.h"

int sc_main(int, char *[]) {
  // signals to connect ports
  sc_signal<sc_dt::sc_ufixed<cordic::phi_size, 1, sc_dt::sc_q_mode::SC_RND,
                             sc_dt::sc_o_mode::SC_SAT>>
      sig_phi;
  sc_signal<sc_dt::sc_ufixed<cordic::xcos_size, 0, sc_dt::sc_q_mode::SC_RND,
                             sc_dt::sc_o_mode::SC_SAT>>
      sig_xcos;
  sc_signal<sc_dt::sc_ufixed<cordic::ysin_size, 0, sc_dt::sc_q_mode::SC_RND,
                             sc_dt::sc_o_mode::SC_SAT>>
      sig_ysin;
  sc_signal<bool> sig_start;
  sc_signal<bool> sig_rdy;

  // a simple adder process
  cordicModule myCordic("myCordic");
  myCordic.phi(sig_phi);
  myCordic.xcos(sig_xcos);
  myCordic.ysin(sig_ysin);
  myCordic.start(sig_start);
  myCordic.rdy(sig_rdy);

  // testbench process
  testbench stimul("stimul");
  // connect ports with adder
  stimul.result_cos_i(sig_xcos);
  stimul.result_sin_i(sig_ysin);
  stimul.corner_o(sig_phi);
  stimul.start(sig_start);
  stimul.rdy(sig_rdy);

  // Generate a VCD-Tracefile
  sc_trace_file *my_trace_file;
  my_trace_file = sc_create_vcd_trace_file("cordic_trace");
  my_trace_file->delta_cycles(true); // enable tracing of delta cycles
  // Note: in VCD files, delta cylces are shown as timestamps!!

  // signals which should be traced
  sc_trace(my_trace_file, myCordic.phi, "phi");
  sc_trace(my_trace_file, myCordic.xcos, "xcos");
  sc_trace(my_trace_file, myCordic.ysin, "ysin");
  sc_trace(my_trace_file, myCordic.start, "start");
  sc_trace(my_trace_file, myCordic.rdy, "rdy");

  sc_start();
  // sc_start(200, SC_NS);
  // close trace file
  sc_close_vcd_trace_file(my_trace_file);

  return 0;
}
