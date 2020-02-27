#include "testbench.h"

// #define DEBUG

void testbench::set_seed(int seed) { given_seed = seed; }

void testbench::check_logs(std::string const &str) {
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

void testbench::test_transaction(ofstream &logfile, int address,
                                 int write_data) {
  int read_data;

  // do write
  data_out.write(write_data);
  adr.write(address);
  write.write(true);
  start.write(true);
  wait(SC_ZERO_TIME);

  // handshake
  wait(rdy.posedge_event());
  start.write(false);
  wait(1, SC_NS);

  // do read
  write.write(false);
  start.write(true);
  wait(SC_ZERO_TIME);

  // handshake & read
  wait(rdy.posedge_event());
  read_data = data_in.read();
  start.write(false);
  wait(1, SC_NS);

  // validate access
  if (read_data == write_data)
    logfile << "wb_access: adr " << address << " with data " << read_data
            << endl;
  else
    logfile << "wb_access ERROR: adr " << address
            << " mismatch wr: " << write_data << "rd: " << read_data << endl;
}

// stimuli function
void testbench::stimulus() {
  size_t const num_tests = std::pow(10, 6);
  int const local_time = time(0);
  int seed = local_time;
  ofstream logfile;

  // check for predefined seed
  if (given_seed)
    seed = given_seed;

  std::string logfile_name =
      "cycle_master_slave_" + std::to_string(seed) + ".log";

  wait(clk.posedge_event());

  logfile.open(logfile_name.c_str());
  // clear old logs if present
  logfile.clear();

  cout << "Test wishbone bus with tests: " << num_tests << endl;
  cout << "Run with seed: " << seed << endl;

  logfile << "Test wishbone bus with tests: " << num_tests << endl;
  logfile << "Run with seed: " << seed << endl;

  // initialize rand with seed
  std::srand(seed);

  // initialize test
  wait(SC_ZERO_TIME);

  auto stw_start = std::chrono::system_clock::now();

  // run linear access test all memoryplaces
  for (size_t i = 0; i < wishbone::max_memory; ++i) {
    test_transaction(logfile, i, i);
  }

  // random test
  for (size_t i = 0; i < num_tests; ++i) {
    test_transaction(logfile, std::rand() % wishbone::max_memory, std::rand());
  }

  auto stw_end = std::chrono::system_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
      stw_end - stw_start);
  std::cout << "Time: " << elapsed.count() << " ms\n";

  logfile.close();
  check_logs(logfile_name);

  // terminate when finished
  std::cout << "Occured Deltas: " << sc_delta_count() << std::endl;
  sc_stop();
}
