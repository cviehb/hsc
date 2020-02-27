/***************************** Include Files ********************************/
#include <stdio.h>
#include <unistd.h>

#include "xparameters.h"
#include "xgpiops.h"
#include "xgpio.h"
#include "xstatus.h"
#include "xplatform_info.h"
#include <xil_printf.h>

/************************** Constant Definitions ****************************/

/*
 * The following constants map to the XPAR parameters created in the
 * xparameters.h file. They are defined here such that a user can easily
 * change all the needed parameters in one place.
 */
#define GPIO_DEVICE_ID  	XPAR_XGPIOPS_0_DEVICE_ID
#define GPIO_AXI0_DEVICE_ID  XPAR_GPIO_0_DEVICE_ID
#define GPIO_AXI1_DEVICE_ID  XPAR_GPIO_1_DEVICE_ID

#define printf			xil_printf	/* Smalller foot-print printf */

#define GPIO_PS_BUTTON_OFFSET				0 //MIO#0
#define GPIO_PS_LED_R_OFFSET				52 //MIO#52 (906+52=958)
#define GPIO_PS_LED_G_OFFSET				53 //MIO#53 (906+53=959)

#define LED_COLOR_OFF	0
#define LED_COLOR_GREEN	1
#define LED_COLOR_RED	2
#define LED_COLOR_AMBER	3
/************************** Variable Definitions **************************/
static u32 PsButtonPin = 0; /* MIO_0 = PS Pushbutton */
static u32 PSRedLedPin = 52; /* MIO_52 = Red LED */
static u32 PSGreenLedPin = 53; /* MIO53 = Green LED */

/*
 * The following are declared globally so they are zeroed and can be
 * easily accessible from a debugger.
 */
XGpioPs PS_Gpio;	/* The driver instance for PS GPIO Device. */
XGpio PL_Gpio0;	/* The driver instance for PL GPIO Device. */
XGpio PL_Gpio1;	/* The driver instance for PL GPIO Device. */

void set_PS_LED_color(unsigned char led_color)
{
	switch(led_color)
	{
		case LED_COLOR_OFF :
			XGpioPs_WritePin(&PS_Gpio, GPIO_PS_LED_R_OFFSET, 0x0); //Red LED off
			XGpioPs_WritePin(&PS_Gpio, GPIO_PS_LED_G_OFFSET, 0x0); //Green LED off
			break;
		case LED_COLOR_GREEN :
			XGpioPs_WritePin(&PS_Gpio, GPIO_PS_LED_R_OFFSET, 0x0); //Red LED off
			XGpioPs_WritePin(&PS_Gpio, GPIO_PS_LED_G_OFFSET, 0x1); //Green LED on
			break;
		case LED_COLOR_RED :
			XGpioPs_WritePin(&PS_Gpio, GPIO_PS_LED_R_OFFSET, 0x1); //Red LED on
			XGpioPs_WritePin(&PS_Gpio, GPIO_PS_LED_G_OFFSET, 0x0); //Green LED off
			break;
		case LED_COLOR_AMBER :
			XGpioPs_WritePin(&PS_Gpio, GPIO_PS_LED_R_OFFSET, 0x1); //Red LED on
			XGpioPs_WritePin(&PS_Gpio, GPIO_PS_LED_G_OFFSET, 0x1); //Green LED on
			break;
		default : /* Error */
			//Do nothing
			break;
	} //switch(led_color)
} //set_PS_LED_color()

void set_PL_LED_color(unsigned char led_color)
{
	switch(led_color)
	{
	case LED_COLOR_OFF :
		XGpio_DiscreteWrite(&PL_Gpio0, 1, 0x0); //Red LED off
		XGpio_DiscreteWrite(&PL_Gpio0, 2, 0x0); //Green LED off
		break;
	case LED_COLOR_GREEN :
		XGpio_DiscreteWrite(&PL_Gpio0, 1, 0x1); //Red LED on
		XGpio_DiscreteWrite(&PL_Gpio0, 2, 0x0); //Green LED off
		break;
	case LED_COLOR_RED :
		XGpio_DiscreteWrite(&PL_Gpio0, 1, 0x0); //Red LED off
		XGpio_DiscreteWrite(&PL_Gpio0, 2, 0x1); //Green LED on
		break;
	case LED_COLOR_AMBER :
		XGpio_DiscreteWrite(&PL_Gpio0, 1, 0x1); //Red LED on
		XGpio_DiscreteWrite(&PL_Gpio0, 2, 0x1); //Green LED on
		break;
		default : /* Error */
			//Do nothing
			break;
	} //switch(led_color)
} //set_PL_LED_color()

unsigned char get_PS_button_value(void)
{
	unsigned char PsButtonStatus;
	PsButtonStatus = XGpioPs_ReadPin(&PS_Gpio, PsButtonPin);
	return(PsButtonStatus);
} //get_PS_button_value()

unsigned char get_PL_switch_value(void)
{
	unsigned char PlSwitchStatus;
	PlSwitchStatus = XGpio_DiscreteRead(&PL_Gpio1, 1);
	return(PlSwitchStatus);
} //get_PL_switch_value()

