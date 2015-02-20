/*
 * UART5.c
 *
 *  Created on: Oct 1, 2013
 *      Author: bmichini
 */

#include "comm.h"

uint8_t sbus_rec_buff[25];
uint8_t sbus_ptr = 0;
uint8_t sbusFrame[25];
portTickType lastGoodSbusFrame;
portTickType lastSbusChar;
xSemaphoreHandle sh_Sbus_ready;

void UART5_init(){
    //
    // Initialize the UART and configure it for 115,200, 8-N-1 operation.
    //
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART5);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    GPIOPinConfigure(GPIO_PE4_U5RX);
    GPIOPinConfigure(GPIO_PE5_U5TX);
    ROM_GPIOPinTypeUART(GPIO_PORTE_BASE, GPIO_PIN_4 | GPIO_PIN_5);
    UARTConfigSetExpClk(UART5_BASE, SysCtlClockGet(), 100000,
                                    (UART_CONFIG_PAR_EVEN | UART_CONFIG_STOP_TWO | UART_CONFIG_WLEN_8));

    // Disable the UART5 fifo buffer
    UARTFIFODisable( UART5_BASE);

    // Enable the UART5RX interrupt
    IntPrioritySet( INT_UART5, configMAX_SYSCALL_INTERRUPT_PRIORITY );
    UARTIntEnable( UART5_BASE, UART_INT_RX);
    IntEnable(INT_UART5);

    lastSbusChar = xTaskGetTickCount();
    lastGoodSbusFrame = 0;

    // Create the semaphore to trigger conversion of new sbus frame availability
	vSemaphoreCreateBinary( sh_Sbus_ready );
	xSemaphoreTake( sh_Sbus_ready ,portMAX_DELAY);

}

void UART5IntHandler(void){
	static signed portBASE_TYPE xHigherPriorityTaskWoken;

	UARTIntClear( UART5_BASE, UART_INT_RX);

	static portTickType timeSinceLast;
	timeSinceLast = xTaskGetTickCountFromISR() - lastSbusChar;
	lastSbusChar = xTaskGetTickCountFromISR();

	// Is this the start of a new frame?
	if( timeSinceLast > 10*portTICKS_PER_MS ){
		sbus_ptr = 0;
	}


	sbus_rec_buff[ sbus_ptr ] = (uint8_t) UARTCharGetNonBlocking( UART5_BASE );
	if(sbus_ptr < 24 ){
		sbus_ptr++;
	}else if( sbus_ptr == 24){
		if( sbus_rec_buff[0] == 0x0F &&  sbus_rec_buff[24] == 0x00 ){
			memcpy( sbusFrame, sbus_rec_buff, 25);
			lastGoodSbusFrame = lastSbusChar;
			xSemaphoreGiveFromISR( sh_Sbus_ready, &xHigherPriorityTaskWoken );
		}
	}

}
