
#include <stdio.h>
#include <unistd.h>

#include "xgpio.h"
#include "xgpiops.h"
#include "xparameters.h"
#include "xplatform_info.h"
#include "xstatus.h"
#include <xil_printf.h>

#include "xil_exception.h"
#include "xil_printf.h"
#include "xparameters.h"
#include "xscugic.h"
#include "xscutimer.h"
#include <stdio.h>
#include <unistd.h>

#include "xiic.h"

/*#######################TEMP_MOTION###############*/

/************************** Constant Definitions *****************************/

/*
 * The following constants map to the XPAR parameters created in the
 * xparameters.h file. They are defined here such that a user can easily
 * change all the needed parameters in one place.
 */
#define IIC_BASE_ADDRESS                                                                   \
  XPAR_IIC_0_BASEADDR             /*                                                       \
                                   * The following constant defines the address of the IIC \
                                   * device on the IIC bus.             Note that since    \
                                   * the address is             only 7 bits, this             constant                                                                      \
                                   * is the             address             divided        \
                                   * by 2.*/
#define MAGNETOMETER_ADDRESS 0x1E /* LIS3MDL on Arduino shield */
#define MINIZED_MOTION_SENSOR_ADDRESS_SA0_LO                                   \
  0x1E /* 0011110b for LIS2DS12 on MiniZed when SA0 is pulled low*/
#define MINIZED_MOTION_SENSOR_ADDRESS_SA0_HI                                   \
  0x1D /* 0011101b for LIS2DS12 on MiniZed when SA0 is pulled high*/

#define LIS2DS12_ACC_WHO_AM_I 0x43
/************** Device Register  *******************/
#define LIS2DS12_ACC_SENSORHUB_OUT1 0X06
#define LIS2DS12_ACC_SENSORHUB_OUT2 0X07
#define LIS2DS12_ACC_SENSORHUB_OUT3 0X08
#define LIS2DS12_ACC_SENSORHUB_OUT4 0X09
#define LIS2DS12_ACC_SENSORHUB_OUT5 0X0A
#define LIS2DS12_ACC_SENSORHUB_OUT6 0X0B
#define LIS2DS12_ACC_MODULE_8BIT 0X0C
#define LIS2DS12_ACC_WHO_AM_I_REG 0X0F
#define LIS2DS12_ACC_CTRL1 0X20
#define LIS2DS12_ACC_CTRL2 0X21
#define LIS2DS12_ACC_CTRL3 0X22
#define LIS2DS12_ACC_CTRL4 0X23
#define LIS2DS12_ACC_CTRL5 0X24
#define LIS2DS12_ACC_FIFO_CTRL 0X25
#define LIS2DS12_ACC_OUT_T 0X26
#define LIS2DS12_ACC_STATUS 0X27
#define LIS2DS12_ACC_OUT_X_L 0X28
#define LIS2DS12_ACC_OUT_X_H 0X29
#define LIS2DS12_ACC_OUT_Y_L 0X2A
#define LIS2DS12_ACC_OUT_Y_H 0X2B
#define LIS2DS12_ACC_OUT_Z_L 0X2C
#define LIS2DS12_ACC_OUT_Z_H 0X2D
#define LIS2DS12_ACC_FIFO_THS 0X2E
#define LIS2DS12_ACC_FIFO_SRC 0X2F
#define LIS2DS12_ACC_FIFO_SAMPLES 0X30
#define LIS2DS12_ACC_TAP_6D_THS 0X31
#define LIS2DS12_ACC_INT_DUR 0X32
#define LIS2DS12_ACC_WAKE_UP_THS 0X33
#define LIS2DS12_ACC_WAKE_UP_DUR 0X34
#define LIS2DS12_ACC_FREE_FALL 0X35
#define LIS2DS12_ACC_STATUS_DUP 0X36
#define LIS2DS12_ACC_WAKE_UP_SRC 0X37
#define LIS2DS12_ACC_TAP_SRC 0X38
#define LIS2DS12_ACC_6D_SRC 0X39
#define LIS2DS12_ACC_STEP_C_MINTHS 0X3A
#define LIS2DS12_ACC_STEP_C_L 0X3B
#define LIS2DS12_ACC_STEP_C_H 0X3C
#define LIS2DS12_ACC_FUNC_CK_GATE 0X3D
#define LIS2DS12_ACC_FUNC_SRC 0X3E
#define LIS2DS12_ACC_FUNC_CTRL 0X3F

