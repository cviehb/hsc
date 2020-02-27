#ifndef CORDIC_DRIVER_H_
#define CORDIC_DRIVER_H_

/* call hardware block (hal) here and do transactions */
/* similar in systemc and firmware */
void cordic_calc_xy(float const *const phi, float *const cos, float *const sin,
                    unsigned int *adr);

#endif
