// leds.h:
// Application module to simulate typing on the screen.

#ifndef _LEDS_H_
#define _LEDS_H_

void initializeLeds(void);
void driveLED(int pin);
void bounceLEDS(void);
void increaseSpeed(void);
void decreaseSpeed(void);
int getSpeed(void);
void Led_notifyOnTimeIsr(void);
void setSpeed(int setspeed);

#endif