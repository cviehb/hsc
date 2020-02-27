#ifndef CC_CORDICMODULE_FIXED_H_INCLUDED
#define CC_CORDICMODULE_FIXED_H_INCLUDED

#include <systemc.h>
#include <vector>

#include "bus_spec.h"

/*namespace cordic {
size_t const phi_size = 22;
size_t const xcos_size = 16;
size_t const ysin_size = 16;
size_t const calc_count = 16;
}; // namespace cordic*/

enum cordic_state { IDLE = 0, CALC };

// a simple ader module
SC_MODULE(cc_cordicModule_fixed) {
public:
  bus_spec::phi_t_i phi;
  bus_spec::sincos_t_o xcos;
  bus_spec::sincos_t_o ysin;
  sc_in<bool> start;
  sc_out<bool> rdy;

  // clock and reset
  sc_in<bool> clk;
  sc_in<bool> nrst;

  SC_CTOR(cc_cordicModule_fixed) {
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
  /* fixed precision */
  static uint8_t const frac = 21;
  static uint8_t const mant = 2;
  typedef sc_dt::sc_fixed<frac, mant> fx_t;
  /* internal statemachine*/
  cordic_state state;
  uint8_t cycle;

  /* radian variables */
  fx_t r_phi, r_next_phi, r_sin, r_cos, r_next_sin, r_next_cos;
  double K, div;

  const fx_t atan_lut[bus_spec::calc_count] = {
      0.78539816339744827899949086713605, 0.46364760900080609351547877849953,
      0.24497866312686414347332686247682, 0.12435499454676143815667899161781,
      0.06241880999595735002305474381501, 0.03123983343026827744215445648024,
      0.01562372862047683129416153491320, 0.00781234106010111114398730691732,
      0.00390623013196697175739013907503, 0.00195312251647881875843415500071,
      0.00097656218955931945943649274966, 0.00048828121119489828992621394121,
      0.00024414062014936177124474481204, 0.00012207031189367020785306594544,
      0.00006103515617420877259350145416, 0.00003051757811552609572715473452};
};

#endif // CC_CORDICMODULE_H_INCLUDED
