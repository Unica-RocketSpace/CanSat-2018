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
 *  Построение траектории
 *  Параметры:
 *
 */
void constructTrajectory(	stateIMU_isc_t* localStateIMU_isc,
							stateIMU_isc_t* localStateIMU_isc_prev,
							state_system_t* localState_system,
							state_system_t* localState_system_prev,
							stateIMU_rsc_t* localStateIMU_rsc
						);


#endif /* KINEMATIC_UNIT_H_ */
