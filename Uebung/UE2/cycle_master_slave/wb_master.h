#ifndef WB_MASTER_INCLUDED
#define WB_MASTER_INCLUDED

#include <sysc/datatypes/fx/sc_ufixed.h>
#include <systemc.h>
#include <vector>

namespace wishbone {
size_t const max_memory = 4096;
size_t const max_wait_states = 10;
}; // namespace wishbone

// wishbone master simulation
SC_MODULE(wb_master) {
  // connection to bus interface
  sc_in<bool> clk_i;
  sc_out<int> adr_o;
  sc_in<int> data_i;
  sc_out<int> data_o;
  sc_out<bool> we_o;
  sc_out<bool> cyc_o;
  sc_out<bool> stb_o;
  sc_out<int> sel_o;
  sc_in<bool> ack_i;

  // connection to testbench
  sc_in<bool> tb_start;
  sc_out<bool> tb_rdy;
  sc_in<bool> tb_write;
  sc_in<int> tb_adr_i;
  sc_in<int> tb_data_i;
  sc_out<int> tb_data_o;

  void transfer();

  SC_CTOR(wb_master) {
    SC_THREAD(transfer);
    sensitive << tb_start << ack_i;
    tb_rdy.initialize(false);
  }

private:
  void write_transfer();
  void read_transfer();
};

#endif // WB_MASTER_INCLUDED
