#include "wb_master.h"

void wb_master::write_transfer() {
  adr_o.write(tb_adr_i.read());
  data_o.write(tb_data_i.read());
  we_o.write(true);
  cyc_o.write(true);
  stb_o.write(true);
  
  wait(ack_i.posedge_event());
  cyc_o.write(false);
  stb_o.write(false);
  we_o.write(false);
  wait(clk_i.posedge_event());
}

void wb_master::read_transfer() {
  adr_o.write(tb_adr_i.read());
  cyc_o.write(true);
  stb_o.write(true);
  
  wait(ack_i.posedge_event());
  tb_data_o.write(data_i.read());
  cyc_o.write(false);
  stb_o.write(false);
  wait(clk_i.posedge_event());
}


void wb_master::transfer() {
  // init signals
  data_o.write(0);
  we_o.write(false);
  cyc_o.write(false);
  stb_o.write(false);
  sel_o.write(0);
  wait(SC_ZERO_TIME);
  
  while (true) {
    // start transaction
    wait(tb_start.posedge_event());
    wait(rand() % wishbone::max_wait_states, SC_NS);
    wait(clk_i.posedge_event());
    
    if(tb_write.read()) {
      // do write access
      write_transfer();
    } else {
      read_transfer();
    }
    
    // finished transfer
    tb_rdy.write(true);
    wait(SC_ZERO_TIME);
    wait(tb_start.negedge_event());
    tb_rdy.write(false);
    wait(SC_ZERO_TIME);
  }
}
