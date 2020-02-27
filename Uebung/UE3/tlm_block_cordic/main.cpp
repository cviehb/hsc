// Shows the direct memory interfaces and the DMI hint.
// Shows the debug transaction interface
// Shows the proper use of response status

#include <systemc.h>
using namespace sc_core;
using namespace sc_dt;
using namespace std;

#include <tlm.h>
#include <tlm_utils/simple_initiator_socket.h>
#include <tlm_utils/simple_target_socket.h>

#include "emu_cpu.h"
#include "main_c.c"
#include "slave_cordic.h"

int sc_main(int argc, char *agrv[])
{

  // create tlm connections
  Emu_Cpu *cpu;
  Slave_Cordic *slave;

  cpu = Emu_Cpu::get_instance(main_c);
  slave = new Slave_Cordic("target");
  cpu->socket.bind(slave->socket);

  sc_trace_file *my_trace_file;
  my_trace_file = sc_create_vcd_trace_file("cordic_trace");
  my_trace_file->delta_cycles(true); // enable tracing of delta cycles
  // Note: in VCD files, delta cylces are shown as timestamps!!

  // signals which should be traced
  sc_trace(my_trace_file, slave->sig_phi, "phi");
  sc_trace(my_trace_file, slave->sig_xcos, "xcos");
  sc_trace(my_trace_file, slave->sig_ysin, "ysin");
  sc_trace(my_trace_file, slave->sig_start, "start");
  sc_trace(my_trace_file, slave->sig_rdy, "rdy");

  sc_start();

  sc_close_vcd_trace_file(my_trace_file);
  return 0;
}
