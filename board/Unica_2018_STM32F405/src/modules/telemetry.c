/*
 * TELEMETRY_UNIT
 *
 * 		This unit will be used for transmitting and receiving data
 * 	with device and earth using radio transmitter.
 *
 * 	Authors: Korr237i, RaKetov
 */

#include "stdint.h"

#include "diag/Trace.h"
#include "FreeRTOS.h"
#include "task.h"
#include "mavlink/UNISAT/mavlink.h"

#include "state.h"
#include "telemetry.h"
#include "nRF24L01.h"
#include "drivers/sd/sd.h"
#include "drivers/sd/dump.h"

#include "mavlink/UNISAT/mavlink.h"

uint8_t UNISAT_ID = 0x01;
uint8_t UNISAT_NoComp = 0xFF;
uint8_t UNISAT_IMU = 0x02;
uint8_t UNISAT_SENSORS = 0x03;
uint8_t UNISAT_GPS = 0x04;
uint8_t UNISAT_RPI = 0x05;
uint8_t UNISAT_CAM = 0x06;

static dump_state_t stream_file;
static const char filename[] = "TM-";

// FIXME: ЗАМЕНИТЬ ВРЕМЯ В ПАКЕТЕ НА ВРЕМЯ ПОЛУЧЕНИЯ ДАННЫХ

static uint8_t mavlink_msg_state_send() {

	mavlink_state_t msg_state;
	msg_state.time = (float)HAL_GetTick() / 1000;
taskENTER_CRITICAL();
	msg_state.accel_state	= state_system.accel_state;
	msg_state.gyro_state	= state_system.gyro_state;
	msg_state.compass_state	= state_system.compass_state;
	msg_state.baro_state	= state_system.baro_state;
	msg_state.GPS_state		= state_system.GPS_state;
	msg_state.RPI_state		= state_system.RPI_state;
taskEXIT_CRITICAL();

	mavlink_message_t msg;
	uint16_t len = mavlink_msg_state_encode(UNISAT_ID, UNISAT_NoComp, &msg, &msg_state);
	uint8_t buffer[100];
	len = mavlink_msg_to_send_buffer(buffer, &msg);
	uint8_t error = nRF24L01_send(&spi_nRF24L01, buffer, len, 1);
	return error;
}

static uint8_t mavlink_msg_imu_rsc_send() {

	mavlink_imu_rsc_t msg_imu_rsc;
	msg_imu_rsc.time = (float)HAL_GetTick() / 1000;
taskENTER_CRITICAL();
	for (int i = 0; i < 3; i++) {
		msg_imu_rsc.accel[i] = stateIMU_rsc.accel[i];
		msg_imu_rsc.gyro[i] = stateIMU_rsc.gyro[i];
		msg_imu_rsc.compass[i] = stateIMU_rsc.compass[i];
	}
taskEXIT_CRITICAL();

	mavlink_message_t msg;
	uint16_t len = mavlink_msg_imu_rsc_encode(UNISAT_GPS, UNISAT_IMU, &msg, &msg_imu_rsc);
	uint8_t buffer[100];
	len = mavlink_msg_to_send_buffer(buffer, &msg);
	uint8_t error = nRF24L01_send(&spi_nRF24L01, buffer, len, 1);
	return error;
}

static uint8_t mavlink_msg_imu_isc_send() {

	mavlink_imu_isc_t msg_imu_isc;
	msg_imu_isc.time = (float)HAL_GetTick() / 1000;
taskENTER_CRITICAL();
	for (int i = 0; i < 3; i++) {
		msg_imu_isc.accel[i] = stateIMU_isc.accel[i];
		msg_imu_isc.gyro[i] = stateIMU_isc.gyro[i];
		msg_imu_isc.compass[i] = stateIMU_isc.compass[i];
		msg_imu_isc.velocities[i] = stateIMU_isc.velocities[i];
		msg_imu_isc.coordinates[i] = stateIMU_isc.coordinates[i];
	}
	for (int j = 0; j < 4; j++) {
		msg_imu_isc.quaternion[j] = stateIMU_isc.quaternion[j];
	}
taskEXIT_CRITICAL();

	mavlink_message_t msg;
	uint16_t len = mavlink_msg_imu_isc_encode(UNISAT_ID, UNISAT_IMU, &msg, &msg_imu_isc);
	uint8_t buffer[100];
	len = mavlink_msg_to_send_buffer(buffer, &msg);
	uint8_t error = nRF24L01_send(&spi_nRF24L01, buffer, len, 1);
	return error;
}

static uint8_t mavlink_msg_sensors_send() {

	mavlink_sensors_t msg_sensors;
	msg_sensors.time = (float)HAL_GetTick() / 1000;
taskENTER_CRITICAL();
	msg_sensors.temp = stateSensors.temp;
	msg_sensors.pressure = stateSensors.pressure;
	msg_sensors.height = stateSensors.height;
taskEXIT_CRITICAL();

	mavlink_message_t msg;
	uint16_t len = mavlink_msg_sensors_encode(UNISAT_ID, UNISAT_SENSORS, &msg, &msg_sensors);
	uint8_t buffer[100];
	len = mavlink_msg_to_send_buffer(buffer, &msg);
	uint8_t error = nRF24L01_send(&spi_nRF24L01, buffer, len, 1);
	return error;
}