/*#######################TIMER#####################*/
#ifndef TESTAPP_GEN
#define TIMER_DEVICE_ID XPAR_XSCUTIMER_0_DEVICE_ID
#define INTC_DEVICE_ID XPAR_SCUGIC_SINGLE_DEVICE_ID
#define TIMER_IRPT_INTR XPAR_SCUTIMER_INTR
#endif

#define TIMER_LOAD_VALUE 18499999

/*#######################LED#######################*/

#define GPIO_DEVICE_ID XPAR_XGPIOPS_0_DEVICE_ID
#define GPIO_AXI0_DEVICE_ID XPAR_GPIO_0_DEVICE_ID

#define printf xil_printf /* Smalller foot-print printf */

#define GPIO_PS_BUTTON_OFFSET 0 // MIO#0
#define GPIO_PS_LED_R_OFFSET 52 // MIO#52 (906+52=958)
#define GPIO_PS_LED_G_OFFSET 53 // MIO#53 (906+53=959)

#define LED_COLOR_OFF 0
#define LED_COLOR_GREEN 1
#define LED_COLOR_RED 2
#define LED_COLOR_AMBER 3

static u32 PsButtonPin = 0;    /* MIO_0 = PS Pushbutton */
static u32 PSRedLedPin = 52;   /* MIO_52 = Red LED */
static u32 PSGreenLedPin = 53; /* MIO53 = Green LED */

/*
 * The following are declared globally so they are zeroed and can be
 * easily accessible from a debugger.
 */
XGpioPs PS_Gpio; /* The driver instance for PS GPIO Device. */
XGpio PL_Gpio0;  /* The driver instance for PL GPIO Device. */
XGpio PL_Gpio1;  /* The driver instance for PL GPIO Device. */

/*#######################TEMP_MOTION###############*/

int ByteCount;
u8 send_byte;
u8 write_data[256];
u8 read_data[256];
u8 i2c_device_addr = MINIZED_MOTION_SENSOR_ADDRESS_SA0_HI; // by default

/*#######################TIMER#####################*/

int ScuTimerIntrExample(XScuGic *IntcInstancePtr, XScuTimer *TimerInstancePtr,
                        u16 TimerDeviceId, u16 TimerIntrId);

static void TimerIntrHandler(void *CallBackRef);

static int TimerSetupIntrSystem(XScuGic *IntcInstancePtr,
                                XScuTimer *TimerInstancePtr, u16 TimerIntrId);

static void TimerDisableIntrSystem(XScuGic *IntcInstancePtr, u16 TimerIntrId);

XScuTimer TimerInstance;
XScuGic IntcInstance;

volatile int TimerExpired;

static int TimerSetupIntrSystem(XScuGic *IntcInstancePtr,
                                XScuTimer *TimerInstancePtr, u16 TimerIntrId) {
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
  Status = XScuGic_Connect(IntcInstancePtr, TimerIntrId,
                           (Xil_ExceptionHandler)TimerIntrHandler,
                           (void *)TimerInstancePtr);
  if (Status != XST_SUCCESS) {
    return Status;
  }

  /*
   * Enable the interrupt for the device.
   */
  XScuGic_Enable(IntcInstancePtr, TimerIntrId);

  /*
   * Enable the timer interrupts for timer mode.
   */
  XScuTimer_EnableInterrupt(TimerInstancePtr);

#ifndef TESTAPP_GEN
  /*
   * Enable interrupts in the Processor.
   */
  Xil_ExceptionEnable();
#endif

  return XST_SUCCESS;
}

static void TimerIntrHandler(void *CallBackRef) {
  XScuTimer *TimerInstancePtr = (XScuTimer *)CallBackRef;

  /*
   * Check if the timer counter has expired, checking is not necessary
   * since that's the reason this function is executed, this just shows
   * how the callback reference can be used as a pointer to the instance
   * of the timer counter that expired, increment a shared variable so
   * the main thread of execution can see the timer expired.
   */
  if (XScuTimer_IsExpired(TimerInstancePtr)) {
    XScuTimer_ClearInterruptStatus(TimerInstancePtr);
    // poor mans signal mainloop timer has expired
    TimerExpired++;
    // if (TimerExpired == 3) {
    //   XScuTimer_DisableAutoReload(TimerInstancePtr);
    // }
  }
}

