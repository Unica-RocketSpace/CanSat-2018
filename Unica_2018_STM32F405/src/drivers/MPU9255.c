/*
 * MPU9255.c
 *
 *  Created on: 21 янв. 2017 г.
 *      Author: developer
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include <stm32f4xx_hal.h>
#include "diag/Trace.h"

#include <sofa.h>

#include "FreeRTOS.h"
#include "task.h"

#include "kinematic_unit.h"
#include "MPU9255.h"


//I2C_HandleTypeDef* i2c_mpu9255 = NULL;


int mpu9255_readRegister(mpu9255_address_t address, uint8_t reg_address, uint8_t* dataRead, uint8_t count)
{
	return HAL_I2C_Mem_Read(&i2c_mpu9255, address, reg_address, I2C_MEMADD_SIZE_8BIT, dataRead, count, 20000);
}

int mpu9255_writeRegister(mpu9255_address_t address, uint8_t reg_address, uint8_t dataWrite)
{
	int error = 0;
	uint8_t regData = 0x00;
	PROCESS_ERROR(mpu9255_readRegister(address, reg_address, &regData, 1));
	uint8_t regData_new = (regData | dataWrite);
	return HAL_I2C_Mem_Write(&i2c_mpu9255, address, reg_address, I2C_MEMADD_SIZE_8BIT, &regData_new, 1, 20000);

end:
	return error;
}

int mpu9255_init(I2C_HandleTypeDef* hi2c)
{
	int error = 0;

	hi2c->Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	hi2c->Init.ClockSpeed = 50000;
	hi2c->Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	hi2c->Init.DutyCycle = I2C_DUTYCYCLE_2;
	hi2c->Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	hi2c->Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;

	//	TODO: УСТАНОВИТЬ РЕАЛЬНЫЙ АДРЕС
	hi2c->Init.OwnAddress1 = 0x00;
//	hi2c->Init.OwnAddress2 = GYRO_AND_ACCEL;

	hi2c->Instance = I2C1;
	hi2c->Mode = HAL_I2C_MODE_MASTER;

	PROCESS_ERROR(HAL_I2C_Init(hi2c));

	PROCESS_ERROR(mpu9255_writeRegister(GYRO_AND_ACCEL,	107,	0b10000000));	//RESET
	vTaskDelay(100/portTICK_RATE_MS);

	PROCESS_ERROR(mpu9255_writeRegister(GYRO_AND_ACCEL,	25,		0b00000001));	//Sample Rate Divider
	PROCESS_ERROR(mpu9255_writeRegister(GYRO_AND_ACCEL,	26,		0b00000101));	//config (DLPF = 101)
	PROCESS_ERROR(mpu9255_writeRegister(GYRO_AND_ACCEL,	28,		(0b00000000 | (ACCEL_RANGE << 3)))); 	//accel config (rate 4g = 01)
	PROCESS_ERROR(mpu9255_writeRegister(GYRO_AND_ACCEL,	29,		0b00000100));	//accel config 2 (Fch_b = 0, DLPF = 100)
	PROCESS_ERROR(mpu9255_writeRegister(GYRO_AND_ACCEL,	35,		0b00000000));	//FIFO enable (not enabled)
	PROCESS_ERROR(mpu9255_writeRegister(GYRO_AND_ACCEL,	56,		0b00000000));	//interrupt enable (int disable = 0)
	PROCESS_ERROR(mpu9255_writeRegister(GYRO_AND_ACCEL,	106,	0b00000000));	//user control
	PROCESS_ERROR(mpu9255_writeRegister(GYRO_AND_ACCEL,	107,	0b00000001));	//power managment 1
	PROCESS_ERROR(mpu9255_writeRegister(GYRO_AND_ACCEL,	108,	0b00000000));	//power managment 2
	PROCESS_ERROR(mpu9255_writeRegister(GYRO_AND_ACCEL,	27,		(0b00000000 | (GYRO_RANGE << 4)) ));	//gyro config (rate 500dps = 01, Fch_b = 00)

	/*Установка OFFSET-ов*/
	uint8_t x_offset_l, x_offset_h, y_offset_l, y_offset_h, z_offset_l, z_offset_h;
	int16_t x_offset, y_offset, z_offset;
	PROCESS_ERROR(mpu9255_readRegister(GYRO_AND_ACCEL, 120, &x_offset_l, 1));
	PROCESS_ERROR(mpu9255_readRegister(GYRO_AND_ACCEL, 119, &x_offset_h, 1));
	x_offset = (x_offset_h << 7) + (x_offset_l >> 1) - 17;
	PROCESS_ERROR(mpu9255_writeRegister(GYRO_AND_ACCEL,	120,	(uint8_t)(x_offset << 1)));
	PROCESS_ERROR(mpu9255_writeRegister(GYRO_AND_ACCEL,	119,	(uint8_t)(x_offset >> 7)));


	PROCESS_ERROR(mpu9255_readRegister(GYRO_AND_ACCEL, 123, &y_offset_l, 1));
	PROCESS_ERROR(mpu9255_readRegister(GYRO_AND_ACCEL, 122, &y_offset_h, 1));
	y_offset = (y_offset_h << 7) + (y_offset_l >> 1) - 16;
	PROCESS_ERROR(mpu9255_writeRegister(GYRO_AND_ACCEL,	123,	(uint8_t)(y_offset << 1)));
	PROCESS_ERROR(mpu9255_writeRegister(GYRO_AND_ACCEL,	122,	(uint8_t)(y_offset >> 7)));


	PROCESS_ERROR(mpu9255_readRegister(GYRO_AND_ACCEL, 126, &z_offset_l, 1));
	PROCESS_ERROR(mpu9255_readRegister(GYRO_AND_ACCEL, 125, &z_offset_h, 1));
	z_offset = (z_offset_h << 7) + (z_offset_l >> 1) - 20;
	PROCESS_ERROR(mpu9255_writeRegister(GYRO_AND_ACCEL,	126,	(uint8_t)(z_offset << 1)));
	PROCESS_ERROR(mpu9255_writeRegister(GYRO_AND_ACCEL,	125,	(uint8_t)(z_offset >> 7)));

	/*mpu9255_writeRegister(GYRO_AND_ACCEL,	123,	(uint8_t)(Y_ACCEL_OFFSET << 1));
	mpu9255_writeRegister(GYRO_AND_ACCEL,	122,	(uint8_t)(Y_ACCEL_OFFSET << 7));
	mpu9255_writeRegister(GYRO_AND_ACCEL,	126,	(uint8_t)(Z_ACCEL_OFFSET << 1));
	mpu9255_writeRegister(GYRO_AND_ACCEL,	125,	(uint8_t)(Z_ACCEL_OFFSET << 7));*/

	PROCESS_ERROR(mpu9255_writeRegister(GYRO_AND_ACCEL,	55,		0b00000010));	//режим bypass on

	//compass init
	PROCESS_ERROR(mpu9255_writeRegister(COMPASS,		  	0x0A,	0b00010110));	//control 1

	//TODO:	bmp280 init

	PROCESS_ERROR(mpu9255_writeRegister(GYRO_AND_ACCEL,	55,		0b00000000));	//режим bypass off

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

