/*
 * dynamic_unit.c
 *
 *  Created on: 11 нояб. 2017 г.
 *      Author: developer
 */

#include <stm32f4xx_hal.h>
#include <math.h>

#include "dynamic_unit.h"
#include "state.h"
#include "quaternion.h"

#define STEP_DIVIDER		2

#define DRV8855_STEP_PIN	GPIO_PIN_3
#define DRV8855_STEP_PORT	GPIOC
#define DRV8855_DIR_PIN		GPIO_PIN_4
#define DRV8855_DIR_PORT	GPIOA
#define DRV8855_MODE0_PIN	GPIO_PIN_2
#define DRV8855_MODE0_PORT	GPIOD
#define DRV8855_MODE1_PIN	GPIO_PIN_11
#define DRV8855_MODE1_PORT	GPIOC
#define DRV8855_MODE2_PIN	GPIO_PIN_10
#define DRV8855_MODE2_PORT	GPIOA
#define DRV8855_nFAULT_PIN	GPIO_PIN_11
#define DRV8855_nFAULT_PORT	GPIOB

#define TARGET_X			100
#define TARGET_Y			100



void step_engine_init () {

	GPIO_InitTypeDef DRV8825_pins;

	/*Pin STEP*/
	DRV8825_pins.Pin = DRV8855_STEP_PIN;
	DRV8825_pins.Mode = GPIO_MODE_OUTPUT_PP;
	DRV8825_pins.Pull = GPIO_NOPULL;
	DRV8825_pins.Speed = GPIO_SPEED_FREQ_MEDIUM;
	HAL_GPIO_Init(DRV8855_STEP_PORT, &DRV8825_pins);

	/*Pin DIR*/
	DRV8825_pins.Pin = DRV8855_DIR_PIN;
	DRV8825_pins.Mode = GPIO_MODE_OUTPUT_PP;
	DRV8825_pins.Pull = GPIO_NOPULL;
	DRV8825_pins.Speed = GPIO_SPEED_FREQ_MEDIUM;
	HAL_GPIO_Init(DRV8855_DIR_PORT, &DRV8825_pins);

	/*Pin MODE0*/
	DRV8825_pins.Pin = DRV8855_MODE0_PIN;
	DRV8825_pins.Mode = GPIO_MODE_OUTPUT_PP;
	DRV8825_pins.Pull = GPIO_NOPULL;
	DRV8825_pins.Speed = GPIO_SPEED_FREQ_MEDIUM;
	HAL_GPIO_Init(DRV8855_MODE0_PORT, &DRV8825_pins);
	/*Pin MODE1*/
	DRV8825_pins.Pin = DRV8855_MODE1_PIN;
	DRV8825_pins.Mode = GPIO_MODE_OUTPUT_PP;
	DRV8825_pins.Pull = GPIO_NOPULL;
	DRV8825_pins.Speed = GPIO_SPEED_FREQ_MEDIUM;
	HAL_GPIO_Init(DRV8855_MODE1_PORT, &DRV8825_pins);
	/*Pin MODE2*/
	DRV8825_pins.Pin = DRV8855_MODE2_PIN;
	DRV8825_pins.Mode = GPIO_MODE_OUTPUT_PP;
	DRV8825_pins.Pull = GPIO_NOPULL;
	DRV8825_pins.Speed = GPIO_SPEED_FREQ_MEDIUM;
	HAL_GPIO_Init(DRV8855_MODE2_PORT, &DRV8825_pins);

	/*Pin nFAULT*/
	DRV8825_pins.Pin = DRV8855_nFAULT_PIN;
	DRV8825_pins.Mode = GPIO_MODE_INPUT;
	DRV8825_pins.Pull = GPIO_NOPULL;
	DRV8825_pins.Speed = GPIO_SPEED_FREQ_MEDIUM;
	HAL_GPIO_Init(DRV8855_nFAULT_PORT, &DRV8825_pins);

	//TODO:Написать определение MODE-ов с учетом STEP_DIVIDER
	HAL_GPIO_WritePin(DRV8855_MODE0_PORT, DRV8855_MODE0_PIN, STEP_DIVIDER & (1<<0));
	HAL_GPIO_WritePin(DRV8855_MODE1_PORT, DRV8855_MODE1_PIN, STEP_DIVIDER & (1<<1));
	HAL_GPIO_WritePin(DRV8855_MODE2_PORT, DRV8855_MODE2_PIN, STEP_DIVIDER & (1<<2));

}



void calculate_angles (stateIMU_isc_t* localStateIMU_isc, float* servo_ang, float* step_motor_ang) {

	float quat_ISC_RSC[4] = {0, 0, 0, 0};		//	кватернион для перехода из инерциальной системы в связанную
	float target[3] = {0, 0, 0};				//	направляющий вектор цели
	float target_RSC[3] = {0, 0, 0};			//	направляющий вектор цели в ССК

	//TODO: CHOOSE WHAT DATA TO USE: IMU OR GPS
	target[0] = TARGET_X - localStateIMU_isc->coord_IMU[0];
	target[1] = TARGET_Y - localStateIMU_isc->coord_IMU[1];
	target[2] = - localStateIMU_isc->coord_IMU[2];

	quat_invert(localStateIMU_isc->quaternion, quat_ISC_RSC);		//	получаем кватернион ИСК->ССК
	vect_rotate(target, quat_ISC_RSC, target_RSC);	//	получаем вектор цели в ССК

	*step_motor_ang = atan(target_RSC[0] / target_RSC[1]);
	*servo_ang = atan(target_RSC[2] / sqrt(pow(target_RSC[0], 2) + pow(target_RSC[1], 2)));
}


error rotate_step_engine (state_system_t* localState_system, state_system_t* localState_system_prev) {

	if (HAL_GPIO_ReadPin(DRV8855_nFAULT_PORT, DRV8855_nFAULT_PIN) == 0) return driver_overheat;

	float STEP_DEGREES = localState_system->step_engine_pos - localState_system_prev->step_engine_pos;

	rotate_step_engine_by_angles(&STEP_DEGREES);

	return no_error;
}

void rotate_step_engine_by_angles (float * angles) {

	float STEP_DEGREES = *angles;

	if (STEP_DEGREES > 0) HAL_GPIO_WritePin(DRV8855_DIR_PORT, DRV8855_DIR_PIN, 1);
	else HAL_GPIO_WritePin(DRV8855_DIR_PORT, DRV8855_DIR_PIN, 0);

	float STEP_TERNS= STEP_DEGREES/ (M_PI *2) * 200 * pow(2, STEP_DIVIDER);
	for(int i = 0; i < round(STEP_TERNS); i++) {
		HAL_GPIO_WritePin(DRV8855_STEP_PORT, DRV8855_STEP_PIN, 1);
		for(int j = 0; j< 15000; j++){}
		HAL_GPIO_WritePin(DRV8855_STEP_PORT, DRV8855_STEP_PIN, 0);
		for(int j = 0; j< 15000; j++){}
	}

}