static void TimerDisableIntrSystem(XScuGic *IntcInstancePtr, u16 TimerIntrId) {
  XScuGic_Disconnect(IntcInstancePtr, TimerIntrId);
}

/*################################LED###########################*/

void set_PS_LED_color(unsigned char led_color) {
  switch (led_color) {
  case LED_COLOR_OFF:
    XGpioPs_WritePin(&PS_Gpio, GPIO_PS_LED_R_OFFSET, 0x0); // Red LED off
    XGpioPs_WritePin(&PS_Gpio, GPIO_PS_LED_G_OFFSET, 0x0); // Green LED off
    break;
  case LED_COLOR_GREEN:
    XGpioPs_WritePin(&PS_Gpio, GPIO_PS_LED_R_OFFSET, 0x0); // Red LED off
    XGpioPs_WritePin(&PS_Gpio, GPIO_PS_LED_G_OFFSET, 0x1); // Green LED on
    break;
  case LED_COLOR_RED:
    XGpioPs_WritePin(&PS_Gpio, GPIO_PS_LED_R_OFFSET, 0x1); // Red LED on
    XGpioPs_WritePin(&PS_Gpio, GPIO_PS_LED_G_OFFSET, 0x0); // Green LED off
    break;
  case LED_COLOR_AMBER:
    XGpioPs_WritePin(&PS_Gpio, GPIO_PS_LED_R_OFFSET, 0x1); // Red LED on
    XGpioPs_WritePin(&PS_Gpio, GPIO_PS_LED_G_OFFSET, 0x1); // Green LED on
    break;
  default: /* Error */
    // Do nothing
    break;
  } // switch(led_color)
} // set_PS_LED_color()

void set_PL_LED_color(unsigned char led_color) {
  switch (led_color) {
  case LED_COLOR_OFF:
    XGpio_DiscreteWrite(&PL_Gpio0, 1, 0x0); // Red LED off
    XGpio_DiscreteWrite(&PL_Gpio0, 2, 0x0); // Green LED off
    break;
  case LED_COLOR_GREEN:
    XGpio_DiscreteWrite(&PL_Gpio0, 1, 0x1); // Red LED on
    XGpio_DiscreteWrite(&PL_Gpio0, 2, 0x0); // Green LED off
    break;
  case LED_COLOR_RED:
    XGpio_DiscreteWrite(&PL_Gpio0, 1, 0x0); // Red LED off
    XGpio_DiscreteWrite(&PL_Gpio0, 2, 0x1); // Green LED on
    break;
  case LED_COLOR_AMBER:
    XGpio_DiscreteWrite(&PL_Gpio0, 1, 0x1); // Red LED on
    XGpio_DiscreteWrite(&PL_Gpio0, 2, 0x1); // Green LED on
    break;
  default: /* Error */
    // Do nothing
    break;
  } // switch(led_color)
} // set_PL_LED_color()

unsigned char get_PL_switch_value(void) {
  unsigned char PlSwitchStatus;
  PlSwitchStatus = XGpio_DiscreteRead(&PL_Gpio1, 1);
  return (PlSwitchStatus);
} // get_PL_switch_value()

int gpio_init() {
  XGpioPs_Config *ConfigPtr;
  int Status;

  /* Initialize the PS GPIO driver. */
  ConfigPtr = XGpioPs_LookupConfig(GPIO_DEVICE_ID);
  Status = XGpioPs_CfgInitialize(&PS_Gpio, ConfigPtr, ConfigPtr->BaseAddr);
  if (Status != XST_SUCCESS) {
    return XST_FAILURE;
  }

  /* Set the direction for the PS button pin to be input. */
  XGpioPs_SetDirectionPin(&PS_Gpio, PsButtonPin, 0x0);

  /* Set the direction for the LED pins to be outputs */
  XGpioPs_SetDirectionPin(&PS_Gpio, PSRedLedPin, 1);
  XGpioPs_SetDirectionPin(&PS_Gpio, PSGreenLedPin, 1);
  /* Set the output Enable for the LED pins */
  XGpioPs_SetOutputEnablePin(&PS_Gpio, PSRedLedPin, 1);
  XGpioPs_SetOutputEnablePin(&PS_Gpio, PSGreenLedPin, 1);

  /* Initialize the PL AXI GPIO0 driver */
  Status = XGpio_Initialize(&PL_Gpio0, GPIO_AXI0_DEVICE_ID);
  if (Status != XST_SUCCESS) {
    return XST_FAILURE;
  }

  /* Set the direction for all LED signals as outputs */
  XGpio_SetDataDirection(&PL_Gpio0, 1, 0x00); // All outputs

  /* BEGIN_COMMENTED  OUT NOT FOUND
  * Initialize the PL AXI GPIO1 driver *
  Status = XGpio_Initialize(&PL_Gpio1, GPIO_AXI1_DEVICE_ID);
  if (Status != XST_SUCCESS) {
          return XST_FAILURE;
  }

  * Set the direction of Switch bit 0 as input, others as outputs *
  XGpio_SetDataDirection(&PL_Gpio1, 1, 0x01);
  END_COMMENTED  OUT NOT FOUND*/
  return 0;
} // gpio_init()

