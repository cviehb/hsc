#include "main_c.h"
#include "cordic_driver.c"

#include <math.h>

#define MAX_GRAD 360
#define CORDIC_UNIT_ADDR 0

double to_rad(double const input) { return input / 180.0 * M_PI; }
double to_grad(double const input) { return input / M_PI * 180.0; }

/* firmeware main */
int main_c() {
  float angular;
  float sin, cos;
  unsigned int memory_mapped_address = CORDIC_UNIT_ADDR;

  /* do firmware calc and gen all values */
  for (int i = 0; i <= MAX_GRAD; ++i) {
    sin = 0;
    cos = 0;
    angular = (float)i;

    angular = to_rad(angular);
    cordic_calc_xy(&angular, &cos, &sin, &memory_mapped_address);

    printf("calc: ang%f cos%f sin%f\n", to_grad(angular), cos, sin);
  }

  return 0;
}