static uint8_t mavlink_msg_gps_send() {

	mavlink_gps_t msg_gps;
	msg_gps.time = (float)HAL_GetTick() / 1000;
taskENTER_CRITICAL();
	for (int i = 0; i < 3; i++)
		msg_gps.coordinates[i] = stateGPS.coordinates[i];
taskEXIT_CRITICAL();

	mavlink_message_t msg;
	uint16_t len = mavlink_msg_gps_encode(UNISAT_ID, UNISAT_GPS, &msg, &msg_gps);
	uint8_t buffer[100];
	len = mavlink_msg_to_send_buffer(buffer, &msg);
	uint8_t error = nRF24L01_send(&spi_nRF24L01, buffer, len, 1);
	return error;
}

static uint8_t mavlink_msg_state_zero_send() {

	mavlink_state_zero_t msg_state_zero;
	msg_state_zero.time = (float)HAL_GetTick() / 1000;
taskENTER_CRITICAL();
	msg_state_zero.zero_pressure = state_zero.zero_pressure;
	for (int i = 0; i < 4; i++)
		msg_state_zero.zero_quaternion[i] = state_zero.zero_quaternion[i];
	for (int i = 0; i < 3; i++) {
		msg_state_zero.zero_GPS[i] = state_zero.zero_GPS[i];
		msg_state_zero.gyro_staticShift[i] = state_zero.gyro_staticShift[i];
	}
taskEXIT_CRITICAL();

	mavlink_message_t msg;
	uint16_t len = mavlink_msg_state_zero_encode(UNISAT_ID, UNISAT_GPS, &msg, &msg_state_zero);
	uint8_t buffer[100];
	len = mavlink_msg_to_send_buffer(buffer, &msg);
	uint8_t error = nRF24L01_send(&spi_nRF24L01, buffer, len, 1);
	return error;
}

static uint8_t mavlink_msg_camera_orientation_send() {

	mavlink_camera_orientation_t msg_camera_orient;
	msg_camera_orient.time = (float)HAL_GetTick() / 1000;
taskENTER_CRITICAL();
	msg_camera_orient.servo_pos = stateCamera_orient.servo_pos;
	msg_camera_orient.step_engine_pos = stateCamera_orient.step_engine_pos;
taskEXIT_CRITICAL();

	mavlink_message_t msg;
	uint16_t len = mavlink_msg_camera_orientation_encode(UNISAT_ID, UNISAT_GPS, &msg, &msg_camera_orient);
	uint8_t buffer[100];
	len = mavlink_msg_to_send_buffer(buffer, &msg);
	uint8_t error = nRF24L01_send(&spi_nRF24L01, buffer, len, 1);
	return error;
}


void IO_RF_Init() {
	//	//TODO: ДОБАВИТЬ РАБОТУ С НАЗЕМКОЙ (ПРОВЕРКА ВНУТРЕННЕГО БУФЕРА РАДИО-МОДУЛЯ)
//		uint8_t nRF24L01_initError = nRF24L01_init(&spi_nRF24L01);
	//	state_initErrors.NRF_E = nRF24L01_initError;

	//	запуск SD
	dump_init(&stream_file, filename);
}


void IO_RF_task() {

	const TickType_t _delay = 100 / portTICK_RATE_MS;
	for (;;) {

//		char* msg = "UNISAT_SD. Trying to transmit any data\n";
//		dump(&stream_file, msg, strlen(msg));

//		mavlink_msg_state_send();
		mavlink_msg_imu_isc_send();
		mavlink_msg_imu_rsc_send();
		mavlink_msg_sensors_send();
		mavlink_msg_gps_send();
		mavlink_msg_camera_orientation_send();


		vTaskDelay(_delay);

//		// Этап 0. Подтверждение инициализации отправкой пакета состояния и ожидание ответа от НС
//		if (state_system.globalStage == 0) {
//			mavlink_msg_state_send();
//			mavlink_msg_imu_rsc_send();
//			mavlink_msg_sensors_send();
//			mavlink_msg_gps_send();
//		}
//		// Этап 1. Погрузка в ракету
//		if (state_system.globalStage == 1) {
//			mavlink_msg_state_send();
//			mavlink_msg_gps_send();
//		}
//		// Этап 2. Определение начального состояния
//		if (state_system.globalStage == 2 && stateTasks_flags.zeroOrient_isSet) {
//			mavlink_msg_state_send();
//			mavlink_msg_imu_rsc_send();
//			mavlink_msg_sensors_send();
//			mavlink_msg_gps_send();
//
//			mavlink_msg_state_zero_send();
//
//			state_system.globalStage = 3;
//			continue;
//		}
//		// Этап 3. Полет в ракете
//		if (state_system.globalStage == 3) {
//			mavlink_msg_state_send();
//			mavlink_msg_imu_isc_send();
//			mavlink_msg_sensors_send();
//			mavlink_msg_gps_send();
//		}
//		// Этап 4. Свободное падение
//		if (state_system.globalStage == 4) {
//			mavlink_msg_state_send();
//			mavlink_msg_imu_isc_send();
//			mavlink_msg_sensors_send();
//			mavlink_msg_gps_send();
//			mavlink_msg_camera_orientation_send();
//		}
//		// Этап 5. Спуск
//		if (state_system.globalStage == 5) {
//			mavlink_msg_state_send();
//			mavlink_msg_imu_isc_send();
//			mavlink_msg_sensors_send();
//			mavlink_msg_gps_send();
//			mavlink_msg_camera_orientation_send();
//		}
//		// Этап 6. Окончание полета
//		if (state_system.globalStage == 6) {
//			mavlink_msg_state_send();
//			mavlink_msg_imu_isc_send();
//			mavlink_msg_sensors_send();
//			mavlink_msg_gps_send();
//			mavlink_msg_camera_orientation_send();
//		}
	}
}

