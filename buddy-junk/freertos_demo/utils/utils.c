/*
 * utils.c
 *
 *  Created on: Oct 18, 2013
 *      Author: bmichini
 */


#include "utils.h"


void utils_init(){

	// Green LED
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE , GPIO_PIN_3);
    GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_3, GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD);
	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, ~GPIO_PIN_3 );

	// Red LED
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE , GPIO_PIN_1);
	GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_1, GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD);
	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, ~GPIO_PIN_1 );

	// Blue LED
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE , GPIO_PIN_2);
	GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_2, GPIO_STRENGTH_8MA,GPIO_PIN_TYPE_STD);
	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, ~GPIO_PIN_2 );

	// Debug GPIO pin
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE , GPIO_PIN_2);
	GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_2, ~GPIO_PIN_2 );

	// Left and right buttons
	// Unlock PF0 so we can change it to a GPIO input
	// Once we have enabled (unlocked) the commit register then re-lock it
	// to prevent further changes.  PF0 is muxed with NMI thus a special case.
	//
	HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY_DD;
	HWREG(GPIO_PORTF_BASE + GPIO_O_CR) |= 0x01;
	HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = 0;

	//
	// Set each of the button GPIO pins as an input with a pull-up.
	//
	GPIODirModeSet(GPIO_PORTF_BASE, RIGHT_BUTTON, GPIO_DIR_MODE_IN);
	GPIOPadConfigSet(GPIO_PORTF_BASE, RIGHT_BUTTON,GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
	GPIODirModeSet(GPIO_PORTF_BASE, LEFT_BUTTON, GPIO_DIR_MODE_IN);
	GPIOPadConfigSet(GPIO_PORTF_BASE, LEFT_BUTTON,GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
}

void debugGPIO(tBoolean on){
	if( on ){
		GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_2, GPIO_PIN_2 );
	}else{
		GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_2, ~GPIO_PIN_2 );
	}
}

tBoolean Button_get(uint32_t button){
	if( GPIOPinRead(GPIO_PORTF_BASE, button) & 0x000000FF != 0){
		return false;
	}else{
		return true;
	}
}

void LED_set(uint32_t led, tBoolean on){

	if( on ){
		GPIOPinWrite(GPIO_PORTF_BASE, led, led );
	}else{
		GPIOPinWrite(GPIO_PORTF_BASE, led, ~led );
	}
}

