#ifndef EMU_CPU_H_
#define EMU_CPU_H_

#include <systemc.h>

using namespace sc_core;
using namespace sc_dt;
using namespace std;

#include <chrono>

#include <tlm.h>
#include <tlm_utils/simple_initiator_socket.h>
#include <tlm_utils/simple_target_socket.h>

#include "bus_spec.h"
// #include "main_c.c"

// uncomment if necessary
// #define DEBUG

/* Emu_Cpu module:*/
class Emu_Cpu : sc_module {
public:
  SC_HAS_PROCESS(Emu_Cpu);
  // initiator soket -> using default settings
  tlm_utils::simple_initiator_socket<Emu_Cpu> socket;

  typedef int (*main_func)();

  /* return the instance of the module */
  static Emu_Cpu *get_instance() {
    static Emu_Cpu instance("emu_cpu");
    return &instance;
  }

  /* set calling main and return instance */
  static Emu_Cpu *get_instance(main_func fnc) {
    auto local_inst = get_instance();
    local_inst->set_main(fnc);
    return local_inst;
  }

  /* set main function for specific object */
  void set_main(main_func fnc) { main_c = fnc; }

  /* prevent copying and assignment of cpu */
  Emu_Cpu(Emu_Cpu const &) = delete;
  void operator=(Emu_Cpu const &) = delete;

  /* do write on the bus */
  void write_bus(unsigned int adr, unsigned int data) {
    do_transaction(tlm::TLM_WRITE_COMMAND, adr, &data);
  }

  /* do read on bus */
  void read_bus(unsigned int adr, unsigned int *data) {
    do_transaction(tlm::TLM_READ_COMMAND, adr, data);
  }

  /* do the real transaction */
  void do_transaction(tlm::tlm_command cmd, unsigned int adr,
                      unsigned int *data) {
    // payload object
    tlm::tlm_generic_payload *trans = new tlm::tlm_generic_payload;
    // set DELAY of transactionm, reference from slave
    sc_time DELAY = sc_time(10, SC_NS);
    // simulate busy bus and wait random for access
    wait(rand() % bus_spec::max_cycles, SC_NS);

    // prepare transaction -> set parameter
    doPrepareTransaction(trans);
    trans->set_command(cmd); // write cmd
    trans->set_address(adr); // address for access
    // pointer to a data buffer
    trans->set_data_ptr(reinterpret_cast<unsigned char *>(data));

    // call b_transport function of target ->
    // implemented by the target, executed by the initiator
    socket->b_transport(*trans, DELAY);

    // Initiator obliged to check response status and DELAY
    if (trans->is_response_error()) {
      SC_REPORT_ERROR("TLM-2 Response Error from b_transport",
                      trans->get_response_string().c_str());
    }
    // wait for slave delay after trans
    wait(DELAY);
  }

  /* testbench function using firmwarecode */
  void run() {
    int ret = main_c();
    std::cout << "Firmwarecall: " << ret;
    sc_stop();
  }

private:
  // private constructor for singleton
  Emu_Cpu(sc_module_name name)
      : sc_module(name), socket("wishbone_bus_rw") {
    // register process
    SC_THREAD(run);
  }

  void doPrepareTransaction(tlm::tlm_generic_payload *trans) {
    // configure standard set of attributes
    // Initialize 8 out of the 10 attributes, byte_enable_length and
    // extensions being unused
    trans->set_data_length(4);        // length of data in bytes
    trans->set_streaming_width(4);    // width of streaming burst, for
                                      // non streaming set value equal
                                      // to data length
    trans->set_byte_enable_length(0); // must not be set because the
                                      // ptr is set to 0
    trans->set_byte_enable_ptr(0);    // set to 0 to indicate that byte
                                      // enables are unused
    trans->set_dmi_allowed(false);    // will may be set by the target
                                      // to indicate a DMI (direct
                                      // memory interface)
    // status may be set by the target
    trans->set_response_status(tlm::TLM_INCOMPLETE_RESPONSE);
  }

  /* check if transaction was successfull */
  void check_transaction(ofstream &logfile, int const address,
                         int const data_read, int const data_write) {
    if (data_read == data_write)
      logfile << "OK : Address " << address << " Data " << data_read
              << std::endl;
    else
      logfile << "ERROR : Address " << address << " Data " << data_write << "|"
              << data_read << std::endl;
  }

  void debugTransaction(tlm::tlm_command cmd, int num, int data) {
#ifdef DEBUG
    cout << "trans = { " << (cmd ? 'W' : 'R') << ", " << hex << num
         << " } , data = " << hex << data << " at time " << sc_time_stamp()
         << " DELAY = " << DELAY << endl;
#endif
  }

  // set seed from outside
  void set_seed(int seed) { given_seed = seed; }

  void check_logs(std::string const &str) {
    // check if error is present in logfile
    std::string word;
    int error = 0;
    std::ifstream logfile(str.c_str());

    if (logfile.good()) {
      cout << "Check Logfile ..." << endl;
      while (logfile >> word) {
        if (word == "ERROR") {
          error++;
        }
      }
      cout << "End of test with " << error << " Error(s)!" << endl;
    } else {
      cout << "End of test couldn't check logfile for Error(s)" << endl;
    }
  }

  // private variables
  main_func main_c;
  sc_time DELAY;
  int data_write;
  int data_read;
  int given_seed = 0;
};

#endif
