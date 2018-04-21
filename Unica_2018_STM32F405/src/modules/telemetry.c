/*
 * TELEMETRY_UNIT
 *
 * 		This unit will be used for transmitting and receiving data
 * 	with device and earth using radio transmitter.
 *
 * 	Authors: Korr237i, RaKetov
 */

#include <stdint.h>

#include "diag/Trace.h"

#include "FreeRTOS.h"
#include "task.h"

#include "state.h"
#include "c_source/UNISAT_messages/mavlink.h"
#include "telemetry.h"
#include "nRF24L01.h"


uint8_t UNISAT_ID = 0x01;
uint8_t UNISAT_NoComp = 0xFF;
uint8_t UNISAT_IMU = 0x02;
uint8_t UNISAT_SENSORS = 0x03;
uint8_t UNISAT_GPS = 0x04;
uint8_t UNISAT_RPI = 0x05;
uint8_t UNISAT_CAM = 0x06;

/*uint8_t mavlink_msg_state_send() {

	mavlink_message_t msgState;
	uint16_t len_msgState = mavlink_msg_state_pack(
		UNISAT_ID, UNISAT_NoComp,
		&msgState,
		state_system.accel_state, state_system.gyro_state, state_system.compass_state,
		state_system.baro_state, state_system.GPS_state, state_system.RPI_state, 0, HAL_GetTick()/1000);
	//FIXME: error = HAL_SPI_Transmit(hspi, pData, Size, Timeout);
	//return error;
}

uint8_t mavlink_msg_imu_rsc_send() {

	mavlink_message_t msgIMU;
	uint16_t len_msgIMU = mavlink_msg_imu_rsc_pack(
		UNISAT_ID, UNISAT_IMU,
		&msgIMU,
		stateIMU_rsc.accel, stateIMU_rsc.gyro, stateIMU_rsc.compass,
		HAL_GetTick()/1000);
	//FIXME: error = HAL_SPI_Transmit(hspi, pData, Size, Timeout);
	//return error;
}

uint8_t mavlink_msg_imu_isc_send() {

	mavlink_message_t msgIMU;
	uint16_t len_msgIMU = mavlink_msg_imu_isc_pack(
		UNISAT_ID, UNISAT_IMU,
		&msgIMU,
		stateIMU_isc.accel, stateIMU_isc.gyro, stateIMU_isc.compass,
		stateIMU_isc.velocities, stateIMU_isc.coordinates, stateIMU_isc.quaternion, HAL_GetTick()/1000);
	//FIXME: error = HAL_SPI_Transmit(hspi, pData, Size, Timeout);
	//return error;
}

uint8_t mavlink_msg_sensors_send() {

	mavlink_message_t msgSensors;
	uint16_t len_msgSensors = mavlink_msg_sensors_pack(
		UNISAT_ID, UNISAT_SENSORS,
		&msgSensors,
		stateSensors.temp, stateSensors.pressure, stateSensors.height, HAL_GetTick()/1000);
	//FIXME: error = HAL_SPI_Transmit(hspi, pData, Size, Timeout);
	//return error;
}

uint8_t mavlink_msg_gps_send() {

	mavlink_message_t msgGPS;
	uint16_t len_msgGPS = mavlink_msg_gps_pack(
		UNISAT_ID, UNISAT_GPS,
		&msgGPS, stateGPS.coordinates, HAL_GetTick());
	//FIXME: error = HAL_SPI_Transmit(hspi, pData, Size, Timeout);
	//return error;
}

uint8_t mavlink_msg_state_zero_send() {

	mavlink_message_t msgStateZero;
	uint16_t len_msgStateZero = mavlink_msg_state_zero_pack(
		UNISAT_ID, UNISAT_NoComp,
		&msgStateZero,
		state_zero.zero_pressure, state_zero.zero_quaternion,
		state_zero.zero_GPS, state_zero.gyro_staticShift, HAL_GetTick()/1000);
	//FIXME: error = HAL_SPI_Transmit(hspi, pData, Size, Timeout);
	//return error;

}

uint8_t mavlink_msg_camera_orientation_send() {

	mavlink_message_t msgCameraOrient;
	uint16_t len_msgCameraOrient = mavlink_msg_camera_orientation_pack(
		UNISAT_ID, UNISAT_CAM,
		&msgCameraOrient,
		stateCamera_orient.servo_pos, stateCamera_orient.step_engine_pos, HAL_GetTick()/1000);
	//FIXME: error = HAL_SPI_Transmit(hspi, pData, Size, Timeout);
	//return error;
}
*/

void IO_RF_task() {

	//TODO: ДОБАВИТЬ РАБОТУ С НАЗЕМКОЙ (ПРОВЕРКА ВНУТРЕННЕГО БУФЕРА РАДИО-МОДУЛЯ)
//	taskENTER_CRITICAL();
	nRF24L01_init(&spi_nRF24L01);
//	taskEXIT_CRITICAL();

	uint32_t i = 0;
	const TickType_t _delay = 500 / portTICK_RATE_MS;
	for (;;) {

//		taskENTER_CRITICAL();
//		printf("%f, %f, %f\n", stateIMU_isc.accel[0], stateIMU_isc.accel[1], stateIMU_isc.accel[2]);
//		printf("%f, %f, %f\n", stateIMU_isc.gyro[0], stateIMU_isc.gyro[1], stateIMU_isc.gyro[2]);
//		printf("%f, %f, %f\n", stateIMU_isc.compass[0], stateIMU_isc.compass[1], stateIMU_isc.compass[2]);
//		printf("\n");
		uint8_t nRF_status = nRF24L01_read_status(&spi_nRF24L01);
//		printf("nRF_status = %d\n", nRF_status);
		printf("RX_DR = %d TX_DS = %d MAX_RT = %d RX_P_NO = %d TX_FULL = %d\n",
						(((nRF_status) & (1 << RX_DR)) >> RX_DR),
						(((nRF_status) & (1 << TX_DS)) >> TX_DS),
						(((nRF_status) & (1 << MAX_RT)) >> MAX_RT),
						(((nRF_status) & (0b111 << RX_P_NO)) >> RX_P_NO),
						(((nRF_status) & (1 << TX_FULL))) >> TX_FULL);
//		if (nRF_status & (1 << TX_FULL))
		nRF24L01_clear_TX_FIFO(&spi_nRF24L01);
		nRF24L01_clear_status(&spi_nRF24L01, true, true, true);

taskENTER_CRITICAL();
		char buffer[32];
		sprintf(buffer, "UNICA's broadcasting %lu", i);
		nRF24L01_write(&spi_nRF24L01, (uint8_t*)buffer, strlen(buffer), 1);
		i++;
taskEXIT_CRITICAL();

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

