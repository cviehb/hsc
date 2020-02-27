#include "wb_slave.h"

void wb_slave::init_memory(int const val) {}

bool wb_slave::write_memory(size_t const adr, int const data) {
  if (adr > m_memory.size())
    return false;
  
  m_memory[adr] = data;
  return true;
}

bool wb_slave::read_memory(size_t const adr, int &data) {
  data = 0;

  if (adr > m_memory.size())
    return false;

  data = m_memory[adr];
  return true;
}

void wb_slave::start_condition() {
  while(!cyc_i.read() && !stb_i.read())
    wait(clk_i.posedge_event());
} 

void wb_slave::stop_condition() {
  ack_o.write(true);
  wait(clk_i.posedge_event());
  ack_o.write(false);
  wait(clk_i.posedge_event());
}

void wb_slave::do_memory() {
  size_t wait_time;
  init_memory();

  while (true) {
    start_condition();

    if (we_i.read()) {
      write_memory(adr_i.read(), data_i.read());
    } else {
      int data;
      read_memory(adr_i.read(), data);
      data_o.write(data);
    }

    wait_time = std::rand() % wishbone::max_wait_states;
    for (size_t i = 0; i < wait_time; i++)
      wait(clk_i.posedge_event());

    stop_condition();
  }
}
