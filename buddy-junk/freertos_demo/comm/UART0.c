/*
 * UART0.c
 *
 *  Created on: Sep 20, 2013
 *      Author: bmichini
 */

#include "comm.h"

//*****************************************************************************
// The mutex that protects concurrent access of UART from multiple tasks.
//*****************************************************************************
xSemaphoreHandle g_pUARTSemaphore;

void UART0_init(){
    //
    // Initialize the UART and configure it for 115,200, 8-N-1 operation.
    //
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    ROM_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    UARTStdioInit(0);

    //
    // Create a mutex to guard the UART.
    //
    g_pUARTSemaphore = xSemaphoreCreateMutex();
}
