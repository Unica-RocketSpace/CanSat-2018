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
#include <stdbool.h>

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
	float coordinates[3];
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
	float coordinates[3];

	//	orientation
	float quaternion[4];

} stateIMU_isc_t;


//	transformed temperature and pressure
typedef struct {
	float temp;
	float pressure;
	float height;
} stateSensors_t;


typedef struct {
	//	"position" of servo and step engine
	float servo_pos;
	float step_engine_pos;
} stateCamera_orient_t;

//	system parameters
typedef struct {
	uint8_t accel_state;	//	state of accelerometer
	uint8_t gyro_state;		//	state of gyroscope
	uint8_t compass_state;	//	state of magnetometer
	uint8_t baro_state;		//	state of barometer

	uint8_t GPS_state;		//	state of GPS-module

	uint8_t RPI_state;		//	state of RaspRi

	uint8_t globalStage;	//	number of current global stage

	uint16_t state;			//	special field (1st bit for saving compass' prepareness)
	float time;				//	current time
} state_system_t;


typedef struct {
	//	zero params; this fields should be filled when device started it`s work
	float zero_pressure;
	float zero_quaternion[4];
	float zero_GPS[3];
	float gyro_staticShift[3];
} state_zero_t;


//	struct for keeping flags for switching FreeRTOS tasks
typedef struct {
	uint8_t IO_RF_flag;
	bool zeroOrient_isSet;		//	zero orient is set
	uint8_t GPS_flag;
	uint8_t RPI_IO_flag;

} stateTasks_flags_t;


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
extern stateCamera_orient_t	stateCamera_orient;
extern stateCamera_orient_t	stateCamera_orient_prev;
extern state_system_t 		state_system;
extern state_zero_t			state_zero;

extern stateIMU_isc_t		stateIMU_isc_prev;
extern state_system_t		state_system_prev;

extern stateTasks_flags_t	stateTasks_flags;

#endif /* STATE_H_ */
