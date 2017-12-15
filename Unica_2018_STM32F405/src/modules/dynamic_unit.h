/*
 * dynamic_unit.h
 *
 *  Created on: 11 нояб. 2017 г.
 *      Author: developer
 */

#ifndef DYNAMIC_UNIT_H_
#define DYNAMIC_UNIT_H_

#include "state.h"

void step_engine_init ();
void calculate_angles (state_t * state);
error rotate_step_engine (state_t * stare);
void rotate_step_engine_by_angles( float *angles);


#endif /* DYNAMIC_UNIT_H_ */
