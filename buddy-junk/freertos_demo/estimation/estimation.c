/*
 * estimation.c
 *
 *  Created on: Sep 20, 2013
 *      Author: bmichini
 */


#include "estimation.h"

tINSstate INS;
tINSparameters INSparam;
tQuaternion att_accel;

extern tSensorData  sensorData;
extern tRCinput RCin;
extern uint8_t sensorStatus;
extern xSemaphoreHandle sh_BiasDone;
extern xSemaphoreHandle sh_EdgeActive;

xTaskHandle th_estimationtask;

void init_estimation(){

	// Initialize the INS state vector
	INS.att = qunit();
	INS.p = INS.q = INS.r = INS.bias_p = INS.bias_q = INS.bias_r = 0.0;
	INS.ax = INS.ay = 0.0; INS.az = 1.0;
	sensorData.acc_x_LPF = sensorData.acc_y_LPF = 0.0; sensorData.acc_z_LPF = 1.0;
	INS.bias_ax = INS.bias_ay = INS.bias_az = 0.0;
	INS.baroAlt_LPF = 0.0;
	INS.dBaroAlt = INS.dBaroAlt_HPF = 0.0;

	// Set the INS parameters
	INSparam.dt = ((float)ESTIMATION_TASK_INTERVAL_MS)*0.001;
	INSparam.K_att_correct = 0.075 * INSparam.dt;

	INSparam.K_accel_LPF = 2.0 * INSparam.dt; // 0.9

	INSparam.K_baroAlt_LPF = 1.0 *  5.0* INSparam.dt; // Runs every 10 dt's

	INSparam.K_dBaroAlt = 0.7;
	INSparam.K_dBaroAlt_HPF = INSparam.K_dBaroAlt / ( INSparam.K_dBaroAlt + 5.0*INSparam.dt  ); // Runs every 10 dt's

	INSparam.K_gyroBias = 0.06 * INSparam.dt;
	INSparam.sat_gyroBias = 0.2 * INSparam.dt;

	// Start the estimation task
	xTaskCreate( EstimationTask, "Estimation Task", 2*configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+ESTIMATOR_TASK, &th_estimationtask );

}


void EstimationTask(void *pvParameters){
	// Initialize the sensors now that the I2C task is running
	init_sensors();

	// Delay while sensors are being initialized
	vTaskDelay( 10 * portTICKS_PER_MS );

    // Get the current tick count.
    portTickType ulWakeTime = xTaskGetTickCount();

    // Todo: Measure dt from timer ticks

    static tQuaternion att_meas;
    static unsigned int ticks=0;
    static tBoolean tempRead = false;
	while(1){
		ticks++;


#ifdef MS5611
		// Read pressure sensor every 10ms
		if( ticks % 5 == 0){

			if( tempRead ){
				read_temp_press();
				tempRead = false;
			}else{
				read_press();
			}

			if( ticks % 500 == 0){
				start_temp_press(); // Measure the temperature every 1000ms
				tempRead = true;
			}else{
				start_press();
			}
			scale_press();

			static float baroAlt_last;
			baroAlt_last = INS.baroAlt_LPF;
			if( INS.baroAlt_LPF == 0.0 ){
				INS.baroAlt_LPF = sensorData.baroAlt;
			}else{
				INS.baroAlt_LPF += ( sensorData.baroAlt - INS.baroAlt_LPF ) * INSparam.K_baroAlt_LPF;
			}

			// Estimate baro vertical rate
			INS.dBaroAlt_HPF = INSparam.K_dBaroAlt_HPF*( INS.dBaroAlt_HPF + INS.baroAlt_LPF - baroAlt_last );
			INS.dBaroAlt = INS.dBaroAlt_HPF / INSparam.K_dBaroAlt;
		}
#endif

		// Scale the sensors from the last I2C read
		scale_accel();
		scale_gyro();

		// Queue next I2C sensor messages
		read_accel();
		read_gyro();

		// Apply estimated biases
		INS.p = sensorData.p - INS.bias_p;
		INS.q = sensorData.q - INS.bias_q;
		INS.r = sensorData.r - INS.bias_r;

		// Convert accels
		sensorData.acc_x_LPF += (sensorData.acc_x - sensorData.acc_x_LPF)*INSparam.K_accel_LPF;
		sensorData.acc_y_LPF += (sensorData.acc_y - sensorData.acc_y_LPF)*INSparam.K_accel_LPF;
		sensorData.acc_z_LPF += (sensorData.acc_z - sensorData.acc_z_LPF)*INSparam.K_accel_LPF;
		INS.ax = sensorData.acc_x_LPF - INS.bias_ax;
		INS.ay = sensorData.acc_y_LPF - INS.bias_ay;
		INS.az = sensorData.acc_z_LPF - INS.bias_az;


		// Propagate attitude from gyros
		qpropagate( & INS.att, INS.p, INS.q, INS.r, INSparam.dt );

		// If just activated, set accel biases, get attitude, set est attitude
		if( xSemaphoreTake( sh_EdgeActive, 0) ){
			INS.bias_ax = INS.ax + INS.bias_ax;
			INS.bias_ay = INS.ay + INS.bias_ay;
			// TODO: Set z bias
			INS.att = qunit();
		}else{
			// Else just calculate accelerometer-based roll/pitch
			GetAttAccel();
		}
		// If bias done, set est to measured and reset biases
		if( xSemaphoreTake( sh_BiasDone, 0) ){
			//INS.att.o = att_accel.o;
			//INS.att.x = att_accel.x;
			//INS.att.y = att_accel.y;
			//INS.att.z = att_accel.z;
			INS.att = qunit();
			INS.bias_p = 0.0;
			INS.bias_q = 0.0;
			INS.bias_r = 0.0;
		}

		// Correct attitude
		att_meas = qunit();
		//qcorrect(&INS.att, &att_accel, INSparam.K_att_correct, 0.004);
		qcorrect(&INS.att, &att_meas, INSparam.K_att_correct, 0.004);

		// Convert to RPY
		q2Euler( &INS.att, &INS.roll, &INS.pitch, &INS.yaw, false );

		// Estimate gyro bias
		tQuaternion Qbias;
		//Qbias = qprod(qconj(INS.att), att_accel);
		Qbias = qprod(qconj(INS.att), att_meas);
		INS.bias_p -= INSparam.K_gyroBias*satf( Qbias.x, -INSparam.sat_gyroBias, INSparam.sat_gyroBias);
		INS.bias_q -= INSparam.K_gyroBias*satf( Qbias.y, -INSparam.sat_gyroBias, INSparam.sat_gyroBias);
		//INS.bias_r -= INSparam.K_gyroBias*satf( Qbias.z, -INSparam.sat_gyroBias, INSparam.sat_gyroBias);

		// Run controller
		RunController( INSparam.dt );

		// Sleep until next execution
		vTaskDelayUntil(&ulWakeTime, ESTIMATION_TASK_INTERVAL_MS * portTICKS_PER_MS);
	}

}

