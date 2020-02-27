#include "cordicModule.h"
#include <algorithm>
#include <math.h>

void cordicModule::compute() {
  double op_phi;
  double res_xcos;
  int const op_N = cordic::calc_count;
  double res_ysin;
  short int d{0};

  // Set Constants
  const size_t XY_ARY_DIMENSION{2};
  const double TAN[] = {0.78540, 0.46365, 0.24498, 0.12435, 0.06242, 0.03124,
                       0.01562, 0.00781, 0.00391, 0.00195, 0.00098, 0.00049,
                       0.00024, 0.00012, 0.00006, 0.00003};
  
  // do calc every pos edge
  while (true) {
    // wait for event
    wait(start.posedge_event());
    rdy.write(false);
    wait(SC_ZERO_TIME);

    op_phi = phi.read().to_double();

    // Initialize locals
    // Set the vector length
    xy.resize(XY_ARY_DIMENSION);
    for_each(xy.begin(), xy.end(), [&](auto &elem) { elem.resize(op_N + 1); });
    z.resize(op_N + 1);

    // Initialize the vectors with zeros
    for_each(xy.begin(), xy.end(),
             [&](auto &elem) { std::fill(elem.begin(), elem.end(), 0.0); });
    std::fill(z.begin(), z.end(), 0.0);

    // Initialize Member
    K = 1;
    z[0] = op_phi;// / 180 * 3.1415;
    xy[0][0] = 1.0;

    // Indexingvariable for the loop
    // used after loop again
    int n{0};
    for (n = 0; n < op_N; n++) {

      // Set direction of rotation
      if (z[n] < 0)
        d = -1;
      else
        d = 1;

      // Calc. the gain-factor
      K = K * 1 / std::sqrt(1 + std::pow(2, -2 * n));
      // Calc. the next z for direction
      z[n + 1] = z[n] - TAN[n] * d;

      // Calc. each approx for cos as x in xy[0][i] and sin as y in xy[1][i]
      xy[0][n + 1] = 1 * xy[0][n] + (-d * std::pow(2, -1 * (n))) * xy[1][n];
      xy[1][n + 1] = 1 * xy[0][n] * (d * std::pow(2, -1 * (n))) + xy[1][n];
    }

    // Temp store the trig-func-approx result.
    // Last approx. times the gain results in cos and sin.
    res_xcos = xy[0][n] * K;
    res_ysin = xy[1][n] * K;

    // Write temp store on Systemc-var - out
    xcos.write(res_xcos);
    ysin.write(res_ysin);

    // handshake
    rdy.write(true);
    wait(SC_ZERO_TIME);
  }
}
