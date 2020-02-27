#ifndef WB_SLAVE_INCLUDED
#define WB_SLAVE_INCLUDED

#include <array>
#include <algorithm>

#include <systemc.h>
#include <sysc/datatypes/fx/sc_ufixed.h>

#include "wb_master.h"

// wishbone slave (memory)
SC_MODULE(wb_slave) {
  sc_in<bool> clk_i;
  sc_in<int> adr_i;
  sc_in<int> data_i;
  sc_out<int> data_o;
  sc_in<bool> we_i;
  sc_in<bool> cyc_i;
  sc_in<bool> stb_i;
  sc_in<int> sel_i;
  sc_out<bool> ack_o;
  
  void do_memory();
  
  SC_CTOR(wb_slave) {
    SC_THREAD(do_memory);
    sensitive << clk_i << stb_i << cyc_i << sel_i;
    ack_o.initialize(false);
  }
  
 private:
  void init_memory(int const val = 0);
  bool write_memory(size_t const adr, int const data);
  bool read_memory(size_t const adr, int & data);
  
  void start_condition();
  void stop_condition();
  
  // abstract memory
  std::array<int, wishbone::max_memory> m_memory;
};

#endif // WB_SLAVE_INCLUDED
