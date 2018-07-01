#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <stm32f4xx_hal.h>
#include <stm32f4xx_hal_cortex.h>
#include <stm32f4xx_hal_i2c.h>
#include <stm32f4xx_hal_usart.h>
#include <stm32f4xx_hal_dma.h>
#include <stm32f4xx_hal_gpio.h>

#include <FreeRTOS.h>
#include "task.h"

#include "diag/Trace.h"

#include "state.h"
#include "kinematic_unit.h"
#include "dynamic_unit.h"
#include "gps_nmea.h"
#include "MPU9255.h"
#include "UNICS_bmp280.h"
#include "nRF24L01.h"
#include "telemetry.h"

// ----- Timing definitions -------------------------------------------------

// Keep the LED on for 2/3 of a second.
#define BLINK_ON_TICKS  (TIMER_FREQUENCY_HZ * 3 / 4)
#define BLINK_OFF_TICKS (TIMER_FREQUENCY_HZ - BLINK_ON_TICKS)

// ----- main() ---------------------------------------------------------------

// Sample pragmas to cope with warnings. Please note the related line at
// the end of this function, used to pop the compiler diagnostics status.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"


// глобальные структуры
stateSensors_raw_t 	stateSensors_raw;
stateGPS_t 			stateGPS;
stateIMU_rsc_t 		stateIMU_rsc;
stateIMU_isc_t 		stateIMU_isc;
stateSensors_t 		stateSensors;
stateCamera_orient_t stateCamera_orient;
state_system_t 		state_system;
state_zero_t		state_zero;

stateIMU_isc_t		stateIMU_isc_prev;
state_system_t		state_system_prev;
stateCamera_orient_t stateCamera_orient_prev;


//	параметры IO_RF_task
#define IO_RF_TASK_STACK_SIZE (30*configMINIMAL_STACK_SIZE)
static StackType_t	_iorfTaskStack[IO_RF_TASK_STACK_SIZE];
static StaticTask_t	_iorfTaskObj;


//	параметры GPS_task
#define GPS_TASK_STACK_SIZE (50*configMINIMAL_STACK_SIZE)
static StackType_t _gpsTaskStack[GPS_TASK_STACK_SIZE];
static StaticTask_t _gpsTaskObj;

//	параметры IMU_task
#define IMU_TASK_STACK_SIZE (30*configMINIMAL_STACK_SIZE)
static StackType_t	_IMUTaskStack[IMU_TASK_STACK_SIZE];
static StaticTask_t	_IMUTaskObj;

//	параметры MOTORS_task
#define MOTORS_TASK_STACK_SIZE (20*configMINIMAL_STACK_SIZE)
static StackType_t	_MOTORSTaskStack[MOTORS_TASK_STACK_SIZE];
static StaticTask_t	_MOTORSTaskObj;


int main(int argc, char* argv[])
{
	// Инициализация структур глобального состояния (в нашем случае просто заполняем их нулями)
	memset(&stateSensors_raw, 	0x00, sizeof(stateSensors_raw));
	memset(&stateGPS, 			0x00, sizeof(stateGPS));
	memset(&stateIMU_rsc, 		0x00, sizeof(stateIMU_rsc));
	memset(&stateIMU_isc, 		0x00, sizeof(stateIMU_isc));
	memset(&stateSensors, 		0x00, sizeof(stateSensors));
	memset(&stateCamera_orient, 0x00, sizeof(stateCamera_orient));
	memset(&state_system, 		0x00, sizeof(state_system));
	memset(&state_zero, 		0x00, sizeof(state_zero));

	memset(&stateIMU_isc_prev, 			0x00, sizeof(stateIMU_isc_prev));
	memset(&state_system_prev, 			0x00, sizeof(state_system_prev));
	memset(&stateCamera_orient_prev, 	0x00, sizeof(stateCamera_orient_prev));

	state_system.BMP_state = -1;
	state_system.GPS_state = -1;
	state_system.MOTOR_state = -1;
	state_system.MPU_state = -1;
	state_system.NRF_state = -1;
	state_system.SD_state = -1;


//	TaskHandle_t GPS_task_handle = xTaskCreateStatic(
//			GPS_task, "GPS", GPS_TASK_STACK_SIZE, NULL, 2, _gpsTaskStack, &_gpsTaskObj
//	);

	TaskHandle_t IMU_task_handle = xTaskCreateStatic(
				IMU_task, "IMU", IMU_TASK_STACK_SIZE, NULL, 2, _IMUTaskStack, &_IMUTaskObj
	);


	TaskHandle_t IO_RF_task_handle = xTaskCreateStatic(
				IO_RF_task, "IO_RF", IO_RF_TASK_STACK_SIZE,	NULL, 2, _iorfTaskStack, &_iorfTaskObj
	);

//	TaskHandle_t MOTORS_task_handle = xTaskCreateStatic(
//			MOTORS_task, "MOTORS", MOTORS_TASK_STACK_SIZE, NULL, 2, _MOTORSTaskStack, &_MOTORSTaskObj
//	);

	vTaskStartScheduler();

	return 0;
}

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
