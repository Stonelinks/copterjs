/*
 * sensors.c
 *
 *  Created on: Sep 17, 2013
 *      Author: bmichini
 */
#include "sensors.h"

#include "../comm/comm.h"

tSensorBytes sensorBytes;
tSensorData  sensorData;
tI2C_message sensor_read;
uint8_t sensorStatus = SENSOR_STATUS_START;

int gyro_bias_reads = GYRO_BIAS_SAMPLES;

uint8_t ADXL345_cfg1[2] = {ADXL345_REG_POWER_CTL, ADXL345_MODE_MEASURE};
uint8_t ITG3200_cfg1[2] = {ITG3200_REG_DLPF_FS, ITG3200_MODE_2000DPS_8KHZ};
uint8_t MPU9150_cfg1[2] = {MPU9150_REG_PWRMGMT1, MPU9150_PWRMGMT_UNSLEEP};
uint8_t MPU9150_accelcfg1[2] = {MPU9150_REG_ACCELCONFIG, MPU9150_ACCELCONFIG_8G};
uint8_t MPU9150_gyrocfg1[2] = {MPU9150_REG_GYROCONFIG, MPU9150_GYROCONFIG_2000DPS};
uint8_t press_start = MS5611_PRESS_SAMP_4096;
uint8_t temp_press_start = MS5611_TEMP_SAMP_4096;

xSemaphoreHandle sh_BiasDone;

void init_sensors(){

#ifdef MPU9150
	// Unsleep the MPU-9150
	tI2C_message msg_configure;
	msg_configure.read = false;
	msg_configure.slave_addr = MPU9150_ADDR;
	msg_configure.bytes = 2;
	msg_configure.data = MPU9150_cfg1;
	I2C1_QueueMessage( &msg_configure );

	// Wait while unsleeping takes place
	vTaskDelay( 100 * portTICKS_PER_MS );
#endif

	init_accel();

	init_gyro();

	//init_press();

	// Create semaphore for bias capture
	vSemaphoreCreateBinary( sh_BiasDone );
	xSemaphoreTake( sh_BiasDone ,portMAX_DELAY);

	// Start gyro bias capture
	capture_gyro_bias();
}

void init_press()
{

#ifdef MS5611
	// Read the MS5611 cal data
	sensorData.press_Pa = 101325.0;
	sensorData.C1 = 0; // Will be non-zero when the pressure cal has be written in
	sensorBytes.press_cal_bytes[10] = 0; // Will be non-zero when the pressure cal has be read from i2c
	sensorBytes.temp_press_bytes[0] = 0; // Will be non-zero once first temp is read
	read_press_cal();
#endif

}


float dT, OFF, SENS, D1, D2;
void scale_press(){
	static uint32_t tmp1, tmp2;

	// Check if calibration needs to be read in
	if( sensorData.C1 == 0  ){
		// The calibration hasn't been read in
		// Check to make sure the I2C message has been received
		if( sensorBytes.press_cal_bytes[10] == 0 ){
			// Haven't gotten the calibration bytes over I2C yet, return until later
			return;
		}

		// Otherwise we have gotten I2C calibration bytes

		tmp1 = sensorBytes.press_cal_bytes[0]; tmp1 = (tmp1 << 8); tmp1 += sensorBytes.press_cal_bytes[1];
		sensorData.C1 = (float)tmp1;

		tmp1 = sensorBytes.press_cal_bytes[2]; tmp1 = (tmp1 << 8); tmp1 += sensorBytes.press_cal_bytes[3];
		sensorData.C2 = (float)tmp1;

		tmp1 = sensorBytes.press_cal_bytes[4]; tmp1 = (tmp1 << 8); tmp1 += sensorBytes.press_cal_bytes[5];
		sensorData.C3 = (float)tmp1;

		tmp1 = sensorBytes.press_cal_bytes[6]; tmp1 = (tmp1 << 8); tmp1 += sensorBytes.press_cal_bytes[7];
		sensorData.C4 = (float)tmp1;

		tmp1 = sensorBytes.press_cal_bytes[8]; tmp1 = (tmp1 << 8); tmp1 += sensorBytes.press_cal_bytes[9];
		sensorData.C5 = (float)tmp1;

		tmp1 = sensorBytes.press_cal_bytes[10]; tmp1 = (tmp1 << 8); tmp1 += sensorBytes.press_cal_bytes[11];
		sensorData.C6 = (float)tmp1;
	} // End read in calibration values

	// Have we read a temperature yet?
	if( sensorBytes.temp_press_bytes[0] == 0)
		return;

	// Do the pressure conversion
	tmp1 = sensorBytes.press_bytes[0]; tmp1 = (tmp1 << 16); tmp2 = sensorBytes.press_bytes[1]; tmp2 = (tmp2 << 8);
	D1 = (float)(tmp1 + tmp2 + sensorBytes.press_bytes[2]);

	// Sanity check the sensor reading, sometimes it reads zero
	if( D1 < 1.0 ){
		return;
	}

	tmp1 = sensorBytes.temp_press_bytes[0]; tmp1 = (tmp1 << 16); tmp2 = sensorBytes.temp_press_bytes[1]; tmp2 = (tmp2 << 8);
	D2 = (float)(tmp1 + tmp2 + sensorBytes.temp_press_bytes[2]);

	dT = D2 - sensorData.C5*256.0;
	OFF = sensorData.C2*65536.0 + (sensorData.C4*dT)/128.0;
	SENS = sensorData.C1*32768.0 + (sensorData.C3*dT)/256.0;

	sensorData.press_Pa = (D1*SENS/2097152.0 - OFF) / 32768.0;
	sensorData.baroAlt = 44330.8 - 4946.54 * pow(sensorData.press_Pa, 0.1902632);

}

