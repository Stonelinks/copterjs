/*
 * utils.h
 *
 *  Created on: Oct 18, 2013
 *      Author: bmichini
 */

#ifndef UTILS_H_
#define UTILS_H_

#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_gpio.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"


#include <stdint.h>


#define LED_GREEN 	GPIO_PIN_3
#define LED_RED 	GPIO_PIN_1
#define LED_BLUE 	GPIO_PIN_2

#define LEFT_BUTTON  	GPIO_PIN_4
#define RIGHT_BUTTON 	GPIO_PIN_0

void utils_init();
void debugGPIO(tBoolean on);

void LED_set(uint32_t led, tBoolean on);
tBoolean Button_get(uint32_t button);

#endif /* UTILS_H_ */
