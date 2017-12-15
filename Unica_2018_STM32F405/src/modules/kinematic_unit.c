/*
 * KINEMATIC_UNIT
 *
 * 		This unit will be used for for getting orientation of device and it`s translations
 *  from transformed sensors data.
 * 		In a basis of getting orientation we use Madgwick`s algorythm,
 * 	which creates quaternion of translation from RSC to ISC from sensors data
 * 	(accelerometer, gyroscope, magnetometer).
 *
 * 	Authors: Korr237i, RaKetov
 */


#include <math.h>
#include <stdlib.h>

#include <sofa.h>
#include "state.h"
#include "MadgwickAHRS.h"
#include "quaternion.h"


void kinematicInit(state_t* state)
{
	for (int i = 0; i < 3; i++) {

		state->raw.accel[i] = 0;
		state->raw.gyro[i] = 0;
		state->raw.compass[i] = 0;

		state->rsc.accel[i] = 0;
		state->rsc.gyro[i] = 0;
		state->rsc.compass[i] = 0;

		state->isc.accel[i] = 0;
		state->isc.gyro[i] = 0;
		state->isc.compass[i] = 0;

		state->isc.velocities[i] = 0;
		state->isc.coord_IMU[i] = 0;
		state->isc.coord_GPS[i] = 0;
	}

	state->raw.GPS = 0;

	state->raw.temp = 0;
	state->raw.pressure = 0;

	state->isc.quaternion[0] = 0;
	state->isc.quaternion[1] = 0;
	state->isc.quaternion[2] = 0;
	state->isc.quaternion[3] = 0;

	state->sensors.temp = 0;
	state->sensors.pressure = 0;

	state->system.servo_pos = 0;
	state->system.step_engine_pos = 0;
	state->system.state = 0;
	state->system.time = 0;
}


void constructTrajectory(state_t * state, state_t * state_prev) {

	// getting integration time
	float dt = (float)(state->system.time - state_prev->system.time) / 1000;

	// getting rotation quaternion
	MadgwickAHRSupdate(state,
				state->rsc.gyro[0], state->rsc.gyro[1], state->rsc.gyro[2],
				state->rsc.accel[0], state->rsc.accel[1], state->rsc.accel[2],
				state->rsc.compass[0], state->rsc.compass[1], state->rsc.compass[2]
	);

	// rotation RSC vectors to ISC using quaternion
	vect_rotate(state->rsc.accel, state->isc.quaternion, state->isc.accel);
	vect_rotate(state->rsc.gyro, state->isc.quaternion, state->isc.gyro);
	vect_rotate(state->rsc.compass, state->isc.quaternion, state->isc.compass);

	// integration velocities and translations
	for (int i = 0; i < 3; i++) {

		state->isc.velocities[i] += (state->isc.accel[i] + state_prev->isc.accel[i]) * dt / 2;
		state->isc.coord_IMU[i] += (state->isc.velocities[i] + state_prev->isc.velocities[i]) * dt / 2;
	}

}

void apply_KalmanFilter(float * sensor_data, const float * sensor_data_prev, float Kalman_gain, int data_array_size) {
	for (int i = 0; i < data_array_size; i++)
		sensor_data[i] = Kalman_gain * sensor_data[i] + (1 - Kalman_gain) * sensor_data_prev[i];
}

void apply_NoiseFilter(float * sensor_data, float noise, int data_array_size) {
	for (int i = 0; i < data_array_size; i++)
	{
		if (fabsf(sensor_data[i]) < noise)
				sensor_data[i] = 0;
	}
}


