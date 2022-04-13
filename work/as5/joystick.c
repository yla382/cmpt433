// GPIO LED demo
#include "soc_AM335x.h"
#include "beaglebone.h"
#include "gpio_v2.h"
#include "hw_types.h"      // For HWREG(...) macro
#include "watchdog.h"
#include "consoleUtils.h"
#include "joystick.h"
#include "leds.h"


/*****************************************************************************
 **                INTERNAL MACRO DEFINITIONS
 *****************************************************************************/
#define JOYSTICK_GPIO_BASE0           (SOC_GPIO_0_REGS)
#define JOYSTICK_GPIO_BASE1           (SOC_GPIO_1_REGS)
#define JSUP_PIN					 (26)
#define JSDN_PIN					 (14)

//#define DELAY_TIME 0x4000000		// Delay with MMU enabled
#define DELAY_TIME 0x40000		// Delay witouth MMU and cache

/*****************************************************************************
 **                INTERNAL FUNCTION DEFINITIONS
 *****************************************************************************/
static int debounce;
static int triggerUP_flag;
static int triggerDN_flag;

void initializeJoystick(void)
{
	/* Enabling functional clocks for GPIO0/1 instance. */
	GPIO0ModuleClkConfig();
	GPIO1ModuleClkConfig();

	/* Enabling the GPIO module. */
	GPIOModuleEnable(JOYSTICK_GPIO_BASE0);
	GPIOModuleEnable(JOYSTICK_GPIO_BASE1);

	/* Resetting the GPIO module. */
	GPIOModuleReset(JOYSTICK_GPIO_BASE0);
	GPIOModuleReset(JOYSTICK_GPIO_BASE1);

	/* Setting the GPIO pin as an input pin. */
	GPIODirModeSet(JOYSTICK_GPIO_BASE0,
			JSUP_PIN,
			GPIO_DATAIN);
	GPIODirModeSet(JOYSTICK_GPIO_BASE1,
			JSDN_PIN,
			GPIO_DATAIN);
	triggerUP_flag = 0;
	triggerDN_flag = 0;
}

void readJoystick(void)
{
	int pin = GPIOPinRead(JOYSTICK_GPIO_BASE0, JSUP_PIN);

	int pin2 = GPIOPinRead(JOYSTICK_GPIO_BASE1, JSDN_PIN);
	if (pin == 0) {
		triggerUP_flag = 1;
	}
	if (pin > 0 && triggerUP_flag == 1) {
		if (debounce >= 10) {
			triggerUP_flag = 0;
			debounce = 0;
			increaseSpeed();
		}
		debounce++;
	}

	if (pin2 == 0) {
		triggerDN_flag = 1;
	}
	if (pin2 > 0 && triggerDN_flag == 1) {
		if (debounce >= 10) {
			triggerDN_flag = 0;
			debounce = 0;
			decreaseSpeed();
		}
		debounce++;
	}
	
}



