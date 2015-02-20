/*
 * comm.h
 *
 *  Created on: Sep 13, 2013
 *      Author: bmichini
 */

#ifndef COMM_H_
#define COMM_H_

#include <stdint.h>

#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_i2c.h"
#include "inc/hw_ints.h"
#include "driverlib/interrupt.h"
#include "driverlib/i2c.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/rom.h"
#include "driverlib/uart.h"

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "semphr.h"
#include "../priorities.h"

// $$ Initialization $$ //
void init_comm();
char* ftoa(float f);


// ##### UART Library ##### //
void UART0_init();
void UART5_init();
void UART5IntHandler(void);


// ##### I2C Library ##### //
typedef struct I2C_message{
	tBoolean 	read;
	uint8_t		read_addr;
	uint8_t 	bytes;
	uint8_t 	slave_addr;
	uint8_t*	data;
}tI2C_message;

void I2C1_init( tBoolean fastmode );
portBASE_TYPE I2C1_QueueMessage(tI2C_message * I2C_message); // Function which queues an I2C read/write message for transmission
void I2C1_QueueHandler(void *pvParameters); // Task which continually handles I2C message queue
#define I2C1_MESSAGE_QUEUE_SIZE 12
#define I2C1_QUEUEHANDLER_PRIORITY 10
#define I2C1_TIMEOUT_THRESH 1*portTICKS_PER_MS


#endif /* COMM_H_ */
