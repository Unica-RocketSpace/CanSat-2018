/*
 * state.h
 *
 *  Created on: 11 нояб. 2017 г.
 *      Author: Korr237i
 *
 *  This header included main structure to share data from module to module
 */

#ifndef STATE_H_
#define STATE_H_

#include <stdint.h>

#include "stm32f4xx_hal.h"

#include "stm32f4xx_hal_rcc.h"
#include "stm32f4xx_hal_i2c.h"
#include "stm32f4xx_hal_usart.h"
#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_hal_dma.h"

// if error set value and go to end
#define PROCESS_ERROR(x) if (0 != (error = (x))) { goto end; }

typedef struct {

	I2C_HandleTypeDef* i2c;
	USART_HandleTypeDef* usart;
	DMA_HandleTypeDef* dma;

} wires_handle_t;


//TODO: ДОБАВИТЬ GLOBAL, ERRORS


typedef struct {
	//	raw data from sensors
	struct {
		//	IMU data
		int16_t accel[3];
		int16_t gyro[3];
		int16_t compass[3];

		//GPS data
		char * GPS;

		//	temperature and pressure
		int16_t temp;
		int16_t pressure;
	} raw;

	//	IMU data in RSC (related system of coordinates)
	struct {
		float accel[3];
		float gyro[3];
		float compass[3];
	} rsc;

	//	orientation and position of device in ISC (inertial system of coordinates)
	struct {
		//	IMU data
		float accel[3];
		float gyro[3];
		float compass[3];

		//	position
		float velocities[3];
		float coord_IMU[3];
		float coord_GPS[3];

		//	orientation
		float quaternion[4];

	} isc;

	//	transformed temperature and pressure
	struct {
		float temp;
		float pressure;
	} sensors;

	//	system parameters
	struct {
		//	"position" of servo and step engine
		float servo_pos;
		float step_engine_pos;

		uint16_t state;
		float time;
	} system;

} state_t;

struct global_t {

	float target[3] = {100, 100, 0};
	float zero_pressure = 100000;

} global;

typedef enum {
	no_error		= 0,
	driver_overheat	= -1

} error;



#endif /* STATE_H_ */