void SwitchLedByValue(unsigned int led_count) {

  led_count %= 0x0F;
  switch (led_count) {
  case 0x00:
    set_PL_LED_color(LED_COLOR_OFF);
    set_PS_LED_color(LED_COLOR_OFF);
    break;
  case 0x01:
    set_PL_LED_color(LED_COLOR_OFF);
    set_PS_LED_color(LED_COLOR_RED);
    break;
  case 0x02:
    set_PL_LED_color(LED_COLOR_OFF);
    set_PS_LED_color(LED_COLOR_GREEN);
    break;
  case 0x03:
    set_PL_LED_color(LED_COLOR_OFF);
    set_PS_LED_color(LED_COLOR_AMBER);
    break;
  case 0x04:
    set_PL_LED_color(LED_COLOR_RED);
    set_PS_LED_color(LED_COLOR_OFF);
    break;
  case 0x05:
    set_PL_LED_color(LED_COLOR_RED);
    set_PS_LED_color(LED_COLOR_RED);
    break;
  case 0x06:
    set_PL_LED_color(LED_COLOR_RED);
    set_PS_LED_color(LED_COLOR_GREEN);
    break;
  case 0x07:
    set_PL_LED_color(LED_COLOR_RED);
    set_PS_LED_color(LED_COLOR_AMBER);
    break;
  case 0x08:
    set_PL_LED_color(LED_COLOR_GREEN);
    set_PS_LED_color(LED_COLOR_OFF);
    break;
  case 0x09:
    set_PL_LED_color(LED_COLOR_GREEN);
    set_PS_LED_color(LED_COLOR_RED);
    break;
  case 0x0A:
    set_PL_LED_color(LED_COLOR_GREEN);
    set_PS_LED_color(LED_COLOR_GREEN);
    break;
  case 0x0B:
    set_PL_LED_color(LED_COLOR_GREEN);
    set_PS_LED_color(LED_COLOR_AMBER);
    break;
  case 0x0C:
    set_PL_LED_color(LED_COLOR_AMBER);
    set_PS_LED_color(LED_COLOR_OFF);
    break;
  case 0x0D:
    set_PL_LED_color(LED_COLOR_AMBER);
    set_PS_LED_color(LED_COLOR_RED);
    break;
  case 0x0E:
    set_PL_LED_color(LED_COLOR_AMBER);
    set_PS_LED_color(LED_COLOR_GREEN);
    break;
  case 0x0F:
    set_PL_LED_color(LED_COLOR_AMBER);
    set_PS_LED_color(LED_COLOR_AMBER);
    break;
  default: /* Error */
           // Do nothing
    break;
  } // switch(led_color)
}

void CleanUp(XScuGic *IntcInstancePtr, XScuTimer *TimerInstancePtr,
             u16 TimerDeviceId, u16 TimerIntrId) {
  XScuTimer_Stop(TimerInstancePtr);
  TimerDisableIntrSystem(IntcInstancePtr, TimerIntrId);
}

/*##############################TEMP_MOTION######################*/

u8 LIS2DS12_WriteReg(u8 Reg, u8 *Bufp, u16 len) {
  write_data[0] = Reg;
  for (ByteCount = 1; ByteCount <= len; ByteCount++) {
    write_data[ByteCount] = Bufp[ByteCount - 1];
  }
  ByteCount = XIic_Send(IIC_BASE_ADDRESS, i2c_device_addr, &write_data[0],
                        (len + 1), XIIC_STOP);
  return (ByteCount);
}

