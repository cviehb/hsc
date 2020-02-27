#include "cordic_driver.h"

/// Fixed-point Format: 11.5 (16-bit)

#include "bus_spec.h"
#include "hal_cordic.h"

#include "xcordic_sinit.h"
#include "xscugic.h"

#include <math.h>
#ifdef EMU_CPU
#include <systemc.h>
#endif

#define MAX_ERROR_TXT_LENGTH 64
#define FIXED_POINT_FRACTIONAL_BITS 15

static bool irq_occured = false;
static bool irq_enabled = false;

typedef uint16_t fixed_point_t;

double fixed_to_float(fixed_point_t input) {
  return ((double)input / (double)(1 << FIXED_POINT_FRACTIONAL_BITS));
}

fixed_point_t float_to_fixed(double input) {
  return (fixed_point_t)(input * (1 << FIXED_POINT_FRACTIONAL_BITS));
}

void CordicIntrHandler(void *data) {
  /* dummy handler for cordic */
  if (irq_enabled)
    irq_occured = true;
}

Cordic_Status_t CordicSetupIntrSystem(XScuGic *IntcInstancePtr,
                                      XCordic *CordicInstancePtr,
                                      u16 CordicIntrId) {
  s32 Status;
  Cordic_Status_t ret_stat;
  XScuGic_Config *IntcConfig;

  IntcConfig = XScuGic_LookupConfig(INTC_DEVICE_ID);
  if (NULL == IntcConfig) {
    ret_stat.status = XST_FAILURE;
    strcpy(ret_stat.txt, "XScuGic_LookupConfig failed.");
  }

  Status = XScuGic_CfgInitialize(IntcInstancePtr, IntcConfig,
                                 IntcConfig->CpuBaseAddress);
  if (Status != XST_SUCCESS) {
    ret_stat.status = XST_FAILURE;
    strcpy(ret_stat.txt, "XScuGic_CfgInitialize failed.");
  }

  Xil_ExceptionInit();

  /*
   * Connect the interrupt controller interrupt handler to the hardware
   * interrupt handling logic in the processor.
   */
  Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_IRQ_INT,
                               (Xil_ExceptionHandler)XScuGic_InterruptHandler,
                               IntcInstancePtr);

  /*
   * Connect the device driver handler that will be called when an
   * interrupt for the device occurs, the handler defined above performs
   * the specific interrupt processing for the device.
   */
  Status = XScuGic_Connect(IntcInstancePtr, CordicIntrId,
                           (Xil_ExceptionHandler)CordicIntrHandler,
                           (void *)CordicInstancePtr);
  if (Status != XST_SUCCESS) {
    ret_stat.status = XST_FAILURE;
    strcpy(ret_stat.txt, "XScuGic_Connect failed.");
  }

  /*
   * Enable the interrupt for the device.
   */
  XScuGic_Enable(IntcInstancePtr, CordicIntrId);

#ifdef EMU_CPU
  irq_enabled = true;
#endif

#ifndef TESTAPP_GEN
  /*
   * Enable interrupts in the Processor.
   */
  Xil_ExceptionEnable();
#endif

  return ret_stat;
}

void CordicDisableIntrSystem(XScuGic *IntcInstancePtr, u16 CordicIntrId) {
  XScuGic_Disconnect(IntcInstancePtr, CordicIntrId);
#ifdef EMU_CPU
  irq_enabled = false;
#endif
}

Cordic_Status_t init_cordic(XScuGic *IntcInstancePtr,
                            XCordic *CordicInstancePtr, u16 CordicDeviceId,
                            u16 CordicIntrId) {
  int Status = XST_SUCCESS;
  Cordic_Status_t ret_stat;
  XCordic_Config *ConfigPtr;

  ConfigPtr = XCordic_LookupConfig(CordicDeviceId);

  Status =
      XCordic_CfgInitialize(CordicInstancePtr, ConfigPtr, ConfigPtr->BaseAddr);
  if (Status != XST_SUCCESS) {
    ret_stat.status = XST_FAILURE;
    strcpy(ret_stat.txt, "XCordic_CfgInitialize failed.");
    return ret_stat;
  }

  return CordicSetupIntrSystem(IntcInstancePtr, CordicInstancePtr,
                               CordicIntrId);
} // init_cordic

int PhiIsValid(float const *const phi) {
  const float MAX_VAL = 360.0;
  if (phi > 0) {
    if ((MAX_VAL - *phi) > 0.0) {
      return 1;
    } else {
      return -1;
    }
  } else {
    if ((*phi - MAX_VAL) < 0.0) {
      return 1;
    } else {
      return -1;
    }
  }
}

Cordic_Status_t cordic_calc_xy(float const *const phi, float *const cos,
                               float *const sin, unsigned int *adr) {
  int data;
  int ctrl = 0;
  uint16_t buff;
  char *cpy_ptr;
  float cut_phi;
  uint8_t map;
  unsigned int int_phi;
  Cordic_Status_t ret_stat;
  ret_stat.status = XST_SUCCESS;

  if (PhiIsValid(phi) < 0) {
    ret_stat.status = XST_SUCCESS;
    strcpy(ret_stat.txt, "Phi was out of bounds.");
    return ret_stat;
  }

  /* at first map the angular to 1st quarter cause cordic is only defined at 0
   * ...90 degrees if a value higher than 2 Pi is given it will be cutted */
  cut_phi = *phi;
  cut_phi = roundf(cut_phi * (1 << 16)) / (1 << 16);

  cut_phi = fmodf(cut_phi, M_PI * 2);
  map = cut_phi / (M_PI / 2);
  cut_phi = fmodf(cut_phi, M_PI / 2);

  /* write angular */
  memcpy(&int_phi, (unsigned int *)&cut_phi, sizeof(int));
  cordic_wr_phi((*adr) + bus_spec::offset_phi, int_phi);

  if (irq_enabled) {
    while (!irq_occured)
#ifdef EMU_CPU
      wait(1, SC_NS);
#endif
  } else {
    /* check flag (busy waiting) */
    while (!ctrl) {
      ctrl = cordic_rd_ctl((*adr) + bus_spec::offset_ctrl);
    }
  }
  /* read back results */
  data = cordic_rd_xy((*adr) + bus_spec::offset_xy);
  cpy_ptr = ((char *)&data);
  irq_occured = false;
  
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
    *sin = 1.0 - *sin;
    /* negate + invert cos */
    *cos = (1.0 - *cos) * -1.0;
    break;
  }
  case 2: {
    /* negate sin */
    *sin *= -1.0;
    /* negate cos */
    *cos *= -1.0;
    break;
  }
  case 3: {
    /* negate + invert sin */
    *sin = (1.0 - *sin) * -1.0;
    /* invert cos */
    *cos = 1 - *cos;
    break;
  }
  default:
    /* cannot be reached normaly */
    break;
  }

  return ret_stat;
  /* Debug feature: :) */
  /* printf("cmod: cos%f sin%f dat %d\n", *cos, *sin, data); */
}
