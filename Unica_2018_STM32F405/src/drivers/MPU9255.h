/*
 * MPU9255.h
 *
 *  Created on: 21 янв. 2017 г.
 *      Author: developer
 */

#ifndef MPU9255_H_
#define MPU9255_H_

#include "state.h"


#define MPU9255_GYRO_SCALE_FACTOR	M_PI / (180 * 131)
#define MPU9255_ACCEL_SCALE_FACTOR	0.00055217

#define X_ACCEL_OFFSET		0x003F//( 0x7FC1 )
#define Y_ACCEL_OFFSET		0x003E//( 0x7FC2 )
#define Z_ACCEL_OFFSET		0x0067//( 0x7F99 )
#define X_ACCEL_KOEFF		1.0796414
#define Y_ACCEL_KOEFF		1.0833494
#define Z_ACCEL_KOEFF		1.0717212

#define X_GYRO_KOEFF		1
#define Y_GYRO_KOEFF		1
#define Z_GYRO_KOEFF		1

#define X_COMPAS_OFFSET		479.1609
#define Y_COMPAS_OFFSET		42.0570
#define Z_COMPAS_OFFSET		191.5415
#define XX_COMPAS_TRANSFORM_MATIX	0.004041
#define YY_COMPAS_TRANSFORM_MATIX	0.004061
#define ZZ_COMPAS_TRANSFORM_MATIX	0.003486
#define XY_COMPAS_TRANSFORM_MATIX	-0.000029
#define XZ_COMPAS_TRANSFORM_MATIX	-0.000166
#define YZ_COMPAS_TRANSFORM_MATIX	-0.000030

#define ACCEL_RANGE			1			//2g - 00, 4g - 01, 8g - 10, 16g - 11
#define GYRO_RANGE			0			//250degps - 00, 500degps - 01, 1000degps - 10, 2000degps - 11


typedef enum {

	RESET_ = (1 << 7)

} mpu9255_params_t;


typedef enum {

	//	full-scale range (FS_SEL_X, +- x = range in g)
	FS_SEL_2G = 0,
	FS_SEL_4G = 1,
	FS_SEL_8G = 2,
	FS_SEL_16G = 3

} accel_params_t;

typedef enum {

	//	full-scale range (FS_SEL_X, +- x = range in dps)
	FS_SEL_250DPS = 0,
	FS_SEL_500DPS = 1,
	FS_SEL_1000DPS = 2,
	FS_SEL_2000DPS = 3

} gyro_params_t;

typedef enum {

	FS_ = 0

} compass_params_t;




// errors
typedef enum {
	MPU9255_ERROR_NONE = 0,
	MPU9255_ERROR_DIV_BY_ZERO = 1
} mpu9255_errors_t;


// addresses of mpu9255 (for getting acsess to accel and gyro)
typedef enum {
	GYRO_AND_ACCEL = 0b1101000,
	COMPASS = 0b0001100,
	BMP180 = 0b11100111

} mpu9255_address_t;


int mpu9255_readRegister(wires_handle_t* handles, mpu9255_address_t address, uint8_t reg_address, uint8_t *dataRead, uint8_t count);
int mpu9255_writeRegister(wires_handle_t* handles, mpu9255_address_t address, uint8_t reg_address, uint8_t dataWrite);

int mpu9255_readIMU(wires_handle_t* handles, int16_t* raw_accel_XYZ, int16_t* raw_gyro_XYZ);	//чтение данных с акселерометра и гироскопа
int mpu9255_readCompass(state_t* state, wires_handle_t* handles, int16_t * raw_compass_XYZ);				//чтение данных с магнитометра


void mpu9255_recalcAccel(const int16_t* raw_accel_XYZ, float* accel_XYZ);	//перевод показаний акселерометра в единицы g
void mpu9255_recalcGyro(const int16_t* raw_gyro_XYZ, float* gyro_XYZ);		//перевод показаний гироскопа в единицы dps
void mpu9255_recalcCompass(const int16_t* raw_compass_XYZ, float* compass_XYZ);

int mpu9255_init(wires_handle_t* handles);		//initialisation of the I2C wire


#endif /* MPU9255_H_ */
