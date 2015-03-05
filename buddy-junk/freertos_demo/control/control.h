/*
 * control.h
 *
 *  Created on: Sep 27, 2013
 *      Author: bmichini
 */

#ifndef CONTROL_H_
#define CONTROL_H_

// Set which kind of quad this is
//#define MINIQUAD
#define BUDDYQUAD

#include "estimation/estimation.h"
#include "mathlib/mathlib.h"
#include "utils/utils.h"

#include "FreeRTOS.h"
#include "semphr.h"

#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_timer.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include "driverlib/rom.h"

// ## Integrator ## //
typedef struct sIntegrator{
	float value;
	float min;
	float max;
	float K;
}tIntegrator;

void Int_init(tIntegrator * integ, float min, float max, float K);
void Int_increment(tIntegrator * integ,  float val, float dt);
void Int_reset(tIntegrator * integ);
float Int_valgain(tIntegrator * integ);


// ## Controller parameters ## //
typedef struct sControlParams{
	float RC_rollpitch_scale;
	float RC_yawrate_scale;
	float Kp_roll;
	float Kp_pitch;
	float Kd_roll;
	float Kd_pitch;
	float Ki_pitch;
	float Ki_roll;

	float Kp_yawrate;
	float Ki_yawrate;

	float max_att_integrator;

	tIntegrator Int_roll;
	tIntegrator Int_pitch;
	tIntegrator Int_yawrate;
}tControlParams;

typedef struct EffectorSetpoint{
	float roll;
	float pitch;
	float yaw;
	float thrust;
	tBoolean kill;
}tEffectorSetpoint;

typedef struct sRCinput{
	float chan[4];
	float chan_offset[4];
	tBoolean sw[2];
	tBoolean TXlost;
}tRCinput;

void init_control();

void RunController( float dt );

void AttitudeControl( float dt );

void UpdateEffectors( );

void RC_init();
void UpdateSbus( );
void RC_GetOffset( );

// ## Effector-specific functions ## //

#define PWM_FREQUENCY 400 // Hz
void EffectorConfigure_QuadPWM();
void EffectorSet_QuadPWM( tEffectorSetpoint *effectors);


#endif /* CONTROL_H_ */
