#include "cordicModule.h"
#include <algorithm>
#include <math.h>

//#define UNROLLED

void cordicModule::compute() {
  double op_phi;
  double res_xcos;
  int const op_N = calc_count;
  double res_ysin;
  short int d{0};

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
    z[0] = op_phi; // / 180 * 3.1415;
    xy[0][0] = 1.0;

#ifdef UNROLLED
    // Unrolled loop for calc
    if (z[0] < 0)
      d = -1;
    else
      d = 1;
    K = K * 1 / std::sqrt(1 + std::pow(2, -2 * 0));
    z[0 + 1] = z[0] - TAN[0] * d;
    xy[0][0 + 1] = 1 * xy[0][0] + (-d * std::pow(2, -1 * (0))) * xy[1][0];
    xy[1][0 + 1] = 1 * xy[0][0] * (d * std::pow(2, -1 * (0))) + xy[1][0];
    if (z[1] < 0)
      d = -1;
    else
      d = 1;
    K = K * 1 / std::sqrt(1 + std::pow(2, -2 * 1));
    z[1 + 1] = z[1] - TAN[1] * d;
    xy[0][1 + 1] = 1 * xy[0][1] + (-d * std::pow(2, -1 * (1))) * xy[1][1];
    xy[1][1 + 1] = 1 * xy[0][1] * (d * std::pow(2, -1 * (1))) + xy[1][1];
    if (z[2] < 0)
      d = -1;
    else
      d = 1;
    K = K * 1 / std::sqrt(1 + std::pow(2, -2 * 2));
    z[2 + 1] = z[2] - TAN[2] * d;
    xy[0][2 + 1] = 1 * xy[0][2] + (-d * std::pow(2, -1 * (2))) * xy[1][2];
    xy[1][2 + 1] = 1 * xy[0][2] * (d * std::pow(2, -1 * (2))) + xy[1][2];
    if (z[3] < 0)
      d = -1;
    else
      d = 1;
    K = K * 1 / std::sqrt(1 + std::pow(2, -2 * 3));
    z[3 + 1] = z[3] - TAN[3] * d;
    xy[0][3 + 1] = 1 * xy[0][3] + (-d * std::pow(2, -1 * (3))) * xy[1][3];
    xy[1][3 + 1] = 1 * xy[0][3] * (d * std::pow(2, -1 * (3))) + xy[1][3];
    if (z[4] < 0)
      d = -1;
    else
      d = 1;
    K = K * 1 / std::sqrt(1 + std::pow(2, -2 * 4));
    z[4 + 1] = z[4] - TAN[4] * d;
    xy[0][4 + 1] = 1 * xy[0][4] + (-d * std::pow(2, -1 * (4))) * xy[1][4];
    xy[1][4 + 1] = 1 * xy[0][4] * (d * std::pow(2, -1 * (4))) + xy[1][4];
    if (z[5] < 0)
      d = -1;
    else
      d = 1;
    K = K * 1 / std::sqrt(1 + std::pow(2, -2 * 5));
    z[5 + 1] = z[5] - TAN[5] * d;
    xy[0][5 + 1] = 1 * xy[0][5] + (-d * std::pow(2, -1 * (5))) * xy[1][5];
    xy[1][5 + 1] = 1 * xy[0][5] * (d * std::pow(2, -1 * (5))) + xy[1][5];
    if (z[6] < 0)
      d = -1;
    else
      d = 1;
    K = K * 1 / std::sqrt(1 + std::pow(2, -2 * 6));
    z[6 + 1] = z[6] - TAN[6] * d;
    xy[0][6 + 1] = 1 * xy[0][6] + (-d * std::pow(2, -1 * (6))) * xy[1][6];
    xy[1][6 + 1] = 1 * xy[0][6] * (d * std::pow(2, -1 * (6))) + xy[1][6];
    if (z[7] < 0)
      d = -1;
    else
      d = 1;
    K = K * 1 / std::sqrt(1 + std::pow(2, -2 * 7));
    z[7 + 1] = z[7] - TAN[7] * d;
    xy[0][7 + 1] = 1 * xy[0][7] + (-d * std::pow(2, -1 * (7))) * xy[1][7];
    xy[1][7 + 1] = 1 * xy[0][7] * (d * std::pow(2, -1 * (7))) + xy[1][7];
    if (z[8] < 0)
      d = -1;
    else
      d = 1;
    K = K * 1 / std::sqrt(1 + std::pow(2, -2 * 8));
    z[8 + 1] = z[8] - TAN[8] * d;
    xy[0][8 + 1] = 1 * xy[0][8] + (-d * std::pow(2, -1 * (8))) * xy[1][8];
    xy[1][8 + 1] = 1 * xy[0][8] * (d * std::pow(2, -1 * (8))) + xy[1][8];
    if (z[9] < 0)
      d = -1;
    else
      d = 1;
    K = K * 1 / std::sqrt(1 + std::pow(2, -2 * 9));
    z[9 + 1] = z[9] - TAN[9] * d;
    xy[0][9 + 1] = 1 * xy[0][9] + (-d * std::pow(2, -1 * (9))) * xy[1][9];
    xy[1][9 + 1] = 1 * xy[0][9] * (d * std::pow(2, -1 * (9))) + xy[1][9];
    if (z[10] < 0)
      d = -1;
    else
      d = 1;
    K = K * 1 / std::sqrt(1 + std::pow(2, -2 * 10));
    z[10 + 1] = z[10] - TAN[10] * d;
    xy[0][10 + 1] = 1 * xy[0][10] + (-d * std::pow(2, -1 * (10))) * xy[1][10];
    xy[1][10 + 1] = 1 * xy[0][10] * (d * std::pow(2, -1 * (10))) + xy[1][10];
    if (z[11] < 0)
      d = -1;
    else
      d = 1;
    K = K * 1 / std::sqrt(1 + std::pow(2, -2 * 11));
    z[11 + 1] = z[11] - TAN[11] * d;
    xy[0][11 + 1] = 1 * xy[0][11] + (-d * std::pow(2, -1 * (11))) * xy[1][11];
    xy[1][11 + 1] = 1 * xy[0][11] * (d * std::pow(2, -1 * (11))) + xy[1][11];
    if (z[12] < 0)
      d = -1;
    else
      d = 1;
    K = K * 1 / std::sqrt(1 + std::pow(2, -2 * 12));
    z[12 + 1] = z[12] - TAN[12] * d;
    xy[0][12 + 1] = 1 * xy[0][12] + (-d * std::pow(2, -1 * (12))) * xy[1][12];
    xy[1][12 + 1] = 1 * xy[0][12] * (d * std::pow(2, -1 * (12))) + xy[1][12];
    if (z[13] < 0)
      d = -1;
    else
      d = 1;
    K = K * 1 / std::sqrt(1 + std::pow(2, -2 * 13));
    z[13 + 1] = z[13] - TAN[13] * d;
    xy[0][13 + 1] = 1 * xy[0][13] + (-d * std::pow(2, -1 * (13))) * xy[1][13];
    xy[1][13 + 1] = 1 * xy[0][13] * (d * std::pow(2, -1 * (13))) + xy[1][13];
    if (z[14] < 0)
      d = -1;
    else
      d = 1;
    K = K * 1 / std::sqrt(1 + std::pow(2, -2 * 14));
    z[14 + 1] = z[14] - TAN[14] * d;
    xy[0][14 + 1] = 1 * xy[0][14] + (-d * std::pow(2, -1 * (14))) * xy[1][14];
    xy[1][14 + 1] = 1 * xy[0][14] * (d * std::pow(2, -1 * (14))) + xy[1][14];
    if (z[15] < 0)
      d = -1;
    else
      d = 1;
    K = K * 1 / std::sqrt(1 + std::pow(2, -2 * 15));
    z[15 + 1] = z[15] - TAN[15] * d;
    xy[0][15 + 1] = 1 * xy[0][15] + (-d * std::pow(2, -1 * (15))) * xy[1][15];
    xy[1][15 + 1] = 1 * xy[0][15] * (d * std::pow(2, -1 * (15))) + xy[1][15];

    // Temp store the trig-func-approx result.
    // Last approx. times the gain results in cos and sin.
    res_xcos = xy[0][16] * K;
    res_ysin = xy[1][16] * K;
#endif

#ifndef UNROLLED
    int n{0};
    for (n = 0; n < op_N; n++) {

      // Set direction of rotation
      if (z[n] < 0.0)
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

#endif

    // std::cout << "cordic: input" << op_phi << " cos" << res_xcos << " sin" <<
    // res_ysin << std::endl;

    // Write temp store on Systemc-var - out
    xcos.write(res_xcos);
    ysin.write(res_ysin);

    // handshake
    wait(SC_ZERO_TIME);
    rdy.write(true);
    wait(SC_ZERO_TIME);
  }
}
