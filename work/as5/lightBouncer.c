#include <stdint.h>
#include "consoleUtils.h"
#include "soc_AM335x.h"
#include "beaglebone.h"
#include "interrupt.h"
#include "dmtimer.h"
#include "error.h"
#include "hw_types.h"
#include "timer.h"
#include "wdtimer.h"
#include "fakeTyper.h"
#include "serial.h"
#include "leds.h"
#include "joystick.h"


#define PRM_DEV 0x44E00F00 // base
#define PRM_RSTST_OFFSET 0x08 // reg offset

static void displayCommands() {
	ConsoleUtilsPrintf("Commands:\n");
	ConsoleUtilsPrintf("?	: Display this help message\n");
	ConsoleUtilsPrintf("0-9 : Set speed 0 (slow) to 9 (fast)\n");
	ConsoleUtilsPrintf("x   : Stop hitting the watchdog\n");
	ConsoleUtilsPrintf("JOY : Up (faster), Down (slower)\n");
}


/******************************************************************************
 **              SERIAL PORT HANDLING
 ******************************************************************************/
static volatile uint8_t s_rxByte = 0;
static void serialRxIsrCallback(uint8_t rxByte) {
	s_rxByte = rxByte;
}

static void doBackgroundSerialWork(void)
{
	if (s_rxByte != 0) {
		switch(s_rxByte)
		{
			case '?':
				ConsoleUtilsPrintf("\n\n");
				displayCommands();
				break;
			case '0':
				setSpeed(0); 
				break;
			case '1':
				setSpeed(1); 
				break;
			case '2':
				setSpeed(2); 
				break;
			case '3':
				setSpeed(3); 
				break;
			case '4':
				setSpeed(4); 
				break;
			case '5':
				setSpeed(5);
				break;
			case '6':
				setSpeed(6);
				break;
			case '7':
				setSpeed(7);
				break;
			case '8':
				setSpeed(8); 
				break;
			case '9':
				setSpeed(9);
				break;
			case 'x':
				ConsoleUtilsPrintf("\nNo longer hitting the watchdog.\n");
				disableWatchDog();
				break;
			default:
				ConsoleUtilsPrintf("\nInvalid command.\n\n");
				displayCommands();
		}
	}

	s_rxByte = 0;
}



static void displayResets(uint32_t reg) 
{
	if((reg & (1 << 0)) != 0) ConsoleUtilsPrintf("(0x00) Global Cold Reset,\n");
	if((reg & (1 << 1)) != 0) ConsoleUtilsPrintf("(0x01) Global Warm Reset,\n");
	if((reg & (1 << 4)) != 0) ConsoleUtilsPrintf("(0x10) WatchDog1 Timer Reset,\n");
	if((reg & (1 << 5)) != 0) ConsoleUtilsPrintf("(0x20) External Warm Reset,\n");
	if((reg & (1 << 9)) != 0) ConsoleUtilsPrintf("(0x200) IcePick Warm Reset,\n");

	ConsoleUtilsPrintf("\n");
}

int main(void)
{
	// Initialization
	Serial_init(serialRxIsrCallback);
	Timer_init();
	Watchdog_init();
	FakeTyper_init();
	initializeJoystick();
	initializeLeds();	
	
	// Setup callbacks from hardware abstraction modules to application:
	Serial_setRxIsrCallback(serialRxIsrCallback);
	Timer_setTimerIsrCallback(FakeTyper_notifyOnTimeIsr);
	Timer_setTimerIsrCallback(Led_notifyOnTimeIsr);

	//
	ConsoleUtilsPrintf("Light Bouncer: by Yoonhong and Jusung\n");
	ConsoleUtilsPrintf("-------------------------------------\n");
	//
	ConsoleUtilsPrintf("Reset souce:\n");
	uint32_t rst_reg = HWREG(PRM_DEV + PRM_RSTST_OFFSET);
	displayResets(rst_reg);
	HWREG(PRM_DEV + PRM_RSTST_OFFSET) |= (1 << 0 | 1 << 1 | 1 << 4 | 1 << 5 | 1 << 9);

	displayCommands();

	while(1) {
		doBackgroundSerialWork();
		FakeTyper_doBackgroundWork();
		readJoystick();

		// Timer ISR signals intermittent background activity.
		if(Timer_isIsrFlagSet()) {
			Timer_clearIsrFlag();
			Watchdog_hit();
		}
	}
}