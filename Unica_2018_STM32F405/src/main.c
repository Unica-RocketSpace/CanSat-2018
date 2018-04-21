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
#include "gps_nmea.h"
#include "MPU9255.h"
#include "nRF24L01.h"
#include "telemetry.h"
#include "dynamic_unit.h"

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


I2C_HandleTypeDef i2c_mpu9255;
USART_HandleTypeDef usart_GPS;
USART_HandleTypeDef usart_dbg;
DMA_HandleTypeDef dma_GPS;
SPI_HandleTypeDef	spi_nRF24L01;
USART_HandleTypeDef usart_motor;

// глобальные структуры
stateIMU_raw_t 		stateIMU_raw;
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

stateTasks_flags_t		stateTasks_flags;


//	параметры IO_RF_task
#define IO_RF_TASK_STACK_SIZE 1024
static StackType_t	_iorfTaskStack[IO_RF_TASK_STACK_SIZE];
static StaticTask_t	_iorfTaskObj;


//	параметры GPS_task
#define GPS_TASK_STACK_SIZE 1024
static StackType_t _gpsTaskStack[GPS_TASK_STACK_SIZE];
static StaticTask_t _gpsTaskObj;

//	параметры IMU_task
#define IMU_TASK_STACK_SIZE 2048
static StackType_t	_IMUTaskStack[IMU_TASK_STACK_SIZE];
static StaticTask_t	_IMUTaskObj;


int main(int argc, char* argv[])
{
	// Инициализация структур глобального состояния (в нашем случае просто заполняем их нулями)
	memset(&stateIMU_raw, 		0x00, sizeof(stateIMU_raw));
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

	memset(&stateTasks_flags,	0x00, sizeof(stateTasks_flags));


//	TaskHandle_t IO_RF_task_handle = xTaskCreateStatic(
//			IO_RF_task,
//			"IO_RF",
//			IO_RF_TASK_STACK_SIZE,
//			NULL,
//			1,
//			_iorfTaskStack,
//			&_iorfTaskObj
//	);
//
//
//	TaskHandle_t GPS_task_handle = xTaskCreateStatic(
//			GPS_task, 			// функция
//			"GPS",				// имя
//			GPS_TASK_STACK_SIZE,// глубина стека
//			NULL,				// аргумент
//			1,					// приоритет
//			_gpsTaskStack,		// стек
//			&_gpsTaskObj		// объект задания
//	);
//
//
//	TaskHandle_t IMU_task_handle = xTaskCreateStatic(
//			IMU_task, 			// функция
//			"IMU",				// имя
//			IMU_TASK_STACK_SIZE,// глубина стека
//			NULL,				// аргумент
//			1,					// приоритет
//			_IMUTaskStack,		// стек
//			&_IMUTaskObj		// объект задания
//	);

	#define MOTOR_TASK_STACK_SIZE 2048
	static StackType_t	_motorTaskStack[MOTOR_TASK_STACK_SIZE];
	static StaticTask_t	_motorTaskObj;

	xTaskCreateStatic(MOTOR_task, "dummy", MOTOR_TASK_STACK_SIZE, NULL, 1,
						_motorTaskStack, &_motorTaskObj);


	vTaskStartScheduler();

	return 0;
}

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
