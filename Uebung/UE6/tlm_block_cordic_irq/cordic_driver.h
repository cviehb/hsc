#ifndef CORDIC_DRIVER_H_
#define CORDIC_DRIVER_H_

#include "bus_spec.h"
#include "hal_cordic.h"

#include "xcordic_sinit.h"
#include "xscugic.h"

#include <math.h>

#define MAX_ERROR_TXT_LENGTH 64
#define FIXED_POINT_FRACTIONAL_BITS 15

typedef uint16_t fixed_point_t;

typedef struct {
  int status;
  char txt[64];
} Cordic_Status_t;

double fixed_to_float(fixed_point_t input);
fixed_point_t float_to_fixed(double input);

void CordicIntrHandler(void *data);

Cordic_Status_t CordicSetupIntrSystem(XScuGic *IntcInstancePtr,
                                      XCordic *CordicInstancePtr,
                                      u16 CordicIntrId);

void CordicDisableIntrSystem(XScuGic *IntcInstancePtr, u16 CordicIntrId);

Cordic_Status_t init_cordic(XScuGic *IntcInstancePtr,
                            XCordic *CordicInstancePtr, u16 CordicDeviceId,
                            u16 CordicIntrId);

/* call hardware block (hal) here and do transactions */
/* similar in systemc and firmware */
Cordic_Status_t cordic_calc_xy(float const *const phi, float *const cos,
                               float *const sin, unsigned int *adr);

#endif
