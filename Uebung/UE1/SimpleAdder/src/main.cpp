#include "testbench.h"
#include "adder.h"

int sc_main( int, char*[]){
  // signals to connect ports
  sc_signal<int> sig_a, sig_b, sig_c;
  sc_signal<int> sig_d, sig_e, sig_f, sig_g;

  // a simple adder process
  adder myAdder("myAdder");
  myAdder(sig_a, sig_b, sig_c);

  // a add3 process
  Add3 myAdd3("Add3");
  myAdd3.in1(sig_d);
  myAdd3.in2(sig_e);
  myAdd3.in3(sig_f);
  myAdd3.sum(sig_g);

  // testbench process 
  testbench stimul("stimul");
  // connect ports with adder
  stimul.op1(sig_a);
  stimul.op2(sig_b);
  stimul.result(sig_c);
  // connect ports with add3
  stimul.op1_add3(sig_d);
  stimul.op2_add3(sig_e);
  stimul.op3_add3(sig_f);
  stimul.res_add3(sig_g);

  //Generate a VCD-Tracefile
  sc_trace_file *my_trace_file;
  my_trace_file = sc_create_vcd_trace_file("adder_trace");
  my_trace_file->delta_cycles(true);   // enable tracing of delta cycles
  // Note: in VCD files, delta cylces are shown as timestamps!!

  //signals which should be traced
  sc_trace(my_trace_file,myAdder.a, "adder_op1");
  sc_trace(my_trace_file,myAdder.b, "adder_op2");
  sc_trace(my_trace_file,myAdder.c, "adder_res");

  sc_trace(my_trace_file,myAdd3.in1, "add3_op1");
  sc_trace(my_trace_file,myAdd3.in2, "add3_op2");
  sc_trace(my_trace_file,myAdd3.in3, "add3_op3");
  sc_trace(my_trace_file,myAdd3.sum, "add3_res");
    
  // run stimuli for 200 ns
  sc_start(200, SC_NS);

  // close trace file
  sc_close_vcd_trace_file(my_trace_file);

  return 0;
}
