/*
 * MPU9255.c
 *
 *  Created on: 21 янв. 2017 г.
 *      Author: developer
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sofa.h>
#include <stm32f4xx_hal_i2c.h>

#include "kinematic_unit.h"
#include "MPU9255.h"


int mpu9255_readRegister(wires_handle_t* handles, mpu9255_address_t address, uint8_t reg_address, uint8_t* dataRead, uint8_t count)
{
	return HAL_I2C_Mem_Read(handles->i2c, address, reg_address, I2C_MEMADD_SIZE_8BIT, dataRead, count, 2000);
}


int mpu9255_writeRegister(wires_handle_t* handles, mpu9255_address_t address, uint8_t reg_address, uint8_t dataWrite)
{
	int error = 0;
	uint8_t regData = 0x00;
	PROCESS_ERROR(mpu9255_readRegister(handles, address, reg_address, &regData, 1));
	uint8_t regData_new = (regData | dataWrite);
	return HAL_I2C_Mem_Write(handles->i2c, address, reg_address, I2C_MEMADD_SIZE_8BIT, &regData_new, 1, 2000);

end:
	return error;
}


int mpu9255_init(wires_handle_t* handles)
{
	int error = 0;

	PROCESS_ERROR(mpu9255_writeRegister(handles, GYRO_AND_ACCEL,	107,	0b10000000));	//RESET

	PROCESS_ERROR(mpu9255_writeRegister(handles, GYRO_AND_ACCEL,	25,		0b00000001));	//Sample Rate Divider
	PROCESS_ERROR(mpu9255_writeRegister(handles, GYRO_AND_ACCEL,	26,		0b00000101));	//config (DLPF = 101)
	PROCESS_ERROR(mpu9255_writeRegister(handles, GYRO_AND_ACCEL,	28,		(0b00000000 | (ACCEL_RANGE << 3)))); 	//accel config (rate 4g = 01)
	PROCESS_ERROR(mpu9255_writeRegister(handles, GYRO_AND_ACCEL,	29,		0b00000100));	//accel config 2 (Fch_b = 0, DLPF = 100)
	PROCESS_ERROR(mpu9255_writeRegister(handles, GYRO_AND_ACCEL,	35,		0b00000000));	//FIFO enable (not enabled)
	PROCESS_ERROR(mpu9255_writeRegister(handles, GYRO_AND_ACCEL,	56,		0b00000000));	//interrupt enable (int disable = 0)
	PROCESS_ERROR(mpu9255_writeRegister(handles, GYRO_AND_ACCEL,	106,	0b00000000));	//user control
	PROCESS_ERROR(mpu9255_writeRegister(handles, GYRO_AND_ACCEL,	107,	0b00000001));	//power managment 1
	PROCESS_ERROR(mpu9255_writeRegister(handles, GYRO_AND_ACCEL,	108,	0b00000000));	//power managment 2
	PROCESS_ERROR(mpu9255_writeRegister(handles, GYRO_AND_ACCEL,	27,		(0b00000000 | (GYRO_RANGE << 4)) ));	//gyro config (rate 500dps = 01, Fch_b = 00)

	/*Установка OFFSET-ов*/
	uint8_t x_offset_l, x_offset_h, y_offset_l, y_offset_h, z_offset_l, z_offset_h;
	int16_t x_offset, y_offset, z_offset;
	PROCESS_ERROR(mpu9255_readRegister(handles, GYRO_AND_ACCEL, 120, &x_offset_l, 1));
	PROCESS_ERROR(mpu9255_readRegister(handles, GYRO_AND_ACCEL, 119, &x_offset_h, 1));
	x_offset = (x_offset_h << 7) + (x_offset_l >> 1) - 17;
	PROCESS_ERROR(mpu9255_writeRegister(handles, GYRO_AND_ACCEL,	120,	(uint8_t)(x_offset << 1)));
	PROCESS_ERROR(mpu9255_writeRegister(handles, GYRO_AND_ACCEL,	119,	(uint8_t)(x_offset >> 7)));


	PROCESS_ERROR(mpu9255_readRegister(handles, GYRO_AND_ACCEL, 123, &y_offset_l, 1));
	PROCESS_ERROR(mpu9255_readRegister(handles, GYRO_AND_ACCEL, 122, &y_offset_h, 1));
	y_offset = (y_offset_h << 7) + (y_offset_l >> 1) - 16;
	PROCESS_ERROR(mpu9255_writeRegister(handles, GYRO_AND_ACCEL,	123,	(uint8_t)(y_offset << 1)));
	PROCESS_ERROR(mpu9255_writeRegister(handles, GYRO_AND_ACCEL,	122,	(uint8_t)(y_offset >> 7)));


	PROCESS_ERROR(mpu9255_readRegister(handles, GYRO_AND_ACCEL, 126, &z_offset_l, 1));
	PROCESS_ERROR(mpu9255_readRegister(handles, GYRO_AND_ACCEL, 125, &z_offset_h, 1));
	z_offset = (z_offset_h << 7) + (z_offset_l >> 1) - 20;
	PROCESS_ERROR(mpu9255_writeRegister(handles, GYRO_AND_ACCEL,	126,	(uint8_t)(z_offset << 1)));
	PROCESS_ERROR(mpu9255_writeRegister(handles, GYRO_AND_ACCEL,	125,	(uint8_t)(z_offset >> 7)));

	/*mpu9255_writeRegister(GYRO_AND_ACCEL,	123,	(uint8_t)(Y_ACCEL_OFFSET << 1));
	mpu9255_writeRegister(GYRO_AND_ACCEL,	122,	(uint8_t)(Y_ACCEL_OFFSET << 7));
	mpu9255_writeRegister(GYRO_AND_ACCEL,	126,	(uint8_t)(Z_ACCEL_OFFSET << 1));
	mpu9255_writeRegister(GYRO_AND_ACCEL,	125,	(uint8_t)(Z_ACCEL_OFFSET << 7));*/

	PROCESS_ERROR(mpu9255_writeRegister(handles, GYRO_AND_ACCEL,	55,		0b00000010));	//режим bypass on

	//compass init
	PROCESS_ERROR(mpu9255_writeRegister(handles, COMPASS,		  	0x0A,	0b00010110));	//control 1

	/*//bmp180 init
	mpu9255_writeRegister(handles, BMP180, 			0xF4, 	0b10000000);	//oversampling*/

	//	bmp280 init



	PROCESS_ERROR(mpu9255_writeRegister(handles, GYRO_AND_ACCEL,	55,		0b00000000));	//режим bypass off