u8 LIS2DS12_ReadReg(u8 Reg, u8 *Bufp, u16 len) {
  write_data[0] = Reg;
  ByteCount = XIic_Send(IIC_BASE_ADDRESS, i2c_device_addr, (u8 *)&write_data, 1,
                        XIIC_REPEATED_START);
  ByteCount =
      XIic_Recv(IIC_BASE_ADDRESS, i2c_device_addr, (u8 *)Bufp, len, XIIC_STOP);
  return (ByteCount);
}

void sensor_init(void) {
  u8 who_am_i;
  i2c_device_addr = MINIZED_MOTION_SENSOR_ADDRESS_SA0_HI; // default
  LIS2DS12_ReadReg(LIS2DS12_ACC_WHO_AM_I_REG, &who_am_i, 1);
  printf("With I2C device address 0x%02x received WhoAmI = 0x%02x\r\n",
         i2c_device_addr, who_am_i);
  if (who_am_i != LIS2DS12_ACC_WHO_AM_I) {
    // maybe the address bit was changed, try the other one:
    i2c_device_addr = MINIZED_MOTION_SENSOR_ADDRESS_SA0_LO;
    LIS2DS12_ReadReg(LIS2DS12_ACC_WHO_AM_I_REG, &who_am_i, 1);
    printf("With I2C device address 0x%02x received WhoAmI = 0x%02x\r\n",
           i2c_device_addr, who_am_i);
  }
  send_byte = 0x00; // No auto increment
  LIS2DS12_WriteReg(LIS2DS12_ACC_CTRL2, &send_byte, 1);

  // Write 60h in CTRL1	// Turn on the accelerometer.  14-bit mode, ODR = 400
  // Hz, FS = 2g
  send_byte = 0x60;
  LIS2DS12_WriteReg(LIS2DS12_ACC_CTRL1, &send_byte, 1);
  printf("CTL1 = 0x60 written\r\n");

  // Enable interrupt
  send_byte = 0x01; // Acc data-ready interrupt on INT1
  LIS2DS12_WriteReg(LIS2DS12_ACC_CTRL4, &send_byte, 1);
  printf("CTL4 = 0x01 written\r\n");
  
#if (0)
  write_data[0] = 0x0F; // WhoAmI
  ByteCount = XIic_Send(IIC_BASE_ADDRESS, MAGNETOMETER_ADDRESS,
                        (u8 *)&write_data, 1, XIIC_REPEATED_START);
  ByteCount = XIic_Recv(IIC_BASE_ADDRESS, MAGNETOMETER_ADDRESS,
                        (u8 *)&read_data[0], 1, XIIC_STOP);
  printf("Received 0x%02x\r\n", read_data[0]);
  printf("\r\n"); // Empty line
  // for (int n=0;n<1400;n++) //118 ms is too little
  for (int n = 0; n < 1500; n++) // 128 ms
  {
    printf(".");
  };
  printf("\r\n");
#endif
} // sensor_init()

void read_temperature(void) {
  int temp;
  u8 read_value;

  LIS2DS12_ReadReg(LIS2DS12_ACC_OUT_T, &read_value, 1);
  // Temperature is from -40 to +85 deg C.  So 125 range.  0 is 25 deg C.  +1
  // deg C/LSB.  So if value < 128 temp = 25 + value else temp = 25 -
  // (256-value)
  if (read_value < 128) {
    temp = 25 + read_value;
  } else {
    temp = 25 - (256 - read_value);
  }

  printf("OUT_T register = 0x%02x -> Temperature = %i degrees C", read_value,
         temp);
  // printf("OUT_T register = 0x%02x -> Temperature = %i degrees
  // C\r\n",read_value,temp);
} // read_temperature()

int u16_2s_complement_to_int(u16 word_to_convert) {
  u16 result_16bit;
  int result_14bit;
  int sign;

  if (word_to_convert & 0x8000) { // MSB is set, negative number
    // Invert and add 1
    sign = -1;
    result_16bit = (~word_to_convert) + 1;
  } else { // Positive number
    // No change
    sign = 1;
    result_16bit = word_to_convert;
  }
  // We are using it in 14-bit mode
  // All data is left-aligned.  So convert 16-bit value to 14-but value
  result_14bit = sign * (int)(result_16bit >> 2);
  return (result_14bit);
} // u16_2s_complement_to_int()

