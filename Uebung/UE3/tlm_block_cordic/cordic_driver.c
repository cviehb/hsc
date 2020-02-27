#include "cordic_driver.h"

#include "bus_spec.h"
#include "hal_cordic.h"

#include <string.h>
#include <math.h>
#include <stdio.h>

/// Fixed-point Format: 11.5 (16-bit)
typedef uint16_t fixed_point_t;

#define FIXED_POINT_FRACTIONAL_BITS 15

double fixed_to_float(fixed_point_t input) {
  return ((double)input / (double)(1 << FIXED_POINT_FRACTIONAL_BITS));
}

fixed_point_t float_to_fixed(double input) {
  return (fixed_point_t)(input * (1 << FIXED_POINT_FRACTIONAL_BITS));
}

void cordic_calc_xy(float const *const phi, float *const cos, float *const sin,
                    unsigned int *adr) {
  int data;
  int ctrl = 0;
  uint16_t buff;
  char *cpy_ptr;
  float cut_phi;
  uint8_t map;
  unsigned int int_phi;

  /* at first map the angular to 1st quarter cause cordic is only defined at 0
   * ...90 degrees if a value higher than 2 Pi is given it will be cutted */
  cut_phi = fmodf(*phi, M_PI * 2);
  map = cut_phi / (M_PI / 2);
  cut_phi = fmodf(cut_phi, M_PI / 2);

  /* write angular */
  memcpy(&int_phi, (unsigned int *)&cut_phi, sizeof(int));
  cordic_wr_phi((*adr) + bus_spec::offset_phi, int_phi);

  /* check flag (busy waiting) */
  while (!ctrl) {
    ctrl = cordic_rd_ctl((*adr) + bus_spec::offset_ctrl);
  }
  /* read back results */
  data = cordic_rd_xy((*adr) + bus_spec::offset_xy);
  cpy_ptr = ((char *)&data);

  /* low bits */
  memset(&buff, 0, sizeof(uint16_t));
  memcpy((char *)&buff, cpy_ptr, 2);
  *cos = fixed_to_float(buff);

  /* *cos = (float)(data & mask); */
  /* high bits */
  memset(&buff, 0, sizeof(uint16_t));
  memcpy(&buff, cpy_ptr + 2, 2);
  *sin = fixed_to_float(buff);

  /* remap the data dependent on quarter */
  switch (map) {
  case 0: {
    /* nothing has to be done */
    break;
  }
  case 1: {
    /* invert sin */
    *sin = 1 - *sin;
    /* negate + invert cos */
    *cos = (1 - *cos) * -1;
    break;
  }
  case 2: {
    /* negate sin */
    *sin *= -1;
    /* negate cos */
    *cos *= -1;
    break;
  }
  case 3: {
    /* negate + invert sin */
    *sin = (1 - *sin) * -1;
    /* invert cos */
    *cos = 1 - *cos;
    break;
  }
  default:
    /* cannot be reached normaly */
    break;
  }

  /* Debug feature: :) */
  /* printf("cmod: cos%f sin%f dat %d\n", *cos, *sin, data); */
}
