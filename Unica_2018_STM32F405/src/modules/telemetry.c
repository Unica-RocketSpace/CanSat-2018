/*
 * TELEMETRY_UNIT
 *
 * 		This unit will be used for transmitting and receiving data
 * 	with device and earth using radio transmitter.
 *
 * 	Authors: Korr237i, RaKetov
 */

#include <stdint.h>

#include "FreeRTOS.h"
#include "task.h"

#include "state.h"
#include "c_source/UNISAT_messages/mavlink.h"
#include "telemetry.h"


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


void IO_RF_task() {

	//TODO: ДОБАВИТЬ РАБОТУ С НАЗЕМКОЙ (ПРОВЕРКА ВНУТРЕННЕГО БУФЕРА РАДИО-МОДУЛЯ)

	for (; ;) {

		// Этап 0. Подтверждение инициализации отправкой пакета состояния и ожидание ответа от НС
		if (state_system.globalStage == 0) {
			mavlink_msg_state_send();
			mavlink_msg_imu_rsc_send();
			mavlink_msg_sensors_send();
			mavlink_msg_gps_send();
		}
		// Этап 1. Погрузка в ракету
		if (state_system.globalStage == 1) {
			mavlink_msg_state_send();
			mavlink_msg_gps_send();
		}
		// Этап 2. Определение начального состояния
		if (state_system.globalStage == 2 && stateTasks_flags.zeroOrient_isSet) {
			mavlink_msg_state_send();
			mavlink_msg_imu_rsc_send();
			mavlink_msg_sensors_send();
			mavlink_msg_gps_send();

			mavlink_msg_state_zero_send();

			state_system.globalStage = 3;
			continue;
		}
		// Этап 3. Полет в ракете
		if (state_system.globalStage == 3) {
			mavlink_msg_state_send();
			mavlink_msg_imu_isc_send();
			mavlink_msg_sensors_send();
			mavlink_msg_gps_send();
		}
		// Этап 4. Свободное падение
		if (state_system.globalStage == 4) {
			mavlink_msg_state_send();
			mavlink_msg_imu_isc_send();
			mavlink_msg_sensors_send();
			mavlink_msg_gps_send();
			mavlink_msg_camera_orientation_send();
		}
		// Этап 5. Спуск
		if (state_system.globalStage == 5) {
			mavlink_msg_state_send();
			mavlink_msg_imu_isc_send();
			mavlink_msg_sensors_send();
			mavlink_msg_gps_send();
			mavlink_msg_camera_orientation_send();
		}
		// Этап 6. Окончание полета
		if (state_system.globalStage == 6) {
			mavlink_msg_state_send();
			mavlink_msg_imu_isc_send();
			mavlink_msg_sensors_send();
			mavlink_msg_gps_send();
			mavlink_msg_camera_orientation_send();
		}
	}
}*/

