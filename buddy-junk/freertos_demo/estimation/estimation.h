/*
 * estimation.h
 *
 *  Created on: Sep 20, 2013
 *      Author: bmichini
 */

#ifndef ESTIMATION_H_
#define ESTIMATION_H_


#include "sensors/sensors.h"
#include "mathlib/mathlib.h"
#include "comm/comm.h"
#include "control/control.h"
#include "utils/utils.h"

#include "inc/hw_types.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "../priorities.h"
#include "utils/uartstdio.h"


typedef struct INSstate{
	tQuaternion att;
	float roll;
	float pitch;
	float yaw;
	float p;
	float q;
	float r;
	float bias_p;
	float bias_q;
	float bias_r;
	float ax;
	float ay;
	float az;
	float bias_ax;
	float bias_ay;
	float bias_az;

	float baroAlt_LPF;
	float dBaroAlt;
	float dBaroAlt_HPF;

}tINSstate;

typedef struct INSparameters{
	float K_baroAlt_LPF;
	float K_att_correct;
	float dt;

	float K_accel_LPF;

	float K_dBaroAlt;
	float K_dBaroAlt_HPF;

	float K_gyroBias;
	float sat_gyroBias;
}tINSparameters;


void init_estimation();

void debug_printINS();

#define ESTIMATION_TASK_INTERVAL_MS 1
void EstimationTask(void *pvParameters);

int8_t GetAttAccel();

#endif /* ESTIMATION_H_ */