void start_press(){
	sensor_read.read = false;
	sensor_read.slave_addr = MS5611_ADDR;
	sensor_read.bytes = 1;
	sensor_read.data = &press_start;
	I2C1_QueueMessage(&sensor_read);
}

void start_temp_press(){
	sensor_read.read = false;
	sensor_read.slave_addr = MS5611_ADDR;
	sensor_read.bytes = 1;
	sensor_read.data = &temp_press_start;
	I2C1_QueueMessage(&sensor_read);
}

void read_press(){
	sensor_read.read = true;
	sensor_read.slave_addr = MS5611_ADDR;
	sensor_read.read_addr  = MS5611_REG_DATA;
	sensor_read.bytes = 3;
	sensor_read.data = sensorBytes.press_bytes;
	I2C1_QueueMessage(&sensor_read);
}

void read_temp_press(){
	sensor_read.read = true;
	sensor_read.slave_addr = MS5611_ADDR;
	sensor_read.read_addr  = MS5611_REG_DATA;
	sensor_read.bytes = 3;
	sensor_read.data = sensorBytes.temp_press_bytes;
	I2C1_QueueMessage(&sensor_read);
}

void read_press_cal(){
	sensor_read.read = true;
	sensor_read.slave_addr = MS5611_ADDR;
	sensor_read.read_addr  = MS5611_REG_CAL + 2;
	sensor_read.bytes = 2;
	sensor_read.data = &sensorBytes.press_cal_bytes[0];
	I2C1_QueueMessage(&sensor_read);

	sensor_read.read_addr  = MS5611_REG_CAL + 4;
	sensor_read.data = &sensorBytes.press_cal_bytes[2];
	I2C1_QueueMessage(&sensor_read);

	sensor_read.read_addr  = MS5611_REG_CAL + 6;
	sensor_read.data = &sensorBytes.press_cal_bytes[4];
	I2C1_QueueMessage(&sensor_read);

	sensor_read.read_addr  = MS5611_REG_CAL + 8;
	sensor_read.data = &sensorBytes.press_cal_bytes[6];
	I2C1_QueueMessage(&sensor_read);

	sensor_read.read_addr  = MS5611_REG_CAL + 10;
	sensor_read.data = &sensorBytes.press_cal_bytes[8];
	I2C1_QueueMessage(&sensor_read);

	sensor_read.read_addr  = MS5611_REG_CAL + 12;
	sensor_read.data = &sensorBytes.press_cal_bytes[10];
	I2C1_QueueMessage(&sensor_read);

}

void init_gyro(){

#ifdef ITG3200
	// Configure the ITG-3200 gyro
	tI2C_message msg_configure;
	msg_configure.read = false;
	msg_configure.slave_addr = ITG3200_ADDR;
	msg_configure.bytes = 2;
	msg_configure.data = ITG3200_cfg1;
	I2C1_QueueMessage( &msg_configure );
#endif

#ifdef MPU9150
	// Configure the MPU-9150 gyro
	tI2C_message msg_configure;
	msg_configure.read = false;
	msg_configure.slave_addr = MPU9150_ADDR;
	msg_configure.bytes = 2;
	msg_configure.data = MPU9150_gyrocfg1;
	I2C1_QueueMessage( &msg_configure );

#endif

}

