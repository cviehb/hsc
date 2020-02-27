#ifndef MASTER_H_
#define MASTER_H_

#include <systemc.h>

using namespace sc_core;
using namespace sc_dt;
using namespace std;

#include <chrono>

#include <tlm.h>
#include <tlm_utils/simple_initiator_socket.h>
#include <tlm_utils/simple_target_socket.h>

#include "bus_spec.h"

// uncomment if necessary
#define ACURATE_TIMED
// #define DEBUG
#define TIME_MEASURE

/* Master module:*/
struct Master : sc_module {
public:
  // initiator soket -> using default settings
  tlm_utils::simple_initiator_socket<Master> socket;

  // CTOR
  SC_CTOR(Master) : socket("wishbone_bus_rw"), transactions(1000000) {
    // register process
    SC_THREAD(master_process);
  }

  /*********************************************************
          Process of the module
  *********************************************************/
  void master_process() {
    // payload object
    tlm::tlm_generic_payload *trans = new tlm::tlm_generic_payload;
    // set DELAY of transactionm, reference from slave
    sc_time DELAY = sc_time(10, SC_NS);
    // TLM command object
    tlm::tlm_command cmd;

    int const local_time = time(0);
    int seed = local_time;
    ofstream logfile;

    // check for predefined seed
    if (given_seed)
      seed = given_seed;

    std::string logfile_name =
        "tlm_block_master_slave_" + std::to_string(seed) + ".log";
    logfile.open(logfile_name.c_str());
    // clear old logs if present
    logfile.clear();
    std::srand(seed);

    std::cout << "Run with Seed: " << seed << std::endl;
#ifdef ACURATE_TIMED
    std::cout << "Acurate Timed Model!" << std::endl;
#else
    std::cout << "Loosly Timed Model!" << std::endl;
#endif

#ifdef TIME_MEASURE
    auto stw_start = std::chrono::system_clock::now();
#endif

    // do some read/write opertions
    for (int i = 0; i < transactions; i++) {
      data_write = rand();
      // prepare transaction -> set parameter
      doPrepareTransaction(trans);
      // set specific parameters
      cmd = tlm::TLM_WRITE_COMMAND;
      trans->set_command(cmd); // write cmd
      address = rand() % bus_spec::memory_size;

      trans->set_address(address); // address for access
      // pointer to a data buffer
      trans->set_data_ptr(reinterpret_cast<unsigned char *>(&data_write));

      // call b_transport function of target ->
      // implemented by the target, executed by the initiator
      socket->b_transport(*trans, DELAY);

      // Initiator obliged to check response status and DELAY
      if (trans->is_response_error()) {
        SC_REPORT_ERROR("TLM-2 Response Error from b_transport",
                        trans->get_response_string().c_str());
      }

      debugTransaction(cmd, i, data_write);

#ifdef ACURATE_TIMED
      // Realize the DELAY annotated onto the transport call
      wait(DELAY);
#else
      // accumulate bus transactions
      loosely_timed += DELAY;
#endif

      // Read back written data
      // prepare transaction -> set parameter
      doPrepareTransaction(trans);
      // set specific parameter for read operation
      cmd = tlm::TLM_READ_COMMAND;
      trans->set_command(cmd);     // read cmd
      trans->set_address(address); // address for access
      // pointer to a data buffer
      data_read = 0;
      trans->set_data_ptr(reinterpret_cast<unsigned char *>(&data_read));

      // call b_transport function of target ->
      // implemented by the target, executed by the initiator
      socket->b_transport(*trans, DELAY);

      // Initiator obliged to check response status and DELAY
      if (trans->is_response_error()) {
        SC_REPORT_ERROR("TLM-2 Response Error from b_transport",
                        trans->get_response_string().c_str());
      }

      debugTransaction(cmd, i, data_read);

#ifdef ACURATE_TIMED
      // Realize the DELAY annotated onto the transport call
      wait(DELAY);
#else
      // accumulate bus transactions
      loosely_timed += DELAY;
#endif
      // check if transaction is valid
      check_transaction(logfile, address, data_write, data_read);
    }

    // wait after all transactions done
    // speedup of simulation
#ifndef ACURATE_TIMED
    wait(loosely_timed);
#endif

#ifdef TIME_MEASURE
    auto stw_end = std::chrono::system_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
        stw_end - stw_start);
    std::cout << "Time: " << elapsed.count() << " ms\n";
#endif

    check_logs(logfile_name);
    // finished all transactions
    std::cout << "Occured Deltas: " << sc_delta_count() << std::endl;
    sc_core::sc_stop();
  }

private:
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

  void dumpMemory(tlm::tlm_generic_payload *trans) {

    // *********************************************
    // Use debug transaction interface to dump memory contents, reusing same
    // transaction object
    // *********************************************

    trans->set_address(0);
    trans->set_read();
    trans->set_data_length(128);

    unsigned char *data = new unsigned char[128];
    trans->set_data_ptr(data);

    unsigned int n_bytes = socket->transport_dbg(*trans);

    for (unsigned int i = 0; i < n_bytes; i += 4) {
      cout << "mem[" << i
           << "] = " << *(reinterpret_cast<unsigned int *>(&data[i])) << endl;
    }
  }

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

private:
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
  sc_time DELAY;
  int transactions;
  int data_write;
  int data_read;
  sc_dt::uint64 address;
  sc_time loosely_timed = sc_time(0, SC_NS);
  int given_seed = 0;
};

#endif
