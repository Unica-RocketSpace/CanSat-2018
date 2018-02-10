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


/*#################################################*/
/*################## СТРУКТУРЫ ###################*/
/*#################################################*/

/*
 * 	Структуры состояний аппарата
 */

//	raw IMU data
typedef struct {
	int16_t accel[3];
	int16_t gyro[3];
	int16_t compass[3];
} stateIMU_raw_t;


typedef struct {
	//	temperature and pressure
	int16_t temp;
	int16_t pressure;
} stateSensors_raw_t;


typedef struct {
	//GPS data
	char * GPS;
	float coord_GPS[3];
} stateGPS_t;


//	IMU data in RSC (related system of coordinates)
typedef struct {
	float accel[3];
	float gyro[3];
	float compass[3];
} stateIMU_rsc_t;


//	orientation and position of device in ISC (inertial system of coordinates)
typedef struct {
	//	IMU data
	float accel[3];
	float gyro[3];
	float compass[3];

	//	position
	float velocities[3];
	float coord_IMU[3];

	//	orientation
	float quaternion[4];

} stateIMU_isc_t;


//	transformed temperature and pressure
typedef struct {
	float temp;
	float pressure;
	float height;
} stateSensors_t;


//	system parameters
typedef struct {
	//	"position" of servo and step engine
	float servo_pos;
	float step_engine_pos;

	//	zero params; this fields should be filled when device started it`s work
	float zero_pressure;
	float zero_quaternion[4];
	float zero_GPS[3];

	uint16_t state;
	float time;
} state_system_t;


typedef struct {


} ground_state_t;



typedef enum {
	no_error		= 0,
	driver_overheat	= -1

} error;


/*##################################################*/
/*################### ПЕРЕМЕННЫЕ ###################*/
/*##################################################*/

extern I2C_HandleTypeDef 	i2c_mpu9255;
extern USART_HandleTypeDef 	usart_GPS;
extern DMA_HandleTypeDef 	dma_GPS;

// глобальные структуры
extern stateIMU_raw_t 		stateIMU_raw;
extern stateSensors_raw_t 	stateSensors_raw;
extern stateGPS_t 			stateGPS;
extern stateIMU_rsc_t 		stateIMU_rsc;
extern stateIMU_isc_t 		stateIMU_isc;
extern stateSensors_t 		stateSensors;
extern state_system_t 		state_system;

extern stateIMU_isc_t		stateIMU_isc_prev;
extern state_system_t		state_system_prev;


#endif /* STATE_H_ */
