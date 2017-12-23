/*
 * state.c
 *
 *  Created on: 18 нояб. 2017 г.
 *      Author: developer
 */

#include "state.h"

state_t* globalState = NULL;
state_t* globalState_prev = NULL;

void swapState(state_t * state, state_t * state_previous) {
	state_previous = state;
}
