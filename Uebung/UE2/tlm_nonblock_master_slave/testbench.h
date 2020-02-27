#ifndef TESTBENCH_H_
#define TESTBENCH_H_

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <math.h>
#include <time.h>
#include <string>
#include <chrono>

#include <systemc.h>

#include "globals.h"

using namespace std;

// testbench
SC_MODULE(Testbench) {
  sc_out<bool> start;
  sc_in<bool> rdy;
  sc_out<bool> write;
  sc_out<int> adr_in;
  sc_out<int> data_in;
  sc_in<int> data_out;
  sc_out<int> delay_in;
  sc_out<bool> cleanup_in;


  void stimulus();
  void set_seed(int seed);
  void check_logs(std::string const & str);

  SC_CTOR(Testbench) {
    SC_THREAD(stimulus);
    sensitive << rdy;
    start.initialize(false);
  }

private:
  // all seeds available except 0
  size_t given_seed = 0;
  size_t ten_to_p6 = 1000000;
  int testcases =2;
};

#endif
