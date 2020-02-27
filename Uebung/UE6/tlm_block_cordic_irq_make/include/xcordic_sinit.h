#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "xparameter.h"

#ifndef XCORDIC_SINIT_H_
#define XCORDIC_SINIT_H_

#define XPAR_XCORDIC_NUM_INSTANCES 1
#define XST_DEVICE_NOT_FOUND 4711

typedef struct {
  u16 DeviceId; /**< Unique ID of device */
  u32 BaseAddr; /**< Base address of the device */
} XCordic_Config;

/**
 * The XScuTimer driver instance data. The user is required to allocate a
 * variable of this type for every timer device in the system.
 * A pointer to a variable of this type is then passed to the driver API
 * functions.
 */
typedef struct {
  XCordic_Config Config; /**< Hardware Configuration */
  u32 IsReady;           /**< Device is initialized and ready */
  u32 IsStarted;         /**< Device timer is running */
} XCordic;

#ifdef __cplusplus
extern "C" {
#endif

int XCordic_CfgInitialize(XCordic *InstancePtr, XCordic_Config *ConfigPtr,
                          u32 BaseAddr);
XCordic_Config *XCordic_LookupConfig(u16 DeviceId);
int XCordic_Initialize(XCordic *InstancePtr, u16 DeviceId);

#ifdef __cplusplus
}
#endif

#endif
