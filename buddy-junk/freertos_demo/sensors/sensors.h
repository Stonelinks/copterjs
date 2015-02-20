/*
 * sensors.h
 *
 *  Created on: Sep 17, 2013
 *      Author: bmichini
 */

#ifndef SENSORS_H_
#define SENSORS_H_

#include <stdint.h>
#include <math.h>
#include "utils/uartstdio.h"
#include "utils/utils.h"

#include "FreeRTOS.h"
#include "semphr.h"

#include "estimation/estimation.h"

// Select which sensors the vehicle has installed
//#define MS5611
//#define ITG3200
//#define ADXL345
#define MPU9150

#define SENSOR_STATUS_BIAS 0
#define SENSOR_STATUS_RUNNING 1
#define SENSOR_STATUS_START 2

#define ADXL345_ADDR 0x53
#define ADXL345_REG_POWER_CTL 0x2D
#define ADXL345_MODE_MEASURE 0x08
#define ADXL345_REG_DATA 0x32

#define ITG3200_ADDR 0x68
#define ITG3200_REG_DLPF_FS 0x16
#define ITG3200_MODE_2000DPS_8KHZ 0x18
#define ITG3200_REG_DATA 0x1D

#define MS5611_ADDR 0x76
#define MS5611_PRESS_SAMP_4096 0x48
#define MS5611_TEMP_SAMP_4096 0x58
#define MS5611_REG_DATA 0x00
#define MS5611_REG_CAL 0xA0

#define MPU9150_ADDR 0x68
#define MPU9150_REG_GYROCONFIG 0x1B
#define MPU9150_GYROCONFIG_2000DPS 0x18
#define MPU9150_REG_GYRODATA 0x43

#define MPU9150_REG_PWRMGMT1 0x6B
#define MPU9150_PWRMGMT_UNSLEEP 0x00

#define MPU9150_REG_ACCELCONFIG 0x1C
#define MPU9150_ACCELCONFIG_8G 0x10
#define MPU9150_REG_ACCELDATA 0x3B

#define GYRO_BIAS_SAMPLES 1000

#ifdef ADXL345
typedef struct SensorBytes{
	int16_t accel_bytes[3];
	uint8_t gyro_bytes[6];
	uint8_t press_bytes[3];
	uint8_t temp_press_bytes[3];
	uint8_t press_cal_bytes [12];
}tSensorBytes;
#endif

#ifdef MPU9150
typedef struct SensorBytes{
	uint8_t accel_bytes[6];
	uint8_t gyro_bytes[6];
	uint8_t press_bytes[3];
	uint8_t temp_press_bytes[3];
	uint8_t press_cal_bytes [12];
}tSensorBytes;
#endif

typedef struct SensorData{
	float acc_x;
	float acc_y;
	float acc_z;
	float acc_x_LPF;
	float acc_y_LPF;
	float acc_z_LPF;

	float p;
	float q;
	float r;
	float bias_gyro_x;
	float bias_gyro_y;
	float bias_gyro_z;

	float press_Pa;
	float temp_C;

	float baroAlt;

	// Cal params for MS5611 pressure sensor
	float C1;
	float C2;
	float C3;
	float C4;
	float C5;
	float C6;

}tSensorData;

void init_sensors();

void capture_gyro_bias();

void start_press();
void start_temp_press();

void init_press();
void read_press();
void read_press_cal();
void read_temp_press();

void init_gyro();
void read_gyro();

void init_accel();
void read_accel();

void scale_press();
void scale_press_cal();
void scale_gyro();
void scale_accel();

void debug_printsensors();

#endif /* SENSORS_H_ */
