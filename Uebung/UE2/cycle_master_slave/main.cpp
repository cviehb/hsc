#include <systemc.h>

#include "testbench.h"
#include "wb_master.h"
#include "wb_slave.h"

int sc_main(int, char *[]) {
  // signals to bus interface
  sc_clock clk("sysclk", 1, SC_NS);
  sc_signal<int> adr;
  sc_signal<int> data_in;
  sc_signal<int> data_out;
  sc_signal<bool> we;
  sc_signal<bool> cyc;
  sc_signal<bool> stb;
  sc_signal<int> sel;
  sc_signal<bool> ack;

  // signals to testbench
  sc_signal<bool> tb_start;
  sc_signal<bool> tb_rdy;
  sc_signal<bool> tb_write;
  sc_signal<int> tb_adr_i;
  sc_signal<int> tb_data_i;
  sc_signal<int> tb_data_o;

  // wishbone interfaces
  wb_master my_master("wishbone_master");
  my_master.clk_i(clk);
  my_master.adr_o(adr);
  my_master.data_i(data_in);
  my_master.data_o(data_out);
  my_master.we_o(we);
  my_master.cyc_o(cyc);
  my_master.stb_o(stb);
  my_master.sel_o(sel);
  my_master.ack_i(ack);

  my_master.tb_start(tb_start);
  my_master.tb_rdy(tb_rdy);
  my_master.tb_write(tb_write);
  my_master.tb_adr_i(tb_adr_i);
  my_master.tb_data_i(tb_data_o);
  my_master.tb_data_o(tb_data_i);

  wb_slave my_memory("wishbone_slave");
  my_memory.clk_i(clk);
  my_memory.adr_i(adr);
  my_memory.data_i(data_out);
  my_memory.data_o(data_in);
  my_memory.we_i(we);
  my_memory.cyc_i(cyc);
  my_memory.stb_i(stb);
  my_memory.sel_i(sel);
  my_memory.ack_o(ack);

  // testbench process
  testbench stimul("stimul");
  stimul.clk(clk);
  stimul.start(tb_start);
  stimul.rdy(tb_rdy);
  stimul.write(tb_write);
  stimul.adr(tb_adr_i);
  stimul.data_in(tb_data_i);
  stimul.data_out(tb_data_o);

  // Generate a VCD-Tracefile
  sc_trace_file *my_trace_file;
  my_trace_file = sc_create_vcd_trace_file("cycle_master_slave_trace");
  my_trace_file->delta_cycles(true); // enable tracing of delta cycles
  // Note: in VCD files, delta cylces are shown as timestamps!!

  // signals which should be traced
  sc_trace(my_trace_file, my_master.clk_i, "clk_i");
  sc_trace(my_trace_file, my_master.adr_o, "adr_o");
  sc_trace(my_trace_file, my_master.data_i, "data_i");
  sc_trace(my_trace_file, my_master.data_o, "data_o");
  sc_trace(my_trace_file, my_master.we_o, "we_o");
  sc_trace(my_trace_file, my_master.cyc_o, "cyc_o");
  sc_trace(my_trace_file, my_master.stb_o, "stb_o");
  sc_trace(my_trace_file, my_master.sel_o, "sel_o");
  sc_trace(my_trace_file, my_master.ack_i, "ack_i");
  sc_trace(my_trace_file, my_master.tb_start, "tb_start");
  sc_trace(my_trace_file, my_master.tb_rdy, "tb_rdy");
  sc_trace(my_trace_file, my_master.tb_write, "tb_write");
  sc_trace(my_trace_file, my_master.tb_adr_i, "tb_adr_i");
  sc_trace(my_trace_file, my_master.tb_data_i, "tb_data_i");
  sc_trace(my_trace_file, my_master.tb_data_o, "tb_data_o");

  sc_start();
  // close trace file
  sc_close_vcd_trace_file(my_trace_file);

  return 0;
}