end:
	return error;
}


static int16_t _swapBytesI16(int16_t value)
{
	uint8_t * value_ptr = (uint8_t*)&value;
	uint8_t tmp = value_ptr[0];
	value_ptr[0] = value_ptr[1];
	value_ptr[1] = tmp;

	return value;
}


int mpu9255_readIMU(wires_handle_t* handles, int16_t * raw_accel_XYZ, int16_t * raw_gyro_XYZ)
{
	int error = 0;

	PROCESS_ERROR(mpu9255_readRegister(handles, GYRO_AND_ACCEL, 59, (uint8_t*)raw_accel_XYZ, 6));	//чтение данных с акселерометра
	PROCESS_ERROR(mpu9255_readRegister(handles, GYRO_AND_ACCEL, 67, (uint8_t*)raw_gyro_XYZ, 6));	//чтение данных с гироскопа


	for (int i = 0; i < 3; i++)
		raw_accel_XYZ[i] = _swapBytesI16(raw_accel_XYZ[i]);

	for (int i = 0; i < 3; i++)
		raw_gyro_XYZ[i] = _swapBytesI16(raw_gyro_XYZ[i]);

end:
	return error;
}


int mpu9255_readCompass(state_t* state, wires_handle_t* handles, int16_t * raw_compass_XYZ)
{
	int error = 0;

	// state of magn (ready to give data or not)
	uint8_t magn_state;

	PROCESS_ERROR(mpu9255_writeRegister(handles, GYRO_AND_ACCEL, 55, 0b00000010));	//режим bypass on
	mpu9255_readRegister(handles, COMPASS, 0x02, &magn_state, 1);
	//printf("read_dummy_error = %d\n", read_dummy_error);
	PROCESS_ERROR(mpu9255_readRegister(handles, COMPASS, 0x02, &magn_state, 1));


	if ((magn_state && 0x01) != 1)
	{
		state->system.state &= ~(1 << 1);		//магнитометр не готов
		PROCESS_ERROR(mpu9255_writeRegister(handles, GYRO_AND_ACCEL, 55, 0b00000000));	//режим bypass off
		goto end;
	}

	state->system.state |= (1 << 1);	////магнитометр готов
	PROCESS_ERROR(mpu9255_readRegister(handles, COMPASS, 0x03, (uint8_t*)raw_compass_XYZ, 6));
	PROCESS_ERROR(mpu9255_readRegister(handles, COMPASS, 0x09, &magn_state, 1));
	PROCESS_ERROR(mpu9255_writeRegister(handles, GYRO_AND_ACCEL, 55, 0b00000000));	//режим bypass off

	//FIXME: не надо ли свапать здесь байты???

end:
	return error;
}