void read_gyro(){

#ifdef ITG3200
	sensor_read.read = true;
	sensor_read.slave_addr = ITG3200_ADDR;
	sensor_read.read_addr  = ITG3200_REG_DATA;
	sensor_read.bytes = 6;
	sensor_read.data = (uint8_t*)sensorBytes.gyro_bytes;
	I2C1_QueueMessage(&sensor_read);
#endif

#ifdef MPU9150
	sensor_read.read = true;
	sensor_read.slave_addr = MPU9150_ADDR;
	sensor_read.read_addr  = MPU9150_REG_GYRODATA;
	sensor_read.bytes = 6;
	sensor_read.data = (uint8_t*)sensorBytes.gyro_bytes;
	I2C1_QueueMessage(&sensor_read);
#endif

}

void capture_gyro_bias(){
	gyro_bias_reads = GYRO_BIAS_SAMPLES;
	sensorData.bias_gyro_x = 0.0;
	sensorData.bias_gyro_y = 0.0;
	sensorData.bias_gyro_z = 0.0;
	sensorStatus = SENSOR_STATUS_BIAS;
}

extern tINSstate INS;
void scale_gyro(){
	static int16_t tmp;
	static uint8_t* ptr;
	ptr = (uint8_t*) &tmp;
	static float gx, gy, gz;

	if(Button_get(RIGHT_BUTTON)){
		capture_gyro_bias();
	}

#ifdef ITG3200
	ptr[0] = sensorBytes.gyro_bytes[1];
	ptr[1] = sensorBytes.gyro_bytes[0];
	gx = ((float)tmp)/ 800.0;  //823.62683;

	ptr[0] = sensorBytes.gyro_bytes[3];
	ptr[1] = sensorBytes.gyro_bytes[2];
	gy = ((float)tmp)/ 800.0; //823.62683;

	ptr[0] = sensorBytes.gyro_bytes[5];
	ptr[1] = sensorBytes.gyro_bytes[4];
	gz = ((float)tmp)/ 800.0; //823.62683;
#endif

#ifdef MPU9150
	ptr[0] = sensorBytes.gyro_bytes[1];
	ptr[1] = sensorBytes.gyro_bytes[0];
	gx = ((float)tmp)/ 939.68; // LSB/radps for fcale factor for 2000dps

	ptr[0] = sensorBytes.gyro_bytes[3];
	ptr[1] = sensorBytes.gyro_bytes[2];
	gy = ((float)tmp)/ 939.68; // LSB/radps for fcale factor for 2000dps

	ptr[0] = sensorBytes.gyro_bytes[5];
	ptr[1] = sensorBytes.gyro_bytes[4];
	gz = ((float)tmp)/ 939.68; // LSB/radps for fcale factor for 2000dps
#endif

	if( gyro_bias_reads > 0 ){
		gyro_bias_reads--;

		sensorData.bias_gyro_x += gx;
		sensorData.bias_gyro_y += gy;
		sensorData.bias_gyro_z += gz;

		sensorData.p = 0.0;
		sensorData.q = 0.0;
		sensorData.r = 0.0;

		sensorStatus = SENSOR_STATUS_BIAS;
		LED_set( LED_BLUE, true );
		return;
	}else if(gyro_bias_reads == 0){
		sensorData.bias_gyro_x /= ((float)GYRO_BIAS_SAMPLES);
		sensorData.bias_gyro_y /= ((float)GYRO_BIAS_SAMPLES);
		sensorData.bias_gyro_z /= ((float)GYRO_BIAS_SAMPLES);
		gyro_bias_reads--;

		INS.bias_p = INS.bias_q = INS.bias_r = 0.0;

		LED_set( LED_BLUE, false );

		xSemaphoreGive( sh_BiasDone );
	}
	sensorStatus = SENSOR_STATUS_RUNNING;

	sensorData.p = gx - sensorData.bias_gyro_x;
	sensorData.q = gy - sensorData.bias_gyro_y;
	sensorData.r = gz - sensorData.bias_gyro_z;
}

