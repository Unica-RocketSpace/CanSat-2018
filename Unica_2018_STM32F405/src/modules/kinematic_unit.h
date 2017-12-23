/*
 * kinematic_unit.h
 *
 *  Created on: 11 нояб. 2017 г.
 *      Author: developer
 */

#ifndef KINEMATIC_UNIT_H_
#define KINEMATIC_UNIT_H_

#include "state.h"

/*###############################################*/
/*################### ФУНКЦИИ ###################*/
/*###############################################*/

/*
 *  Инициализация структур состояния аппарата
 *  Параметры:
 *  	state - инициализируемая структура состояния аппарата
 */
void stateInit(state_t* state);


/*
 *  Построение траектории
 *  Параметры:
 *  	state		- структура текущего состояния аппарата
 *  	state_prev	- структура предыдущего состояния аппарата
 */
void constructTrajectory(state_t * state, state_t * state_prev);


#endif /* KINEMATIC_UNIT_H_ */