int mpu9255_readIMU(int16_t * raw_accelData, int16_t * raw_gyroData)
{
	int error = 0;

	PROCESS_ERROR(mpu9255_readRegister(GYRO_AND_ACCEL, 59, (uint8_t*)raw_accelData, 6));	//чтение данных с акселерометра
	PROCESS_ERROR(mpu9255_readRegister(GYRO_AND_ACCEL, 67, (uint8_t*)raw_gyroData, 6));		//чтение данных с гироскопа

	for (int i = 0; i < 3; i++)
		raw_accelData[i] = _swapBytesI16(raw_accelData[i]);

	for (int i = 0; i < 3; i++)
		raw_gyroData[i] = _swapBytesI16(raw_gyroData[i]);

end:
	return error;
}

int mpu9255_readCompass(int16_t * raw_compassData)
{
	int error = 0;

	// state of magn (ready to give data or not)
	uint8_t magn_state;

	PROCESS_ERROR(mpu9255_writeRegister(GYRO_AND_ACCEL, 55, 0b00000010));	//режим bypass on
	mpu9255_readRegister(COMPASS, 0x02, &magn_state, 1);
	//printf("read_dummy_error = %d\n", read_dummy_error);
	PROCESS_ERROR(mpu9255_readRegister(COMPASS, 0x02, &magn_state, 1));


	if ((magn_state && 0x01) != 1)
	{
		state_system.state &= ~(1 << 1);		//магнитометр не готов
		PROCESS_ERROR(mpu9255_writeRegister(GYRO_AND_ACCEL, 55, 0b00000000));	//режим bypass off
		goto end;
	}

	state_system.state |= (1 << 1);	//магнитометр готов
	PROCESS_ERROR(mpu9255_readRegister(COMPASS, 0x03, (uint8_t*)raw_compassData, 6));
	PROCESS_ERROR(mpu9255_readRegister(COMPASS, 0x09, &magn_state, 1));
	PROCESS_ERROR(mpu9255_writeRegister(GYRO_AND_ACCEL, 55, 0b00000000));	//режим bypass off

	//FIXME: не надо ли свапать здесь байты???

end:
	return error;
}

