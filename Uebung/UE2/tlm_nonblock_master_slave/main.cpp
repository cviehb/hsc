#include <systemc.h>
using namespace sc_core;
using namespace sc_dt;
using namespace std;

#include "testbench.h"
#include "master.h"
#include "slave.h"
#include "tlm2_base_protocol_checker.h"

int sc_main(int argc, char *argv[]) {


  Master *tlm_master;
  Slave *slave;
  tlm_utils::tlm2_base_protocol_checker<> *checker;
  tlm_master = new Master("initiator");
  slave  = new Slave("target");
  checker = new tlm_utils::tlm2_base_protocol_checker<>("checker");
  tlm_utils::num_checks = unsigned(-1);

    // Bind initiator socket to target socket
  tlm_master->socket.bind(checker->target_socket);
  checker->initiator_socket(slave->socket);


  //tb tlm sig
  sc_signal<bool> tb_start;
  sc_signal<bool> tb_rdy;
  sc_signal<bool> tb_write;
  sc_signal<int> tb_adr_i;
  sc_signal<int> tb_data_i;
  sc_signal<int> tb_data_o;
  sc_signal<int> tb_delay_i;
  sc_signal<bool> tb_cleanup_i;

   // tb proc
  Testbench stimul("stimul");

  stimul.start(tb_start);
  stimul.rdy(tb_rdy);
  stimul.write(tb_write);
  stimul.adr_in(tb_adr_i);
  stimul.data_in(tb_data_i);
  stimul.data_out(tb_data_o);
  stimul.delay_in(tb_delay_i);
  stimul.cleanup_in(tb_cleanup_i);


  // tlm_nb
  tlm_master->tb_start(tb_start);
  tlm_master->tb_write(tb_write);
  tlm_master->tb_adr_i(tb_adr_i);
  tlm_master->tb_data_i(tb_data_i);
  tlm_master->tb_data_o(tb_data_o);
  tlm_master->tb_delay_i(tb_delay_i);
  tlm_master->tb_cleanup_i(tb_cleanup_i);
  tlm_master->tb_rdy(tb_rdy);


  // Generate a VCD-Tracefile
  sc_trace_file *nb_trace_file;
  nb_trace_file = sc_create_vcd_trace_file("tlm_nb_master_slave_trace");
  // nb_trace_file->delta_cycles(true); // enable tracing of delta cycles
  // nb_trace_file->set_time_unit(1.0,SC_PS);  

  // signals which should be traced
  sc_trace(nb_trace_file, tlm_master->tb_start, "tb_start");
  sc_trace(nb_trace_file, tlm_master->tb_rdy, "tb_rdy");
  sc_trace(nb_trace_file, tlm_master->tb_write, "tb_write");
  sc_trace(nb_trace_file, tlm_master->tb_adr_i, "tb_adr_i");
  sc_trace(nb_trace_file, tlm_master->tb_data_i, "tb_data_i");
  sc_trace(nb_trace_file, tlm_master->tb_data_o, "tb_data_o");

  sc_start();
  // close trace file
  sc_close_vcd_trace_file(nb_trace_file);
  return 0;

}
