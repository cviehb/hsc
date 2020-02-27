#include "xscugic.h"
#include <stdlib.h>

static XScuGic_Config hidden_cfg;

XScuGic_Config *XScuGic_LookupConfig(u16 DeviceId) { return &hidden_cfg; }

void XScuGic_InterruptHandler(XScuGic *InstancePtr){};

s32 XScuGic_SelfTest(XScuGic *InstancePtr) { return XST_SUCCESS; }

s32 XScuGic_Connect(XScuGic *InstancePtr, u32 Int_Id,
                    Xil_InterruptHandler Handler, void *CallBackRef) {
  return XST_SUCCESS;
}

void XScuGic_Disconnect(XScuGic *InstancePtr, u32 Int_Id){};
void XScuGic_Enable(XScuGic *InstancePtr, u32 Int_Id){};
void XScuGic_Disable(XScuGic *InstancePtr, u32 Int_Id){};
s32 XScuGic_CfgInitialize(XScuGic *InstancePtr, XScuGic_Config *ConfigPtr,
                          u32 EffectiveAddr) {
  return XST_SUCCESS;
}

void Xil_ExceptionInit(void) {}

void Xil_ExceptionRegisterHandler(u32 Exception_id,
                                  Xil_ExceptionHandler Handler, void *Data) {}
void Xil_ExceptionEnable(void) {}
