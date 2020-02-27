#include "cc_cordicModule_fixed.h"
#include <algorithm>
#include <math.h>

/*
let emacs do the atan look up table calculation

(defun calculate_atan_times (num)
  (interactive)
  (let ((i 1)
        (val 1.0))
    (while (< i (+ num 1))
      (insert "\n")
      (insert (number-to-string (atan val)))
      (insert ",")
      (setq val (/ val 2.0))
      (setq i (+ i 1))
      )))
*/

//#define ALTERED_CALC

void cc_cordicModule_fixed::compute() {
  state = IDLE;
  /* reset stops here till behaviour changes */
  wait();

  /* will be called every clock edge,
     implement with a state and if else like vhdl */

  while (true) {
    /* simple fsm */
    switch (state) {
    case IDLE: {
      /* we know begin the calc */
      if (start.read() == true) {
        state = CALC;
        cycle = 0;
        r_phi = phi.read();
        /* initalize start values */
        r_cos = 1.0;
        r_sin = 0.0;
        K = 1;
        rdy.write(false);
        wait(SC_ZERO_TIME);
      }
    } break;
    case CALC: {
      /* check for start down */
      if (cycle < bus_spec::calc_count) {
        K = K * 1 / std::sqrt(1 + std::pow(2, -2 * cycle));
        /* caclulate gain one time at fixed iteration count save ressources*/
        if (r_phi < 0.0) {
          /* std::pow(2, cycle) can be replaced with >> operator, when using
           * fixed point */
          r_next_cos = r_cos + (r_sin >> cycle);
          r_next_sin = r_sin - (r_cos >> cycle);
          r_phi = r_phi + atan_lut[cycle];
          r_cos = r_next_cos;
          r_sin = r_next_sin;
        } else {
          r_next_cos = r_cos - (r_sin >> cycle);
          r_next_sin = r_sin + (r_cos >> cycle);
          r_phi = r_phi - atan_lut[cycle];
          r_cos = r_next_cos;
          r_sin = r_next_sin;
        }
        /* increase counter after calc */
        cycle++;
      } else {
        /* write to out */
        xcos.write(r_cos * K);
        ysin.write(r_sin * K);
        /* enable handshake */
        rdy.write(true);
        state = IDLE;
      }
    } break;
    }
    /* wait on clk */
    wait();
  }
}
