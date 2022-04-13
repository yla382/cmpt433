// GPIO LED demo
#include "soc_AM335x.h"
#include "beaglebone.h"
#include "gpio_v2.h"
#include "hw_types.h"      // For HWREG(...) macro
#include "watchdog.h"
#include "leds.h"
#include <stdbool.h>


/*****************************************************************************
 **                INTERNAL MACRO DEFINITIONS
 *****************************************************************************/
#define LED_GPIO_BASE           (SOC_GPIO_1_REGS)
#define LED0_PIN (21)
#define LED1_PIN (22)
#define LED2_PIN (23)
#define LED3_PIN (24)

#define LED_MASK ((1<<LED0_PIN) | (1<<LED1_PIN) | (1<<LED2_PIN) | (1<<LED3_PIN))
#define LED0_MASK ((1<<LED0_PIN) | (0<<LED1_PIN) | (0<<LED2_PIN) | (1<<LED3_PIN))
#define LED1_MASK ((1<<LED0_PIN) | (1<<LED1_PIN) | (1<<LED2_PIN) | (1<<LED3_PIN))
#define LED2_MASK ((1<<LED0_PIN) | (1<<LED1_PIN) | (1<<LED2_PIN) | (1<<LED3_PIN))
#define LED3_MASK ((1<<LED0_PIN) | (1<<LED1_PIN) | (1<<LED2_PIN) | (1<<LED3_PIN))

//#define DELAY_TIME 0x4000000		// Delay with MMU enabled
#define DELAY_TIME 0x40000		// Delay witouth MMU and cache
static int pin_num = 0;
static int reverse = 0;
static int speed;
static int trigger = 0;
static int period;

/*****************************************************************************
 **                INTERNAL FUNCTION DEFINITIONS
 *****************************************************************************/

void initializeLeds(void)
{
	/* Enabling functional clocks for GPIO1 instance. */
	GPIO1ModuleClkConfig();

	/* Selecting GPIO1[23] pin for use. */
	//GPIO1Pin23PinMuxSetup();

	/* Enabling the GPIO module. */
	GPIOModuleEnable(LED_GPIO_BASE);

	/* Resetting the GPIO module. */
	GPIOModuleReset(LED_GPIO_BASE);

	/* Setting the GPIO pin as an output pin. */
	GPIODirModeSet(LED_GPIO_BASE,
			LED0_PIN,
			GPIO_DIR_OUTPUT);
	GPIODirModeSet(LED_GPIO_BASE,
			LED1_PIN,
			GPIO_DIR_OUTPUT);
	GPIODirModeSet(LED_GPIO_BASE,
			LED2_PIN,
			GPIO_DIR_OUTPUT);
	GPIODirModeSet(LED_GPIO_BASE,
			LED3_PIN,
			GPIO_DIR_OUTPUT);
	speed = 4;
	period = 2 << (9-speed);
}

void Led_notifyOnTimeIsr(void)
{
	bounceLEDS();
}

void setSpeed(int setspeed) {
	speed = setspeed;
	period = 2 << (9-speed);
	ConsoleUtilsPrintf("\nspeed: %d\n", speed);
}

void increaseSpeed() {
	if (speed < 9) {
		speed++;
	}
	period = 2 << (9-speed);
	ConsoleUtilsPrintf("\nspeed: %d\n", speed);
}

void decreaseSpeed() {
	if (speed > 0) {
		speed--;
	}
	period = 2 << (9-speed);
	ConsoleUtilsPrintf("\nspeed: %d\n", speed);
}

int getSpeed() {
	return speed;
}

void driveLED(int pin) {
	/* Driving a logic LOW on the GPIO pin. */
	GPIOPinWrite(LED_GPIO_BASE,
			LED0_PIN,
			GPIO_PIN_LOW);
			/* Driving a logic LOW on the GPIO pin. */
	GPIOPinWrite(LED_GPIO_BASE,
			LED1_PIN,
			GPIO_PIN_LOW);
			/* Driving a logic LOW on the GPIO pin. */
	GPIOPinWrite(LED_GPIO_BASE,
			LED2_PIN,
			GPIO_PIN_LOW);
			/* Driving a logic LOW on the GPIO pin. */
	GPIOPinWrite(LED_GPIO_BASE,
			LED3_PIN,
			GPIO_PIN_LOW);
	/* Driving a logic HIGH on the GPIO pin. */
	GPIOPinWrite(LED_GPIO_BASE,
			pin,
			GPIO_PIN_HIGH);
}

void bounceLEDS(void)
{	
	trigger++;
	if (trigger >= period) {
		if ((pin_num % 4) == 0) {
			driveLED(LED0_PIN);
			reverse = 0;
			pin_num++;
		} else if ((pin_num % 4) == 1) {
			driveLED(LED1_PIN);
			if (reverse == 0) {
				pin_num++;
			} else {
				pin_num--;
			}
		} else if ((pin_num % 4) == 2) {
			driveLED(LED2_PIN);
			if (reverse == 0) {
				pin_num++;
			} else {
				pin_num--;
			}
		} else if ((pin_num % 4) == 3) {
			driveLED(LED3_PIN);
			reverse = 1;
			pin_num--;
		} else {
			reverse = 0;
			pin_num = 0;
		}
		trigger = 0;
	}
}



