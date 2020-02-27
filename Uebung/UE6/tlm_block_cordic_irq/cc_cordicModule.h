#ifndef CC_CORDICMODULE_H_INCLUDED
#define CC_CORDICMODULE_H_INCLUDED

#include <sysc/datatypes/fx/sc_ufixed.h>
#include <systemc.h>
#include <vector>

/*namespace cordic {
size_t const phi_size = 22;
size_t const xcos_size = 16;
size_t const ysin_size = 16;
size_t const calc_count = 16;
}; // namespace cordic*/

static size_t const cc_phi_size = 22;
static size_t const cc_xcos_size = 16;
static size_t const cc_ysin_size = 16;
static size_t const cc_calc_count = 16;

enum cordic_state { IDLE = 0, CALC };

// a simple ader module
SC_MODULE(cc_cordicModule) {
public:
  sc_in<sc_dt::sc_ufixed<cc_phi_size, 1, sc_dt::sc_q_mode::SC_RND,
                         sc_dt::sc_o_mode::SC_SAT>>
      phi;
  sc_out<sc_dt::sc_ufixed<cc_xcos_size, 0, sc_dt::sc_q_mode::SC_RND,
                          sc_dt::sc_o_mode::SC_SAT>>
      xcos;
  sc_out<sc_dt::sc_ufixed<cc_ysin_size, 0, sc_dt::sc_q_mode::SC_RND,
                          sc_dt::sc_o_mode::SC_SAT>>
      ysin;
  sc_in<bool> start;
  sc_out<bool> rdy;

  // clock and reset
  sc_in<bool> clk;
  sc_in<bool> nrst;

  SC_CTOR(cc_cordicModule) {
    // use compute as thread function, trigger on positive edge
    SC_CTHREAD(compute, clk.pos());
    // low-active async reset
    async_reset_signal_is(nrst, false);
    sensitive << start << phi;
    rdy.initialize(false);
  }

  /* clocked cycle function */
  void compute();

private:
  /* internal statemachine*/
  cordic_state state;
  uint8_t cycle;

  /* radian variables */
  double r_phi, r_next_phi, r_sin, r_cos, r_next_sin, r_next_cos;
  double K, div;

  const double atan_lut[cc_calc_count] = {
      0.7853981633974483,     0.4636476090008061,     0.24497866312686414,
      0.12435499454676144,    0.06241880999595735,    0.031239833430268277,
      0.015623728620476831,   0.007812341060101111,   0.0039062301319669718,
      0.0019531225164788188,  0.0009765621895593195,  0.0004882812111948983,
      0.00024414062014936177, 0.00012207031189367021, 0.00006103515617420877,
      0.000030517578115526096};
};

#endif // CC_CORDICMODULE_H_INCLUDED
