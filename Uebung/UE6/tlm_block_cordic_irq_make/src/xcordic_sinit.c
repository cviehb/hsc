#include "xcordic_sinit.h"

static XCordic_Config XCordic_ConfigTable[0] = {{0}};

XCordic_Config *XCordic_LookupConfig(u16 DeviceId) {
  XCordic_Config *ConfigPtr = NULL;

  int Index;

  for (Index = 0; Index < XPAR_XCORDIC_NUM_INSTANCES; Index++) {
    if (XCordic_ConfigTable[Index].DeviceId == DeviceId) {
      ConfigPtr = &XCordic_ConfigTable[Index];
      break;
    }
  }
  return ConfigPtr;
}

int XCordic_Initialize(XCordic *InstancePtr, u16 DeviceId) {
  XCordic_Config *ConfigPtr;

  ConfigPtr = XCordic_LookupConfig(DeviceId);
  if (ConfigPtr == NULL) {
    InstancePtr->IsReady = 0;
    return (XST_DEVICE_NOT_FOUND);
  }
  return XCordic_CfgInitialize(InstancePtr, ConfigPtr);
}