void read_motion(void) {
  int iacceleration_X;
  int iacceleration_Y;
  int iacceleration_Z;
  u8 read_value_LSB;
  u8 read_value_MSB;
  u16 accel_X;
  u16 accel_Y;
  u16 accel_Z;
  u8 accel_status;
  u8 data_ready;

  data_ready = 0;
  while (!data_ready) { // wait for DRDY
    LIS2DS12_ReadReg(LIS2DS12_ACC_STATUS, &accel_status, 1);
    data_ready = accel_status & 0x01; // bit 0 = DRDY
  }                                   // wait for DRDY

  // Read X:
  LIS2DS12_ReadReg(LIS2DS12_ACC_OUT_X_L, &read_value_LSB, 1);
  LIS2DS12_ReadReg(LIS2DS12_ACC_OUT_X_H, &read_value_MSB, 1);
  accel_X = (read_value_MSB << 8) + read_value_LSB;
  iacceleration_X = u16_2s_complement_to_int(accel_X);
  // Read Y:
  LIS2DS12_ReadReg(LIS2DS12_ACC_OUT_Y_L, &read_value_LSB, 1);
  LIS2DS12_ReadReg(LIS2DS12_ACC_OUT_Y_H, &read_value_MSB, 1);
  accel_Y = (read_value_MSB << 8) + read_value_LSB;
  iacceleration_Y = u16_2s_complement_to_int(accel_Y);
  // Read Z:
  LIS2DS12_ReadReg(LIS2DS12_ACC_OUT_Z_L, &read_value_LSB, 1);
  LIS2DS12_ReadReg(LIS2DS12_ACC_OUT_Z_H, &read_value_MSB, 1);
  accel_Z = (read_value_MSB << 8) + read_value_LSB;
  iacceleration_Z = u16_2s_complement_to_int(accel_Z);

  xil_printf("  Acceleration = X: %d, Y: %d, Z: %d\r\n", iacceleration_X,
             iacceleration_Y, iacceleration_Z);
} // read_motion()

int init_timer(XScuGic *IntcInstancePtr, XScuTimer *TimerInstancePtr,
               u16 TimerDeviceId, u16 TimerIntrId) {
  int Status;
  XScuTimer_Config *ConfigPtr;

  ConfigPtr = XScuTimer_LookupConfig(TimerDeviceId);

  Status =
      XScuTimer_CfgInitialize(TimerInstancePtr, ConfigPtr, ConfigPtr->BaseAddr);
  XScuTimer_SetPrescaler(TimerInstancePtr, 8);
  if (Status != XST_SUCCESS) {
    return XST_FAILURE;
  }

  Status = XScuTimer_SelfTest(TimerInstancePtr);
  if (Status != XST_SUCCESS) {
    return XST_FAILURE;
  }

  Status = TimerSetupIntrSystem(IntcInstancePtr, TimerInstancePtr, TimerIntrId);
  if (Status != XST_SUCCESS) {
    return XST_FAILURE;
  }

  XScuTimer_EnableAutoReload(TimerInstancePtr);
  XScuTimer_LoadTimer(TimerInstancePtr, TIMER_LOAD_VALUE);
  XScuTimer_Start(TimerInstancePtr);

  return Status;
} // init_timer

int main_init(XScuGic *IntcInstancePtr, XScuTimer *TimerInstancePtr,
              u16 TimerDeviceId, u16 TimerIntrId) {
  int stat;
  // leds
  gpio_init();
  sensor_init();
  // timer has to be initialized when other hw is up and running
  stat =
      init_timer(IntcInstancePtr, TimerInstancePtr, TimerDeviceId, TimerIntrId);
  return stat;
}

int main() {

  int Status;
  int LastTimerExpired = 0;

  // initialize hw: timer, i2c
  Status = main_init(&IntcInstance, &TimerInstance, TIMER_DEVICE_ID,
                     TIMER_IRPT_INTR);

  // check for error on init functions
  if (Status != XST_SUCCESS) {
    xil_printf("SCU Timer Interrupt Example Test Failed\r\n");
    return XST_FAILURE;
  }

  // endless loop doeing timer
  while (1) {
    // wait on timer till expired
    while (TimerExpired == LastTimerExpired) {
    };

    // switch to next timer
    LastTimerExpired = TimerExpired;

    // change led every timer irq is occured
    SwitchLedByValue((unsigned int)TimerExpired);

    // read i2c values every 4th time of timer irq --> 2s
    if (TimerExpired % 4 == 0) {
      read_temperature();
      read_motion();
    }
  }

  // should normally never be reached
  xil_printf("Some Bad Behaviour occured due to mainloop has ended!\r\n");
  return XST_SUCCESS;
}
