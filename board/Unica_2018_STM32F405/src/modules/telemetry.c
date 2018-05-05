/*
 * TELEMETRY_UNIT
 *
 * 		This unit will be used for transmitting and receiving data
 * 	with device and earth using radio transmitter.
 *
 * 	Authors: Korr237i, RaKetov
 */

#include "diag/Trace.h"

#include "FreeRTOS.h"
#include "task.h"

#include "mavlink/UNISAT/mavlink.h"

#include "stdint.h"

#include "state.h"
#include "telemetry.h"
#include "nRF24L01.h"

#include "mavlink/UNISAT/mavlink.h"

uint8_t UNISAT_ID = 0x01;
uint8_t UNISAT_NoComp = 0xFF;
uint8_t UNISAT_IMU = 0x02;
uint8_t UNISAT_SENSORS = 0x03;
uint8_t UNISAT_GPS = 0x04;
uint8_t UNISAT_RPI = 0x05;
uint8_t UNISAT_CAM = 0x06;

static uint8_t mavlink_msg_state_send() {

	mavlink_message_t msgState;
taskENTER_CRITICAL();
	mavlink_msg_state_pack(
		UNISAT_ID, UNISAT_NoComp,
		&msgState,
		state_system.accel_state, state_system.gyro_state, state_system.compass_state,
		state_system.baro_state, state_system.GPS_state, state_system.RPI_state, 0, HAL_GetTick()/1000);
taskEXIT_CRITICAL();
	uint8_t buffer[100];
	uint16_t len = mavlink_msg_to_send_buffer(buffer, &msgState);
	uint8_t error = nRF24L01_send(&spi_nRF24L01, buffer, len, 1);
	return error;
}

static uint8_t mavlink_msg_imu_rsc_send() {

	mavlink_message_t msgIMU;
taskENTER_CRITICAL();
	mavlink_msg_imu_rsc_pack(
		UNISAT_ID, UNISAT_IMU,
		&msgIMU,
		stateIMU_rsc.accel, stateIMU_rsc.gyro, stateIMU_rsc.compass,
		HAL_GetTick()/1000);
taskEXIT_CRITICAL();
	uint8_t buffer[100];
	uint16_t len = mavlink_msg_to_send_buffer(buffer, &msgIMU);
	uint8_t error = nRF24L01_send(&spi_nRF24L01, buffer, len, 1);
	return error;
}

static uint8_t mavlink_msg_imu_isc_send() {

	mavlink_message_t msgIMU;
taskENTER_CRITICAL();
	mavlink_msg_imu_isc_pack(
		UNISAT_ID, UNISAT_IMU,
		&msgIMU,
		stateIMU_isc.accel, stateIMU_isc.gyro, stateIMU_isc.compass,
		stateIMU_isc.velocities, stateIMU_isc.coordinates, stateIMU_isc.quaternion, HAL_GetTick()/1000);
taskEXIT_CRITICAL();
	uint8_t buffer[100];
	uint16_t len = mavlink_msg_to_send_buffer(buffer, &msgIMU);
//	printf("msg body = ");
//	for (size_t i = 0; i < len; i++)
//	{
//		printf("%02X ", (int)buffer[i]);
//	}
//	printf("\n");

	uint8_t error = nRF24L01_send(&spi_nRF24L01, buffer, len, 1);
	return error;
}

static uint8_t mavlink_msg_sensors_send() {

	mavlink_message_t msgSensors;
taskENTER_CRITICAL();
	mavlink_msg_sensors_pack(
		UNISAT_ID, UNISAT_SENSORS,
		&msgSensors,
		stateSensors.temp, stateSensors.pressure, stateSensors.height, HAL_GetTick()/1000);
taskEXIT_CRITICAL();
	uint8_t buffer[100];
	uint16_t len = mavlink_msg_to_send_buffer(buffer, &msgSensors);
	uint8_t error = nRF24L01_send(&spi_nRF24L01, buffer, len, 1);
	return error;
}