void mpu9255_recalcAccel(const int16_t * raw_accelData, float * accelData)
{
	accelData[0] = (float)(raw_accelData[0]) * MPU9255_ACCEL_SCALE_FACTOR * pow(2, ACCEL_RANGE) * X_ACCEL_KOEFF;
	accelData[1] = (float)(raw_accelData[1]) * MPU9255_ACCEL_SCALE_FACTOR * pow(2, ACCEL_RANGE) * Y_ACCEL_KOEFF;
	accelData[2] = (float)(raw_accelData[2]) * MPU9255_ACCEL_SCALE_FACTOR * pow(2, ACCEL_RANGE) * Z_ACCEL_KOEFF;
}

void mpu9255_recalcGyro(const int16_t * raw_gyroData, float * gyroData)
{
	for (int i = 0; i < 3; i++)
		gyroData[i] = (float)(raw_gyroData[i]) * MPU9255_GYRO_SCALE_FACTOR * pow(2, GYRO_RANGE) * Z_GYRO_KOEFF;
}

void mpu9255_recalcCompass(const int16_t * raw_compassData, float * compassData)
{

	float raw_data[3] = {(float)raw_compassData[0], (float)raw_compassData[1], (float)raw_compassData[2]};
	float offset_vector[3] = {X_COMPAS_OFFSET, Y_COMPAS_OFFSET, Z_COMPAS_OFFSET};
	float transform_matrix[3][3] =	{	{XX_COMPAS_TRANSFORM_MATIX, XY_COMPAS_TRANSFORM_MATIX, XZ_COMPAS_TRANSFORM_MATIX},
										{XY_COMPAS_TRANSFORM_MATIX, YY_COMPAS_TRANSFORM_MATIX, YZ_COMPAS_TRANSFORM_MATIX},
										{XZ_COMPAS_TRANSFORM_MATIX, YZ_COMPAS_TRANSFORM_MATIX, ZZ_COMPAS_TRANSFORM_MATIX}};

	//printf("raw_compass_XYZ = %d, %d, %d\n", raw_compass_XYZ[0], raw_compass_XYZ[1], raw_compass_XYZ[2]);
	//printf("raw_compass_XYZ = %f, %f, %f\n", (float)raw_compass_XYZ[0], (float)raw_compass_XYZ[1], (float)raw_compass_XYZ[2]);
	iauPmp(raw_data, offset_vector, compassData);
	//printf("offset_compass_XYZ = %f, %f, %f\n", compass_XYZ[0], compass_XYZ[1], compass_XYZ[2]);
	iauRxp(transform_matrix, compassData, compassData);
	//printf("transform_compass_XYZ = %f, %f, %f\n\n", compass_XYZ[0], compass_XYZ[1], compass_XYZ[2]);
}


