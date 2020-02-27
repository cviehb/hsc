#ifndef TESTBENCH_H_
#define TESTBENCH_H_

#include <chrono>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <math.h>
#include <string>
#include <time.h>

#include <sysc/datatypes/fx/sc_ufixed.h>
#include <systemc.h>

#include "wb_master.h"

using namespace std;

// testbench
SC_MODULE(testbench) {
  sc_in<bool> clk;
  sc_out<bool> start;
  sc_in<bool> rdy;
  sc_out<bool> write;
  sc_out<int> adr;
  sc_in<int> data_in;
  sc_out<int> data_out;

  void stimulus();
  void set_seed(int seed);
  void check_logs(std::string const &str);

  SC_CTOR(testbench) {
    SC_THREAD(stimulus);
    sensitive << clk << rdy;
    start.initialize(false);
  }

private:
  void test_transaction(std::ofstream & logfile, int address, int write_data);
  // all seeds available except 0
  size_t given_seed = 0;
};

#endif