int8_t GetAttAccel(){
    // todo: try using accelerometer values minus biases here -- didn't make much difference with test data
    float ax = -INS.ax;
    float ay = -INS.ay;
    float az = -INS.az;

    float root = sqrtf(ax*ax + ay*ay + az*az);
    if ((root < 0.5) || (root > 1.5))
            return -2;

    ax /= root;
    ay /= root;
    az /= root;

    if ((ax > 0.998) || (-ax > 0.998))
            return -3;

    root = sqrtf(ay*ay + az*az);
    if (root < 0.0001)
            root = 0.0001;

    float sinR = -ay/root;
    float cosR = -az/root;

    float sinP = ax;
    float cosP = -(ay*sinR + az*cosR);

    float cosR2 = sqrt((cosR+1)*0.5);
    if (cosR2 < 0.0001)
            cosR2 = 0.0001;

    float sinR2 = sinR/cosR2*0.5; //WARNING: This step is numerically ill-behaved!

    float cosP2 = sqrt((cosP+1)*0.5);
    if (cosP2 < 0.0001)
            cosP2 = 0.0001;

    float sinP2 = sinP/cosP2*0.5; //WARNING: This step is numerically ill-behaved!

    if (((cosR2*cosR2 + sinR2*sinR2) > 1.1) || ((cosP2*cosP2 + sinP2*sinP2) > 1.1))
            return -4;


    // Finally get quaternion
    tQuaternion qroll,qpitch;
	qpitch.o = cosP2; qpitch.x = 0;    qpitch.y = sinP2; qpitch.z = 0;
	qroll.o  = cosR2; qroll.x = sinR2; qroll.y = 0;      qroll.z = 0;


	att_accel = qprod(qunit(),qpitch);
	att_accel = qprod(att_accel, qroll);


    return 0;
}

extern float m1,m2,m3,m4;
void debug_printINS(){
	static float roll, pitch, yaw;

	q2Euler(&INS.att, &roll, &pitch, &yaw, false);

	//UARTprintf("roll: %s ", ftoa(roll));
	//UARTprintf("pitch: %s ", ftoa(pitch));
	//UARTprintf("yaw: %s \n", ftoa(yaw));

	//q2Euler(&att_accel, &roll, &pitch, &yaw, true);
	int roll_int = (int)(roll*100.0);
	int pitch_int = (int)(pitch*100.0);
	UARTprintf("a,%d,%d\n", roll_int,pitch_int);
	//UARTprintf("a_roll: %s ", ftoa(roll));
	//UARTprintf("a_pitch: %s \n", ftoa(pitch));

	//UARTprintf("b_p: %s ", ftoa(INS.bias_p*10000.0));
	//UARTprintf("b_q: %s ", ftoa(INS.bias_q*10000.0));
	//UARTprintf("b_r: %s \n", ftoa(INS.bias_r*10000.0));

	//UARTprintf("m1: %s ", ftoa(m1));
	//UARTprintf("m2: %s ", ftoa(m2));
	//UARTprintf("m3: %s ", ftoa(m3));
	//UARTprintf("m4: %s \n", ftoa(m4));

	//UARTprintf("RC0: %s ", ftoa(RCin.chan[0]));
	//UARTprintf("RC1: %s ", ftoa(RCin.chan[1]));
	//UARTprintf("RC2: %s ", ftoa(RCin.chan[2]));
	//UARTprintf("RC3: %s \n", ftoa(RCin.chan[3]));
	//if(RCin.sw[0]){
	//	UARTprintf("SW0 ON \n");
	//}else
	//{
	//	UARTprintf("SW0 OFF \n");
	//}
}
