#ifndef TESTBENCH_H_
#define TESTBENCH_H_

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <math.h>
#include <sysc/datatypes/fx/sc_ufixed.h>
#include <systemc.h>
#include <time.h>

#include "cordicModule.h"

using namespace std;

// testbench for cordic
SC_MODULE(testbench) {
  sc_out<sc_dt::sc_ufixed<cordic::phi_size, 1, sc_dt::sc_q_mode::SC_RND,
                          sc_dt::sc_o_mode::SC_SAT>>
      corner_o;
  sc_in<sc_dt::sc_ufixed<cordic::xcos_size, 0, sc_dt::sc_q_mode::SC_RND,
                         sc_dt::sc_o_mode::SC_SAT>>
      result_cos_i;
  sc_in<sc_dt::sc_ufixed<cordic::ysin_size, 0, sc_dt::sc_q_mode::SC_RND,
                         sc_dt::sc_o_mode::SC_SAT>>
      result_sin_i;
  sc_out<bool> start;
  sc_in<bool> rdy;

  void stimulus();
  void set_seed(int seed);
  void check_logs(std::string const &str);

  SC_CTOR(testbench) {
    SC_THREAD(stimulus);
    sensitive << rdy << result_cos_i << result_sin_i;
    start.initialize(false);
  }

private:
  // all seeds available except 0
  size_t given_seed = 0;
};

#endif
