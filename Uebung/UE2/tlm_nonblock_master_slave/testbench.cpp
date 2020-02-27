#include "testbench.h"
#include "globals.h"
#define DEBUG
#define AT

void Testbench::set_seed(int seed) { given_seed = seed; }

void Testbench::check_logs(std::string const &str) {
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

// stimuli function
void Testbench::stimulus() {
  int const local_time = time(0);
  int seed = local_time;
  ofstream logfile;
#ifdef AT
  std::string logfile_name =
      "tlm_nb_master_slave_AT" + std::to_string(local_time) + ".log";
#endif

#ifndef AT
  std::string logfile_name =
      "tlm_nb_master_slave_AT" + std::to_string(local_time) + ".log";
#endif

  cout << "STEP" << endl;
  logfile.open(logfile_name.c_str());
  // clear old logs if present
  logfile.clear();

  // check for predefined seed
  if (given_seed)
    seed = given_seed;

  cout << "Test tlm_nb bus with tests: " << ten_to_p6 << endl;
  cout << "Run with seed: " << seed << endl;

  logfile << "Test tlm_nb bus with tests: " << ten_to_p6 << endl;
  logfile << "Run with seed: " << seed << endl;

  // initialize rand with seed
  std::srand(seed);

  // initialize test
  wait(SC_ZERO_TIME);
  for (int test = 0; test < testcases; test++) {
    string timingstate = "";
    bool at_test;
    long int akk_waits;
    at_test = true;
    if (test < 1) { // LT-Test
      akk_waits = 0;
      at_test = false;
      timingstate = "LT-TEST";
      logfile << "LT-TEST " << endl;
    } else {
      timingstate = "AT-TEST";
      logfile << "AT-TEST " << endl;
    }
    logfile << "###############################################################"
               "############"
            << endl;
    logfile << "TIMING-TEST: " << timingstate << endl;
    logfile << "---------------------------------------------------------------"
               "------------"
            << endl;
    logfile << "##  SEQ-WRITE " << endl;
    logfile << "" << endl;
    auto stw_start = std::chrono::system_clock::now();
    // run tests in loop with rand vals
    for (size_t i = 0; i < MEM_LEN; ++i) {
      int write_data;
      int address;

      cleanup_in.write(false);
      delay_in.write(0);
      // address = std::rand() % MEM_LEN;
      address = i;
      write_data = i;

      // do write
      data_in.write(write_data);
      adr_in.write(address);
      write.write(true);
      start.write(true);
      wait(SC_ZERO_TIME);

      // handshake
      wait(rdy.posedge_event());
      start.write(false);

      int ran;
      ran = rand();
      if (at_test) {
        wait(ran, SC_NS);
      } else {
        akk_waits += ran;
        wait(SC_ZERO_TIME);
      }
    }
    logfile << "---------------------------------------------------------------"
               "------------"
            << endl;
    logfile << "" << endl;
    logfile << "##  SEQ-READ " << endl;
    logfile << "" << endl;
    for (size_t i = 0; i < MEM_LEN; ++i) {
      int address;
      int read_data = 0;

      // address = std::rand() % MEM_LEN;
      address = i;

      delay_in.write(0);
      // do read
      adr_in.write(address);
      write.write(false);
      start.write(true);
      wait(SC_ZERO_TIME);
      // handshake & read
      wait(rdy.posedge_event());
      read_data = data_out.read();
      start.write(false);
      int ran;
      ran = rand();
      if (at_test) {
        wait(ran, SC_NS);
      } else {
        akk_waits += ran;
        wait(SC_ZERO_TIME);
      }

      // validate access
      if (read_data == (int)i)
        logfile << "Assert " << address << " EQUALS " << read_data << " TRUE "
                << endl;
      else
        logfile << "Assert " << address << " EQUALS: " << read_data << endl;
    }
    int w_data[MEM_LEN];
    int r_data[MEM_LEN];
    for (int i = 0; i < MEM_LEN; i++) {
      w_data[i] = i;
      r_data[i] = i;
    }
    logfile << "---------------------------------------------------------------"
               "------------"
            << endl;
    logfile << "##  TENTOp6_RANDOM " << endl;
    logfile << "" << endl;
    for (size_t i = 0; i < ten_to_p6; ++i) {
      int address;

      address = (rand() % MEM_LEN - 1);

      data_in.write(address);
      if (i + 1 == ten_to_p6) {
        cleanup_in.write(true);
      } else {
        cleanup_in.write(false);
      }

      delay_in.write(0);
      // do read
      adr_in.write(address);
      write.write(rand() % 2);
      start.write(true);
      wait(SC_ZERO_TIME);
      // handshake & read
      wait(rdy.posedge_event());
      r_data[address] = data_out.read();
      start.write(false);
      int ran;
      ran = rand();
      if (at_test) {
        wait(ran, SC_NS);
      } else {
        akk_waits += ran;
        wait(SC_ZERO_TIME);
      }
    }
    for (int i = 0; i < MEM_LEN; i++) {
      if (w_data[i] - r_data[i] == 0)
        logfile << "Assert " << w_data[i] << " EQUALS " << r_data[i] << " TRUE "
                << endl;
      //" AT -> "
      //<< at_test << endl;
      else
        logfile << "Assert " << w_data[i] << " EQUALS " << r_data[i]
                << " FALSE " << endl;
    }
    uint64 dc;
    if (!at_test) {
      wait(akk_waits, SC_NS);
       dc = sc_delta_count();
    }else{
       dc = sc_delta_count();
    }
    auto stw_end = std::chrono::system_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
        stw_end - stw_start);
    std::cout << "Time: " << elapsed.count() << " [ms] at " << timingstate << endl;
    std::cout << "Cycles: " << dc << " [deltacycles] at " << timingstate << endl;
    logfile << "---------------------------------------------------------------"
               "------------"
            << endl;
    logfile << "---------------------------------------------------------------"
               "------------"
            << endl;
    logfile << timingstate << "-Test took: " << elapsed.count() << " [ms] at " << timingstate  << endl;
    logfile << timingstate << "-Test took: " << dc << " [deltacycles] at " << timingstate  << endl;
    logfile << "###############################################################"
               "############"
            << endl;
  }
  logfile.close();
  check_logs(logfile_name);

  // terminate when finished
  sc_stop();
}