int gpio_init()
{
	XGpioPs_Config *ConfigPtr;
	int Status;

	/* Initialize the PS GPIO driver. */
	ConfigPtr = XGpioPs_LookupConfig(GPIO_DEVICE_ID);
	Status = XGpioPs_CfgInitialize(&PS_Gpio, ConfigPtr,
					ConfigPtr->BaseAddr);
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
	XGpio_SetDataDirection(&PL_Gpio0, 1, 0x00); //All outputs

	/* Initialize the PL AXI GPIO1 driver */
	Status = XGpio_Initialize(&PL_Gpio1, GPIO_AXI1_DEVICE_ID);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/* Set the direction of Switch bit 0 as input, others as outputs */
	XGpio_SetDataDirection(&PL_Gpio1, 1, 0x01);
	return 0;
} //gpio_init()

/*****************************************************************************/
// Main function
/******************************************************************************/
int main(void)
{
	u32 PsButtonStatus;
	u32 PsButtonHistory;
	u32 PlSwitchStatus;

	printf("################################################################################\r\n");
    printf("Testing the MiniZed GPIO - Standalone project                                   \r\n");
    printf("--------------------------------------------------------------------------------\r\n");
    printf("The PS Button will reset the LED counter\r\n");
    printf("The PL Switch (one of a double switch) is the one furthest from the board edge  \r\n");
    printf("################################################################################\r\n");

    //Initialized GPIO
    gpio_init();

    /* Read the state of the PS button. */
	PsButtonStatus = XGpioPs_ReadPin(&PS_Gpio, PsButtonPin);
	if (PsButtonStatus == 1)
	{
		printf("PS Button pushed.\r\n");
	}
	else
	{
		printf("PS Button Not pushed.\r\n");
	}

	PlSwitchStatus = get_PL_switch_value();
	if (PlSwitchStatus == 1)
	{
		printf("Status of PL Switch is  0x%x (towards PS button)\r\n", PlSwitchStatus);
	}
	else
	{
		printf("Status of PL Switch is  0x%x (towards LEDs)\r\n", PlSwitchStatus);
	}
	PsButtonHistory = 100; //start

	while (1)
	{
		unsigned int free_count = 0;
		unsigned int led_count = 0;

		while (1) //forever
		{
			PsButtonStatus = get_PS_button_value();
			if (PsButtonStatus == 1)
			{
				free_count = 0;
			}
			if (PsButtonHistory != PsButtonStatus)  //There was a button change
			{
				if (PsButtonStatus == 1)
				{
					//reset the count when the button is pushed:
					printf("PS Button pushed.  Resetting LED counter.   \r");
					free_count = 0;
				}
				else
				{
					printf("Push the PS Button to reset the LED counter.\r");
				}
			}
			PsButtonHistory = PsButtonStatus;
			led_count = (free_count & 0x0F0) >> 4;
			//Show a binary counter on the two LEDs:
			switch(led_count)
			{
				case 0x00 :
					set_PL_LED_color(LED_COLOR_OFF);
					set_PS_LED_color(LED_COLOR_OFF);
					break;
				case 0x01 :
					set_PL_LED_color(LED_COLOR_OFF);
					set_PS_LED_color(LED_COLOR_RED);
					break;
				case 0x02 :
					set_PL_LED_color(LED_COLOR_OFF);
					set_PS_LED_color(LED_COLOR_GREEN);
					break;
				case 0x03 :
					set_PL_LED_color(LED_COLOR_OFF);
					set_PS_LED_color(LED_COLOR_AMBER);
					break;
				case 0x04 :
					set_PL_LED_color(LED_COLOR_RED);
					set_PS_LED_color(LED_COLOR_OFF);
					break;
				case 0x05 :
					set_PL_LED_color(LED_COLOR_RED);
					set_PS_LED_color(LED_COLOR_RED);
					break;
				case 0x06 :
					set_PL_LED_color(LED_COLOR_RED);
					set_PS_LED_color(LED_COLOR_GREEN);
					break;
				case 0x07 :
					set_PL_LED_color(LED_COLOR_RED);
					set_PS_LED_color(LED_COLOR_AMBER);
					break;
				case 0x08 :
					set_PL_LED_color(LED_COLOR_GREEN);
					set_PS_LED_color(LED_COLOR_OFF);
					break;
				case 0x09 :
					set_PL_LED_color(LED_COLOR_GREEN);
					set_PS_LED_color(LED_COLOR_RED);
					break;
				case 0x0A :
					set_PL_LED_color(LED_COLOR_GREEN);
					set_PS_LED_color(LED_COLOR_GREEN);
					break;
				case 0x0B :
					set_PL_LED_color(LED_COLOR_GREEN);
					set_PS_LED_color(LED_COLOR_AMBER);
					break;
				case 0x0C :
					set_PL_LED_color(LED_COLOR_AMBER);
					set_PS_LED_color(LED_COLOR_OFF);
					break;
				case 0x0D :
					set_PL_LED_color(LED_COLOR_AMBER);
					set_PS_LED_color(LED_COLOR_RED);
					break;
				case 0x0E :
					set_PL_LED_color(LED_COLOR_AMBER);
					set_PS_LED_color(LED_COLOR_GREEN);
					break;
				case 0x0F :
					set_PL_LED_color(LED_COLOR_AMBER);
					set_PS_LED_color(LED_COLOR_AMBER);
					break;
				default : /* Error */
					//Do nothing
					break;
			} //switch(led_color)
			usleep(20000); //20 ms
			free_count++;
			if ((free_count & 0x1F) == 0x1F) //Every 31*20ms = 0.62 seconds
			{
				//read_temperature();
				//read_motion();
			}
		}
	}

	printf("Successfully ran GPIO Polled Mode Example Test\r\n");
	return XST_SUCCESS;
} //main()
