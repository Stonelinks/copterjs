/*
 * I2C1.c
 *
 *  Created on: Sep 13, 2013
 *      Author: bmichini
 */
#include "comm.h"

xQueueHandle I2C1_messagequeue;
tI2C_message I2C1_active_message;
uint8_t I2C1_data_ptr;


xTaskHandle th_I2C1queuehandler;
xSemaphoreHandle sh_I2C1_busy;

// Initializes I2C1 as Master
// If fastmode=true speed is 400kbps, otherwise it is 100kbps
void I2C1_init( tBoolean fastmode ){

	// First initialize the I2C1 message queue
	I2C1_messagequeue = xQueueCreate(I2C1_MESSAGE_QUEUE_SIZE, sizeof(tI2C_message) );

	// I2C1 is used with PortA[7:6], GPIO port B needs to be enabled so these pins can be used.
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

	// The I2C1 peripheral must be enabled before use.
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C1);
	GPIOPinTypeI2CSCL(GPIO_PORTA_BASE, GPIO_PIN_6);
	ROM_GPIOPinTypeI2C(GPIO_PORTA_BASE, GPIO_PIN_7);
	ROM_GPIOPinConfigure(GPIO_PA6_I2C1SCL);
	ROM_GPIOPinConfigure(GPIO_PA7_I2C1SDA);

	// Enable the I2C1 interrupt on the processor (NVIC).
	IntPrioritySet( INT_I2C1, configMAX_SYSCALL_INTERRUPT_PRIORITY );
	IntEnable(INT_I2C1);

	// Configure and turn on the I2C1 master interrupt.
	I2CMasterIntEnableEx(I2C1_MASTER_BASE, I2C_MASTER_INT_DATA);

	// Enable and initialize the I2C1 master module.  Use the system clock for
	// the I2C1 module.  The last parameter sets the I2C data transfer rate.
	// If false the data rate is set to 100kbps and if true the data rate will
	// be set to 400kbps.
	I2CMasterInitExpClk(I2C1_MASTER_BASE, SysCtlClockGet(), fastmode);
	HWREG( I2C1_MASTER_BASE + I2C_O_MTPR ) = 0x05; // This sets the speed to 400KHz *for real*

	// Create the semaphore to manage I2C1 availability
	vSemaphoreCreateBinary( sh_I2C1_busy );
	xSemaphoreTake( sh_I2C1_busy ,portMAX_DELAY);

	// Start the I2C1 queue handler task
	xTaskCreate( I2C1_QueueHandler, "I2C1 Queue Handler", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+I2C1_HANDLER_TASK, &th_I2C1queuehandler );

}

uint8_t I2C1_busy_count = 0;
tBoolean waitingForSem, waitingForBusy, waitingForQueue;
tBoolean semSuccess;
void I2C1_QueueHandler(void *pvParameters)
{

	while(1){

		semSuccess = true;
		while(semSuccess){

			// Block indefinitely on the message queue
			xQueueReceive( I2C1_messagequeue, &I2C1_active_message, portMAX_DELAY );

			waitingForBusy = true;
			if( I2CMasterBusBusy(I2C1_MASTER_BASE) ){
				vTaskDelay(5);
			}
			waitingForBusy = false;

			if( I2CMasterBusBusy(I2C1_MASTER_BASE) ){
				semSuccess = false;
			}

			// Initiate the I2C transmission
			if( I2C1_active_message.read ){
				if( I2C1_active_message.read_addr < 255 ){
					// If read operation, set the peripheral read pointer with an I2C write
					I2CMasterSlaveAddrSet(I2C1_MASTER_BASE, I2C1_active_message.slave_addr, false);
					I2CMasterDataPut(I2C1_MASTER_BASE, I2C1_active_message.read_addr);
					I2C1_data_ptr = 255; // So that incrementing yields zero
					// Initiate the write
					I2CMasterControl(I2C1_MASTER_BASE, I2C_MASTER_CMD_BURST_SEND_START);
				}else{
					// Read operation with no I2C write address setup, i.e. just start reading bytes
					I2CMasterSlaveAddrSet(I2C1_MASTER_BASE, I2C1_active_message.slave_addr, true);
					I2C1_data_ptr = 0;
					I2CMasterControl(I2C1_MASTER_BASE, I2C_MASTER_CMD_BURST_RECEIVE_START);
				}
			}else{
				// Write operation, point to first byte of the data
				I2CMasterSlaveAddrSet(I2C1_MASTER_BASE, I2C1_active_message.slave_addr, false);
				I2CMasterDataPut(I2C1_MASTER_BASE,*(I2C1_active_message.data));
				I2C1_data_ptr = 0;
				// Initiate the write
				I2CMasterControl(I2C1_MASTER_BASE, I2C_MASTER_CMD_BURST_SEND_START);
			}



			// The interrupt takes care of the rest
			// Interrupt will give the semaphore after it issues the I2C stop signal
			semSuccess = xSemaphoreTake( sh_I2C1_busy , 10) ;

		} // end inner while(1)

		//
		// If execution breaks to here, there was a timeout
		// Execute bus recovery procedure
		//

		// Disable I2C1 interrupt
		IntDisable(INT_I2C1);

		// Disable I2C1 peripheral,
		ROM_SysCtlPeripheralDisable(SYSCTL_PERIPH_I2C1);
		SysCtlPeripheralReset(SYSCTL_PERIPH_I2C1);
		SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
		ROM_GPIOPinTypeGPIOOutputOD(GPIO_PORTA_BASE, GPIO_PIN_6);

		// Pulse the SCL line several times to clear pending data reads
		static int i,j;
		for( i=0; i<9; i++){
			for(j=0; j<3000; j++){}
			ROM_GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_6, GPIO_PIN_6);
			for(j=0; j<3000; j++){}
			ROM_GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_6, 0);
		}
		vTaskDelay(1);

		// Restart the I2C1 peripheral
		// I2C1 is used with PortA[7:6], GPIO port B needs to be enabled so these pins can be used.
		SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
		ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C1);
		GPIOPinTypeI2CSCL(GPIO_PORTA_BASE, GPIO_PIN_6);
		ROM_GPIOPinTypeI2C(GPIO_PORTA_BASE, GPIO_PIN_7);
		ROM_GPIOPinConfigure(GPIO_PA6_I2C1SCL);
		ROM_GPIOPinConfigure(GPIO_PA7_I2C1SDA);
		IntPrioritySet( INT_I2C1, configMAX_SYSCALL_INTERRUPT_PRIORITY );
		IntEnable(INT_I2C1);
		I2CMasterIntEnableEx(I2C1_MASTER_BASE, I2C_MASTER_INT_DATA);
		I2CMasterInitExpClk(I2C1_MASTER_BASE, SysCtlClockGet(), true);

		xQueueReset( I2C1_messagequeue );

		vTaskDelay( 2 );

	}// End outer while(1)

}

