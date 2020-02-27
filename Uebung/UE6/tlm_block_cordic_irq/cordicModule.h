#ifndef CORDICMODULE_H_INCLUDED
#define CORDICMODULE_H_INCLUDED

#include "bus_spec.h"
#include <systemc.h>
#include <vector>

/*namespace cordic {
size_t const phi_size = 22;
size_t const xcos_size = 16;
size_t const ysin_size = 16;
size_t const calc_count = 16;
}; // namespace cordic*/

// a simple ader module
SC_MODULE(cordicModule) {
public:
  bus_spec::phi_t_i phi;
  bus_spec::sincos_t_o xcos;
  bus_spec::sincos_t_o ysin;
  sc_in<bool> start;
  sc_out<bool> rdy;

  SC_CTOR(cordicModule) {
    SC_THREAD(compute);
    sensitive << start << phi;
    rdy.initialize(false);
  }

  void compute();

private:
  std::vector<std::vector<double>> xy;
  std::vector<double> z;
  double K;
  const size_t XY_ARY_DIMENSION{2};
  const double TAN[bus_spec::calc_count] = {
      0.7853981633974483,     0.4636476090008061,     0.24497866312686414,
      0.12435499454676144,    0.06241880999595735,    0.031239833430268277,
      0.015623728620476831,   0.007812341060101111,   0.0039062301319669718,
      0.0019531225164788188,  0.0009765621895593195,  0.0004882812111948983,
      0.00024414062014936177, 0.00012207031189367021, 0.00006103515617420877,
      0.000030517578115526096};
};

#endif // CORDICMODULE_H_INCLUDED
