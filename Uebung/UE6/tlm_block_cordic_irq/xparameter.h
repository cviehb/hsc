#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#ifndef XPARAMETER_H_
#define XPARAMETER_H_

/*
 * The maximum number of interrupts supported by the hardware.
 */
#ifdef __ARM_NEON__
#define XSCUGIC_MAX_NUM_INTR_INPUTS                                            \
  95U /* Maximum number of interrupt defined by Zynq */
#else
#define XSCUGIC_MAX_NUM_INTR_INPUTS                                            \
  195U /* Maximum number of interrupt defined by Zynq Ultrascale Mp */
#endif

#define INTC_DEVICE_ID 0
#define XIL_EXCEPTION_ID_IRQ_INT 5U

typedef int32_t s32;
typedef uint32_t u32;
typedef uint16_t u16;

#endif