static uint8_t mavlink_msg_gps_send() {

	mavlink_message_t msgGPS;
taskENTER_CRITICAL();
	mavlink_msg_gps_pack(
		UNISAT_ID, UNISAT_GPS,
		&msgGPS, stateGPS.coordinates, HAL_GetTick());
taskEXIT_CRITICAL();
	uint8_t buffer[100];
	uint16_t len = mavlink_msg_to_send_buffer(buffer, &msgGPS);
	uint8_t error = nRF24L01_send(&spi_nRF24L01, buffer, len, 1);
	return error;
}

static uint8_t mavlink_msg_state_zero_send() {

	mavlink_message_t msgStateZero;
taskENTER_CRITICAL();
	mavlink_msg_state_zero_pack(
		UNISAT_ID, UNISAT_NoComp,
		&msgStateZero,
		state_zero.zero_pressure, state_zero.zero_quaternion,
		state_zero.zero_GPS, state_zero.gyro_staticShift, HAL_GetTick()/1000);
taskEXIT_CRITICAL();
	uint8_t buffer[100];
	uint16_t len = mavlink_msg_to_send_buffer(buffer, &msgStateZero);
	uint8_t error = nRF24L01_send(&spi_nRF24L01, buffer, len, 1);
	return error;
}

static uint8_t mavlink_msg_camera_orientation_send() {

	mavlink_message_t msgCameraOrient;
taskENTER_CRITICAL();
	mavlink_msg_camera_orientation_pack(
		UNISAT_ID, UNISAT_CAM,
		&msgCameraOrient,
		stateCamera_orient.servo_pos, stateCamera_orient.step_engine_pos, HAL_GetTick()/1000);
taskEXIT_CRITICAL();
	uint8_t buffer[100];
	uint16_t len = mavlink_msg_to_send_buffer(buffer, &msgCameraOrient);
	uint8_t error = nRF24L01_send(&spi_nRF24L01, buffer, len, 1);
	return error;
}


void IO_RF_task() {

	//TODO: ДОБАВИТЬ РАБОТУ С НАЗЕМКОЙ (ПРОВЕРКА ВНУТРЕННЕГО БУФЕРА РАДИО-МОДУЛЯ)
	uint8_t nRF24L01_initError = nRF24L01_init(&spi_nRF24L01);
	vTaskDelay(100/portTICK_RATE_MS);
	state_initErrors.NRF_E = nRF24L01_initError;
	printf("nRF24L01 error: %d\n", nRF24L01_initError);

	const TickType_t _delay = 500 / portTICK_RATE_MS;
	for (;;) {

		uint8_t nRF_status = 0;
		nRF24L01_read_status(&spi_nRF24L01, &nRF_status);
		printf("RX_DR = %d TX_DS = %d MAX_RT = %d RX_P_NO = %d TX_FULL = %d\n",
						(((nRF_status) & (1 << RX_DR)) >> RX_DR),
						(((nRF_status) & (1 << TX_DS)) >> TX_DS),
						(((nRF_status) & (1 << MAX_RT)) >> MAX_RT),
						(((nRF_status) & (0b111 << RX_P_NO)) >> RX_P_NO),
						(((nRF_status) & (1 << TX_FULL))) >> TX_FULL);
//		if (nRF_status & (1 << TX_FULL))
//		nRF24L01_clear_TX_FIFO(&spi_nRF24L01);
//		nRF24L01_clear_status(&spi_nRF24L01, true, true, true);
//		char buffer[32];
//		sprintf(buffer, "UNICA's broadcasting %lu\n", i);
//		nRF24L01_write(&spi_nRF24L01, (uint8_t*)buffer, strlen(buffer), 1);
//		i++;

//		nRF24L01_clear_status(&spi_nRF24L01, true, true, true);
//
//		if (nRF_status & (1 << TX_FULL))
//			nRF24L01_clear_TX_FIFO(&spi_nRF24L01);

		mavlink_msg_state_send();
		mavlink_msg_imu_isc_send();
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