void init_accel(){

#ifdef ADXL345
	// Configure the ADXL345 accelerometer
	tI2C_message msg_configure;
	msg_configure.read = false;
	msg_configure.slave_addr = ADXL345_ADDR;
	msg_configure.bytes = 2;
	msg_configure.data = ADXL345_cfg1;
	I2C1_QueueMessage( &msg_configure );
#endif

#ifdef MPU9150
	// Configure the MPU-9150 accel
	tI2C_message msg_configure;
	msg_configure.read = false;
	msg_configure.slave_addr = MPU9150_ADDR;
	msg_configure.bytes = 2;
	msg_configure.data = MPU9150_accelcfg1;
	I2C1_QueueMessage( &msg_configure );
#endif

}

void read_accel(){

#ifdef ADXL345
	// Read the accelerometer
	sensor_read.read = true;
	sensor_read.slave_addr = ADXL345_ADDR;
	sensor_read.read_addr  = ADXL345_REG_DATA;
	sensor_read.bytes = 6;
	sensor_read.data = (uint8_t*)sensorBytes.accel_bytes;
	I2C1_QueueMessage(&sensor_read);
#endif

#ifdef MPU9150
	// Read the accelerometer
	sensor_read.read = true;
	sensor_read.slave_addr = MPU9150_ADDR;
	sensor_read.read_addr  = MPU9150_REG_ACCELDATA;
	sensor_read.bytes = 6;
	sensor_read.data = (uint8_t*)sensorBytes.accel_bytes;
	I2C1_QueueMessage(&sensor_read);
#endif

}

void scale_accel(){

#ifdef ADXL345
	sensorData.acc_x = ((float)sensorBytes.accel_bytes[0])*0.004;
	sensorData.acc_y = ((float)sensorBytes.accel_bytes[1])*0.004;
	sensorData.acc_z = ((float)sensorBytes.accel_bytes[2])*0.004;
#endif


#ifdef MPU9150
	static int16_t tmp;
	static uint8_t* ptr;
	ptr = (uint8_t*) &tmp;
	ptr[0] = sensorBytes.accel_bytes[1];
	ptr[1] = sensorBytes.accel_bytes[0];
	//sensorData.acc_x = ((float)tmp)/ 4096.0; // LSB/g for scale factor of 8g
	sensorData.acc_x = ((float)tmp)/ 418.0; // LSB/mpsps for scale factor of 8g

	ptr[0] = sensorBytes.accel_bytes[3];
	ptr[1] = sensorBytes.accel_bytes[2];
	//sensorData.acc_y = ((float)tmp)/ 4096.0; // LSB/g for scale factor of 8g
	sensorData.acc_y = ((float)tmp)/ 418.0; // LSB/mpsps for scale factor of 8g

	ptr[0] = sensorBytes.accel_bytes[5];
	ptr[1] = sensorBytes.accel_bytes[4];
	//sensorData.acc_z = ((float)tmp)/ 4096.0; // LSB/g for scale factor of 8g
	sensorData.acc_z = ((float)tmp)/ 418.0; // LSB/mpsps for scale factor of 8g
#endif

}

extern tINSstate INS;
void debug_printsensors(){

	int p = (int)(sensorData.p*1000.0);
	int q = (int)(sensorData.q*1000.0);
	int r = (int)(sensorData.r*1000.0);
	int ax = (int)(sensorData.acc_x*1000.0);
	int ay = (int)(sensorData.acc_y*1000.0);
	int az = (int)(sensorData.acc_z*1000.0);

	UARTprintf("i,%d,%d,%d,%d,%d,%d\n", p,q,r,ax,ay,az);

	//UARTprintf("gyroX: %s ", ftoa(sensorData.p * 57.3));
	//UARTprintf("gyroY: %s ", ftoa(sensorData.q * 57.3));
	//UARTprintf("gyroZ: %s \n", ftoa(sensorData.r * 57.3));

	//UARTprintf( "baro alt: %s  ",  ftoa(sensorData.baroAlt ) );
	//UARTprintf( "baro alt LPF: %s ", ftoa(INS.baroAlt_LPF ) );
	//UARTprintf( "dbaro: %s \n", ftoa(INS.dBaroAlt ) );

	//UARTprintf("accX: %s ", ftoa(sensorData.acc_x));
	//UARTprintf("accY: %s ", ftoa(sensorData.acc_y));
	//UARTprintf("accZ: %s \n", ftoa(sensorData.acc_z));
}