void mpu9255_recalcAccel(const int16_t * raw_accel_XYZ, float * accel_XYZ)
{
	accel_XYZ[0] = (float)(raw_accel_XYZ[0]) * MPU9255_ACCEL_SCALE_FACTOR * pow(2, ACCEL_RANGE) * X_ACCEL_KOEFF;
	accel_XYZ[1] = (float)(raw_accel_XYZ[1]) * MPU9255_ACCEL_SCALE_FACTOR * pow(2, ACCEL_RANGE) * Y_ACCEL_KOEFF;
	accel_XYZ[2] = (float)(raw_accel_XYZ[2]) * MPU9255_ACCEL_SCALE_FACTOR * pow(2, ACCEL_RANGE) * Z_ACCEL_KOEFF;
}


void mpu9255_recalcGyro(const int16_t * raw_gyro_XYZ, float * gyro_XYZ)
{
	for (int i = 0; i < 3; i++)
		gyro_XYZ[i] = (float)(raw_gyro_XYZ[i]) * MPU9255_GYRO_SCALE_FACTOR * pow(2, GYRO_RANGE) * Z_GYRO_KOEFF;
}


void mpu9255_recalcCompass(const int16_t * raw_compass_XYZ, float * compass_XYZ)
{

	float raw_data[3] = {(float)raw_compass_XYZ[0], (float)raw_compass_XYZ[1], (float)raw_compass_XYZ[2]};
	float offset_vector[3] = {X_COMPAS_OFFSET, Y_COMPAS_OFFSET, Z_COMPAS_OFFSET};
	float transform_matrix[3][3] =	{	{XX_COMPAS_TRANSFORM_MATIX, XY_COMPAS_TRANSFORM_MATIX, XZ_COMPAS_TRANSFORM_MATIX},
										{XY_COMPAS_TRANSFORM_MATIX, YY_COMPAS_TRANSFORM_MATIX, YZ_COMPAS_TRANSFORM_MATIX},
										{XZ_COMPAS_TRANSFORM_MATIX, YZ_COMPAS_TRANSFORM_MATIX, ZZ_COMPAS_TRANSFORM_MATIX}};

	//printf("raw_compass_XYZ = %d, %d, %d\n", raw_compass_XYZ[0], raw_compass_XYZ[1], raw_compass_XYZ[2]);
	//printf("raw_compass_XYZ = %f, %f, %f\n", (float)raw_compass_XYZ[0], (float)raw_compass_XYZ[1], (float)raw_compass_XYZ[2]);
	iauPmp(raw_data, offset_vector, compass_XYZ);
	//printf("offset_compass_XYZ = %f, %f, %f\n", compass_XYZ[0], compass_XYZ[1], compass_XYZ[2]);
	iauRxp(transform_matrix, compass_XYZ, compass_XYZ);
	//printf("transform_compass_XYZ = %f, %f, %f\n\n", compass_XYZ[0], compass_XYZ[1], compass_XYZ[2]);
}

//
//int mpu9255_readBMP280(wires_handle_t* handles, uint16_t* raw_pressure, uint16_t* raw_temp) {
//	int error = 0;
//
//	//PROCESS_ERROR()
//
//end:
//	return error;
//}
//
//
//
///*
// *
// * FUNCTIONS FOR USING BMP180 (doesn`t works now)
// *
// */
//int mpu9255_checkBMP180(wires_handle_t* handles, uint16_t* raw_data) {
//	int error = 0;
//	uint8_t flag = 0;
//	uint8_t MSB = 0, LSB = 0;
//
//	PROCESS_ERROR(mpu9255_writeRegister(handles, 	GYRO_AND_ACCEL, 55, 	0b00000010));	// bypass on
//	PROCESS_ERROR(mpu9255_readRegister(handles, 	BMP180, 		0xF4, 	&flag, 1));
//	if (~flag & (1 << 5)) {
//		PROCESS_ERROR(mpu9255_readRegister(handles, BMP180,			0xF6,	&MSB,	1));
//		PROCESS_ERROR(mpu9255_readRegister(handles, BMP180,			0xF7,	&LSB,	1));
//	}
//	PROCESS_ERROR(mpu9255_writeRegister(handles, 	GYRO_AND_ACCEL,	55,		0b00000000));	//bypass off
//
//end:
//	return error;
//}
//
//
//int mpu9255_readBMP180Temp(wires_handle_t* handles, int16_t* raw_pressure, int16_t* raw_temp) {
//
//	int error = 0;
//
//	PROCESS_ERROR(mpu9255_writeRegister(handles, GYRO_AND_ACCEL, 55, 0b00000010));	//bypass mode set on
//	//PROCESS_ERROR(mpu9255_readRegister(handles, BMP180, , (uint8_t*)raw_pressure, ));
//
//end:
//	return error;
//}