static int IMU_updateDataAll() {
	//	массивы для хранения опросов
	int error = 0;
	int16_t accelData[3] = {0, 0, 0};
	int16_t gyroData[3] = {0, 0, 0};
	int16_t compassData[3] = {0, 0, 0};
	float accel[3] = {0, 0, 0}; float gyro[3] = {0, 0, 0}; float compass[3] = {0, 0, 0};

	//	собираем данные
	PROCESS_ERROR(mpu9255_readIMU(accelData, gyroData));
	PROCESS_ERROR(mpu9255_readCompass(compassData));
	mpu9255_recalcAccel(accelData, accel);
	mpu9255_recalcGyro(gyroData, gyro);
	mpu9255_recalcCompass(compassData, compass);

taskENTER_CRITICAL();
	//	пересчитываем их и записываем в структуры
	for (int k = 0; k < 3; k++) {
		stateIMU_rsc.accel[k] = accel[k];
		stateIMU_rsc.gyro[k] = gyro[k];
		stateIMU_rsc.compass[k] = compass[k];
	}

	//	обновляем ориентацию (предварительно запросив время)
	state_system.time = HAL_GetTick()/1000;
	constructTrajectory(
			&stateIMU_isc, &stateIMU_isc_prev,
			&state_system, &state_system_prev, &stateIMU_rsc);
taskEXIT_CRITICAL();

end:
	return error;
}

