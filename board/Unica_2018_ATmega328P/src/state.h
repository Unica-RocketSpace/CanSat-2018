/*
 * state.h
 *
 *  Created on: 10 марта 2018 г.
 *      Author: developer
 */

#ifndef STATE_H_
#define STATE_H_

#include "bmp280.h"


typedef struct {
	uint32_t package_number;
	float time;
	float pressure;
	float temperature;
	float height;
	uint8_t baro_state;
	uint8_t motor_state;
	uint8_t para_state;
	uint8_t globalStage;
}TM_package_t;

typedef struct {
	float zero_pressure;
}initial_params_t;

typedef struct {
	int32_t bmp280_rawpress;
	int32_t bmp280_rawtemp;
}raw_data_t;

rscs_bmp280_descriptor_t * bmp280;

extern TM_package_t TM_package;
extern initial_params_t initial_params;
extern raw_data_t raw_data;
extern const rscs_bmp280_calibration_values_t * bmp280_calibration_values;


#endif /* STATE_H_ */
