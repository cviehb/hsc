#include "xparameter.h"

#ifndef XSCUGIC_H_
#define XSCUGIC_H_

#define XST_SUCCESS 0L
#define XST_FAILURE 1L

typedef void (*Xil_InterruptHandler)(void *data);

typedef struct {
  Xil_InterruptHandler Handler;
  void *CallBackRef;
} XScuGic_VectorTableEntry;

typedef struct {
  u16 DeviceId;        /**< Unique ID  of device */
  u32 CpuBaseAddress;  /**< CPU Interface Register base address */
  u32 DistBaseAddress; /**< Distributor Register base address */
  XScuGic_VectorTableEntry HandlerTable[XSCUGIC_MAX_NUM_INTR_INPUTS]; /**<
                            Vector table of interrupt handlers */
} XScuGic_Config;

typedef struct {
  XScuGic_Config *Config;  /**< Configuration table entry */
  u32 IsReady;             /**< Device is initialized and ready */
  u32 UnhandledInterrupts; /**< Intc Statistics */
} XScuGic;

/*****************************************************************************/
/*                               scu functions                               */
/*****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

XScuGic_Config *XScuGic_LookupConfig(u16 DeviceId);
void XScuGic_InterruptHandler(XScuGic *InstancePtr);
s32 XScuGic_SelfTest(XScuGic *InstancePtr);
s32 XScuGic_Connect(XScuGic *InstancePtr, u32 Int_Id,
                    Xil_InterruptHandler Handler, void *CallBackRef);
void XScuGic_Disconnect(XScuGic *InstancePtr, u32 Int_Id);
void XScuGic_Enable(XScuGic *InstancePtr, u32 Int_Id);
void XScuGic_Disable(XScuGic *InstancePtr, u32 Int_Id);
s32 XScuGic_CfgInitialize(XScuGic *InstancePtr, XScuGic_Config *ConfigPtr,
                          u32 EffectiveAddr);

/*****************************************************************************/
/*          extern functions, defined here to minimize useless files         */
/*****************************************************************************/

/*
 * This typedef is the exception handler function.
 */
typedef void (*Xil_ExceptionHandler)(void *data);
typedef void (*Xil_InterruptHandler)(void *data);

void Xil_ExceptionInit(void);

void Xil_ExceptionRegisterHandler(u32 Exception_id,
                                  Xil_ExceptionHandler Handler, void *Data);
void Xil_ExceptionEnable(void);

#ifdef __cplusplus
}
#endif

#endif