void IMU_task() {

	//	usart_dbg init
	__GPIOC_CLK_ENABLE();
	GPIO_InitTypeDef gpioc;
	gpioc.Mode = GPIO_MODE_OUTPUT_OD;
	gpioc.Pin = GPIO_PIN_12;
	gpioc.Pull = GPIO_NOPULL;
	gpioc.Speed = GPIO_SPEED_FREQ_LOW;

	HAL_GPIO_Init(GPIOC, &gpioc);

	usart_dbg.Init.BaudRate = 256000;
	usart_dbg.Init.WordLength = UART_WORDLENGTH_8B;
	usart_dbg.Init.StopBits = UART_STOPBITS_1;
	usart_dbg.Init.Parity = UART_PARITY_NONE;
	usart_dbg.Init.Mode = UART_MODE_TX_RX;

	usart_dbg.Instance = USART3;

	HAL_USART_Init(&usart_dbg);

	/*for (;;) {
		// Этап 0. Подтверждение инициализации отправкой пакета состояния и ожидание ответа от НС
		if (state_system.globalStage == 0) {
			mpu9255_init(&i2c_mpu9255);
			//TODO: ГДЕ ОБНОВЛЯТЬ СОСТОЯНИЕ ДАТЧИКОВ
		}
//		// Этап 1. Погрузка в ракету
//		if (state_system.globalStage == 1) {		//НИЧЕГО НЕ ДЕЛАЕМ
//		}
		// Этап 2. Определение начального состояния
		if (state_system.globalStage == 2) {

			uint8_t zero_orientCnt = 10;
			//	массив для хранения статического смещения
			float gyro_staticShift[3] = {0, 0, 0};

			//	находим статическое смещение гироскопа
			for (int i = 0; i < zero_orientCnt; i++) {
				int16_t accelData[3] = {0, 0, 0};
				int16_t gyroData[3] = {0, 0, 0};

				float accel[3]; float gyro[3];

				//	собираем данные
				mpu9255_readIMU(accelData, gyroData);
				mpu9255_recalcGyro(gyroData, gyro);

				for (int m = 0; m < 3; m++) {
					gyro_staticShift[i] += gyroData[i];
				}
			}

			taskENTER_CRITICAL();
			//	записываем смещение в state
			for (int m = 0; m < 3; m++) {
				state_zero.gyro_staticShift[m] = gyro_staticShift[m]/zero_orientCnt;
			}
			taskEXIT_CRITICAL();

			for (int i = 0; i < zero_orientCnt; i++) {
				IMU_updateDataAll();
			}
		}
		// Этап 3. Полет в ракете
		if (state_system.globalStage == 3) {

		}
		// Этап 4. Свободное падение
		if (state_system.globalStage == 4) {

		}
		// Этап 5. Спуск
		if (state_system.globalStage == 5) {

		}
		// Этап 6. Окончание полета
		if (state_system.globalStage == 6) {

		}



	}*/

//	int mpu9255init_error = mpu9255_init(&i2c_mpu9255);
//	printf("mpu_error = %d\n", mpu9255init_error);

	for (;;) {
//		int error = IMU_updateDataAll();
//		printf("IMU_error = %d\n", error);
//		const TickType_t _delay = 10 / portTICK_RATE_MS;
//		vTaskDelay(_delay);
//		int16_t accelData[3] = {0, 0, 0};
//		int16_t gyroData[3] = {0, 0, 0};
//		float accel[3] = {0, 0, 0};
//
//		uint8_t error =	mpu9255_readIMU(accelData, gyroData);
//		mpu9255_recalcAccel(accelData, accel);
//		taskENTER_CRITICAL();
//		for (int i = 0; i < 3; i++) {
//			stateIMU_rsc.accel[i] = accel[i];
//		}
//		taskEXIT_CRITICAL();
//
//
//		taskENTER_CRITICAL();
//
////		char *msg = "dfglkj";
////		uint8_t VT = 0b00001011;
////		HAL_USART_Transmit(&usart_dbg, (uint8_t*)msg, strlen(msg), 2000);
////		HAL_USART_Transmit(&usart_dbg, &VT, 1, 2000);
////		HAL_USART_Transmit(&usart_dbg, &VT_H, 1, 2000);
////		HAL_USART_Transmit(&usart_dbg, &((uint8_t)dummy), sizeof(dummy), 2000);
////		HAL_USART_Transmit(&usart_dbg, (uint8_t*)accelData, sizeof(accelData), 2000);
//
////		trace_printf("%d\n", error);
////		trace_printf("%X, %X, %X\n", accelData[0], accelData[1], accelData[2]);
//
//		trace_printf("%f, %f, %f\n", stateIMU_rsc.accel[0], stateIMU_rsc.accel[1], stateIMU_rsc.accel[2]);
//
//
//		taskEXIT_CRITICAL();
//		taskENTER_CRITICAL();
//		printf("%d\n", error);
//		printf("%f, %f, %f\n", stateIMU_isc.accel[0], stateIMU_isc.accel[1], stateIMU_isc.accel[2]);
//		printf("%f, %f, %f\n", stateIMU_isc.gyro[0], stateIMU_isc.gyro[1], stateIMU_isc.gyro[2]);
//		printf("%f, %f, %f\n", stateIMU_isc.compass[0], stateIMU_isc.compass[1], stateIMU_isc.compass[2]);
//		printf("\n");
//		taskEXIT_CRITICAL();
//		const TickType_t _delay = 100 / portTICK_RATE_MS;
//		vTaskDelay(_delay);
//		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_12, RESET);
//		vTaskDelay(_delay);
//		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_12, SET);

	}
}


//
//int mpu9255_readBMP280(I2C_HandleTypeDef* uint16_t* raw_pressure, uint16_t* raw_temp) {
//	int error = 0;
//
//	//PROCESS_ERROR()
//
//end:
//	return error;
//}

