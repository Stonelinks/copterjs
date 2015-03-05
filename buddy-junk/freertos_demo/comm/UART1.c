/*
 * UART1.c
 *
 *  Created on: Feb 19, 2015
 *      Author: bmichini
 */

#include "comm.h"


int serialCharsInput_ptr=0;
uint8_t serialCharsInput[5];

void UART1_init(){
    //
    // Initialize the UART and configure it for 115,200, 8-N-1 operation.
    //
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    GPIOPinConfigure(GPIO_PC4_U1RX);
    GPIOPinConfigure(GPIO_PC5_U1TX);
    ROM_GPIOPinTypeUART(GPIO_PORTC_BASE, GPIO_PIN_4 | GPIO_PIN_5);
    UARTConfigSetExpClk(UART1_BASE, SysCtlClockGet(), 115200,(UART_CONFIG_PAR_NONE | UART_CONFIG_STOP_ONE |UART_CONFIG_WLEN_8));

    // Disable the UART1 fifo buffer
    UARTFIFODisable( UART1_BASE);

    // Enable the UART1RX interrupt
    IntPrioritySet( INT_UART1, configMAX_SYSCALL_INTERRUPT_PRIORITY );
    UARTIntEnable( UART1_BASE, UART_INT_RX);
    IntEnable(INT_UART1);

    serialCharsInput[0]=126;
    serialCharsInput[1]=126;
    serialCharsInput[2]=126;
    serialCharsInput[3]=126;
}
uint8_t cnt=0;
void UART1IntHandler(void){
	static signed portBASE_TYPE xHigherPriorityTaskWoken;
	UARTIntClear( UART1_BASE, UART_INT_RX);

	uint8_t ch = (uint8_t) UARTCharGetNonBlocking( UART1_BASE );
	cnt++;
	uint8_t sync = 255;
	if(ch==sync)
	{
		serialCharsInput_ptr=0;
	}

	if(serialCharsInput_ptr<5 )
	{
		serialCharsInput[serialCharsInput_ptr] = ch;
		serialCharsInput_ptr++;
	}


}
