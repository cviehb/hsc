#include "main_c.h"
#include "cordic_driver.c"
#include "xcordic_sinit.h"
#include "xscugic.h"

#include <math.h>

#define MAX_GRAD 360
#define CORDIC_UNIT_ADDR 0

double to_rad(double const input) { return input / 180.0 * M_PI; }
double to_grad(double const input) { return input / M_PI * 180.0; }

void CordicIntrHandler(void *data) { /* dummy handler for cordic */
}

int CordicSetupIntrSystem(XScuGic *IntcInstancePtr, XCordic *CordicInstancePtr,
                          u16 CordicIntrId) {
  int Status;

  XScuGic_Config *IntcConfig;

  IntcConfig = XScuGic_LookupConfig(INTC_DEVICE_ID);
  if (NULL == IntcConfig) {
    return XST_FAILURE;
  }

  Status = XScuGic_CfgInitialize(IntcInstancePtr, IntcConfig,
                                 IntcConfig->CpuBaseAddress);
  if (Status != XST_SUCCESS) {
    return XST_FAILURE;
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
    return Status;
  }

  /*
   * Enable the interrupt for the device.
   */
  XScuGic_Enable(IntcInstancePtr, CordicIntrId);

#ifndef TESTAPP_GEN
  /*
   * Enable interrupts in the Processor.
   */
  Xil_ExceptionEnable();
#endif

  return XST_SUCCESS;
}

void CordicDisableIntrSystem(XScuGic *IntcInstancePtr, u16 CordicIntrId) {
  XScuGic_Disconnect(IntcInstancePtr, CordicIntrId);
}

int init_cordic(XScuGic *IntcInstancePtr, XCordic *CordicInstancePtr,
                u16 CordicDeviceId, u16 CordicIntrId) {
  int Status = XST_SUCCESS;
  XCordic_Config *ConfigPtr;

  ConfigPtr = XCordic_LookupConfig(CordicDeviceId);

  Status =
      XCordic_CfgInitialize(CordicInstancePtr, ConfigPtr, ConfigPtr->BaseAddr);
  if (Status != XST_SUCCESS) {
    return XST_FAILURE;
  }

  Status =
      CordicSetupIntrSystem(IntcInstancePtr, CordicInstancePtr, CordicIntrId);
  if (Status != XST_SUCCESS) {
    return XST_FAILURE;
  }

  return Status;
} // init_cordic

/* firmeware main */
int main_c() {
  float angular;
  float sin, cos;
  unsigned int memory_mapped_address = CORDIC_UNIT_ADDR;
  int *IntcInstancePtr = NULL;
  int *CordicInstancePtr = NULL;
  int CordicDeviceId = 0;
  int CordicIntrId = 0;

  init_cordic((XScuGic *)IntcInstancePtr, (XCordic *)CordicInstancePtr,
              CordicDeviceId, CordicIntrId);

  /* do firmware calc and gen all values */
  for (int i = 0; i <= MAX_GRAD; ++i) {
    sin = 0;
    cos = 0;
    angular = (float)i;

    angular = to_rad(angular);
    cordic_calc_xy(&angular, &cos, &sin, &memory_mapped_address);

    printf("calc: ang%f cos%f sin%f\n", to_grad(angular), cos, sin);
  }

  CordicDisableIntrSystem((XScuGic *)IntcInstancePtr, CordicIntrId);

  return 0;
}