portBASE_TYPE I2C1_QueueMessage(tI2C_message * I2C_message){
	return xQueueSendToBack( I2C1_messagequeue, (void *) I2C_message, 0); // Don't wait if queue is full
}


void I2C1MasterIntHandler(void)
{
	static signed portBASE_TYPE xHigherPriorityTaskWoken;

    // Clear the I2C1 interrupt flag.
    I2CMasterIntClear(I2C1_MASTER_BASE);



    if( I2C1_active_message.read){	// *** this is a READ ***

    	if( I2C1_data_ptr == 255){ // This is the first read
    		I2CMasterSlaveAddrSet(I2C1_MASTER_BASE, I2C1_active_message.slave_addr, true);
    		I2C1_data_ptr = 0;
    		// Setup the read with a repeated start
    		if( I2C1_active_message.bytes == 1){
    			// Single-byte read
    			I2CMasterControl(I2C1_MASTER_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE);
    			return;
    		}else{
    			// Multi-byte (burst) read
    			I2CMasterControl(I2C1_MASTER_BASE, I2C_MASTER_CMD_BURST_RECEIVE_START);
    			return;
    		}
    	}else{
    		// Get the next data byte
    		I2C1_active_message.data[I2C1_data_ptr++] = I2CMasterDataGet(I2C1_MASTER_BASE);
    		if( I2C1_data_ptr == I2C1_active_message.bytes -1 )
    		{
    			// Only one byte left, do one more read
    			I2CMasterControl(I2C1_MASTER_BASE, I2C_MASTER_CMD_BURST_RECEIVE_FINISH);
    			return;
    		}else if( I2C1_data_ptr == I2C1_active_message.bytes ) {
    			// No bytes left, return the semaphore
    			xSemaphoreGiveFromISR( sh_I2C1_busy, &xHigherPriorityTaskWoken );
    			return;
    		}else{
    			// more than one byte left, do another read
    			I2CMasterControl(I2C1_MASTER_BASE, I2C_MASTER_CMD_BURST_RECEIVE_CONT);
    			return;
    		}

    	}

    }else {  // *** this is a WRITE ***

		// Returning after issuing stop condition
		if(I2C1_data_ptr >= I2C1_active_message.bytes)
		{
			// Return the semaphore
			xSemaphoreGiveFromISR( sh_I2C1_busy, &xHigherPriorityTaskWoken );
			return;
		}

    	if( ++I2C1_data_ptr >= I2C1_active_message.bytes ){
    		// No more bytes left in message, send stop signal
    		I2CMasterControl(I2C1_MASTER_BASE, I2C_MASTER_CMD_BURST_SEND_ERROR_STOP);
    		return;
    	}

    	// else send next byte
    	I2CMasterDataPut(I2C1_MASTER_BASE, I2C1_active_message.data[I2C1_data_ptr] );
    	I2CMasterControl(I2C1_MASTER_BASE, I2C_MASTER_CMD_BURST_SEND_CONT);
    }
}

