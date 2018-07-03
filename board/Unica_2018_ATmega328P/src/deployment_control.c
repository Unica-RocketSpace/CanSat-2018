/*
 * deployment_control.c
 *
 *  Created on: 22 дек. 2017 г.
 *      Author: developer
 */

#include <stdint.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <math.h>
#include <stdext/stdio.h>

#include "state.h"
#include "deployment_control.h"


float set_zero_pressure()
{
	int32_t raw_pressure32, raw_temp32;
	float pressure32, temp32;
	rscs_bmp280_read(bmp280, &raw_pressure32, &raw_temp32);

	const rscs_bmp280_calibration_values_t * calibrate_values_ = rscs_bmp280_get_calibration_values(bmp280);
	rscs_bmp280_calculate(calibrate_values_, raw_pressure32, raw_temp32, &pressure32, &temp32);

	return pressure32;
}

float calculate_height(float pressure, float zero_pressure){
	return 18.4 * log(zero_pressure / pressure);
};

bool check_height (float height){
	if (height <= MIN_HEIGHT) return true;
	return false;
};


bool check_command (uint8_t * command, uint8_t command_len){
	if (*command == DEPLOYMENT_COMMAND) return true;
	return false;
}

void deploy_parashute (){
	ENGINE_DDR = 1;
	ENGINE_PORT |= (1 << ENGINE_PIN);
}

uint8_t new_h = 0;
uint8_t count = 0;

//bool check_down_height(){
//	float height[2];
//	height[new_h] = TM_package.height;
//	if (new_h == 0) new_h ++;
//	if (new_h == 1){
//		if (height[new_h] > height[new_h - 1]) count ++;
//		new_h ++;
//	}
//
//	if(new_h == 2) new_h = 0;
//	if(count == 5)	return true;
//	return false;
//}

bool check_invariable_height(){
	float height[5];
	height[new_h] = TM_package.height;
	if (new_h == 4) new_h = 0;
	else new_h++;

	int8_t med_h = new_h - 2;
	if (med_h < 0) med_h += 5;
	int8_t old_h = new_h - 4;
	if (old_h < 0) old_h += 5;

	if (((height[old_h] - height[med_h]) < INVARIABLE_DELTA_HEIGHT) && (((height[med_h] - height[new_h]) < INVARIABLE_DELTA_HEIGHT))) return true;
	return false;
}

ISR(INT0_vect)
{
	TM_package.para_state = 1;
}

