#ifndef CORDICMODULE_H_INCLUDED
#define CORDICMODULE_H_INCLUDED

#include <sysc/datatypes/fx/sc_ufixed.h>
#include <systemc.h>
#include <vector>

namespace cordic {
size_t const phi_size = 22;
size_t const xcos_size = 16;
size_t const ysin_size = 16;
size_t const calc_count = 16;
}; // namespace cordic

// a simple ader module
SC_MODULE(cordicModule) {
  sc_in<sc_dt::sc_ufixed<cordic::phi_size, 1, sc_dt::sc_q_mode::SC_RND,
                         sc_dt::sc_o_mode::SC_SAT>>
      phi;
  sc_out<sc_dt::sc_ufixed<cordic::xcos_size, 0, sc_dt::sc_q_mode::SC_RND,
                          sc_dt::sc_o_mode::SC_SAT>>
      xcos;
  sc_out<sc_dt::sc_ufixed<cordic::ysin_size, 0, sc_dt::sc_q_mode::SC_RND,
                          sc_dt::sc_o_mode::SC_SAT>>
      ysin;
  sc_in<bool> start;
  sc_out<bool> rdy;

  void compute();

  SC_CTOR(cordicModule) : phi("op1"), xcos("res1"), ysin("res2") {
    SC_THREAD(compute);
    sensitive << start << phi;
    rdy.initialize(false);
  }

private:
  std::vector<std::vector<double>> xy;
  std::vector<double> z;
  double K;
};

#endif // CORDICMODULE_H_INCLUDED
