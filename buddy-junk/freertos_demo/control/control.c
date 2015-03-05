/*
 * control.c
 *
 *  Created on: Sep 27, 2013
 *      Author: bmichini
 */

#include "control.h"

void (*SetEffectorPtr)(tEffectorSetpoint *);
tEffectorSetpoint effectors;
tControlParams controlParam;
xSemaphoreHandle sh_EdgeActive;

extern tINSstate INS;
extern tRCinput RCin;
extern uint8_t serialCharsInput[5];

void init_control(){

	controlParam.RC_rollpitch_scale = 1.0;
	controlParam.RC_yawrate_scale = 2.0;
	controlParam.Kp_roll = controlParam.Kp_pitch = 0.12;//0.17;
	controlParam.Ki_roll = controlParam.Ki_pitch = 0.05;
	controlParam.Kd_roll = controlParam.Kd_pitch = 0.06;
	controlParam.Kp_yawrate  = 0.1;
	controlParam.Ki_yawrate  = 0.03;

	controlParam.max_att_integrator = 0.2;

	// Initialize integrators
	Int_init( &controlParam.Int_roll, -controlParam.max_att_integrator, controlParam.max_att_integrator, controlParam.Ki_roll  );
	Int_init( &controlParam.Int_pitch, -controlParam.max_att_integrator, controlParam.max_att_integrator, controlParam.Ki_pitch  );
	Int_init( &controlParam.Int_yawrate, -controlParam.max_att_integrator, controlParam.max_att_integrator, controlParam.Ki_yawrate  );

	// Configure the effectors for QuadPWM control
	EffectorConfigure_QuadPWM();
	SetEffectorPtr = &EffectorSet_QuadPWM;

	effectors.kill = true;

	// Init RC input
	RC_init();

	// Create semaphore for bias capture
	vSemaphoreCreateBinary( sh_EdgeActive );
	xSemaphoreTake( sh_EdgeActive ,portMAX_DELAY);
}

extern xSemaphoreHandle sh_Sbus_ready;
void RunController( float dt ){

	// Determine attitude command source
	if( xSemaphoreTake( sh_Sbus_ready , 0) ){
		UpdateSbus( );
	}

	// Set kill mode
	if( RCin.sw[0] ){
		if( effectors.kill ){
			// This is an edge into active mode
			xSemaphoreGive( sh_EdgeActive );
		}
		effectors.kill = false;
	}else{
		effectors.kill = true;
	}

	// Run attitude controller
	AttitudeControl( dt );

	// Send commands to effectors
	UpdateEffectors();

}

void AttitudeControl( float dt ){

	static float rollCmd, pitchCmd, yawRateCmd;
	static float rollErr, pitchErr, yawRateErr;

/*
	rollCmd = -RCin.chan[0] * controlParam.RC_rollpitch_scale;
	pitchCmd = -RCin.chan[1] * controlParam.RC_rollpitch_scale;
	yawRateCmd = -RCin.chan[3] * controlParam.RC_yawrate_scale;
*/
	effectors.thrust = (RCin.chan[2]+1.0) / 2.0;

	float maxAtt_rad = 0.52;// 0.52 rad = 30deg
	float maxAttRate_rad = 0.78;// 0.78 rad = 45deg

	float rollIn = (float)serialCharsInput[1];
	rollIn -= 127.0;
	rollIn = -rollIn /127.0*maxAtt_rad;
	rollCmd = satf( rollIn ,-maxAtt_rad, maxAtt_rad);

	float pitchIn = (float)serialCharsInput[2];
	pitchIn -= 127.0;
	pitchIn = pitchIn /127.0*maxAtt_rad;
	pitchCmd = satf( pitchIn ,-maxAtt_rad, maxAtt_rad);

	float yawRateIn = (float)serialCharsInput[3];
	yawRateIn -= 127.0;
	yawRateIn = -yawRateIn /127.0*maxAttRate_rad;
	yawRateCmd = satf( yawRateIn ,-maxAttRate_rad, maxAttRate_rad);


	rollErr = rollCmd - INS.roll;
	pitchErr = pitchCmd - INS.pitch;
	yawRateErr = yawRateCmd - INS.r;

	// Update integrators if thrust greater than threshold
	if( effectors.thrust > 0.2){
		Int_increment( &controlParam.Int_roll, rollErr, dt );
		Int_increment( &controlParam.Int_pitch, pitchErr, dt );
		Int_increment( &controlParam.Int_yawrate, yawRateErr, dt );
	}

	// Calculate commands
	effectors.roll = controlParam.Kp_roll*rollErr + Int_valgain( &controlParam.Int_roll) - controlParam.Kd_roll*INS.p;
	effectors.pitch = controlParam.Kp_pitch*pitchErr + Int_valgain( &controlParam.Int_pitch) - controlParam.Kd_pitch*INS.q;
	effectors.yaw = controlParam.Kp_yawrate*yawRateErr + Int_valgain( &controlParam.Int_yawrate);

	// Disable attitude control and integrators if thrust less than threshold
	if( effectors.thrust < 0.1 ){
		effectors.roll = 0.0;
		effectors.pitch = 0.0;
		effectors.yaw = 0.0;
		// Reset integrators
		Int_reset( &controlParam.Int_roll );
		Int_reset( &controlParam.Int_pitch );
		Int_reset( &controlParam.Int_yawrate );
	}

}





void UpdateEffectors( ){
	SetEffectorPtr( &effectors );
}


