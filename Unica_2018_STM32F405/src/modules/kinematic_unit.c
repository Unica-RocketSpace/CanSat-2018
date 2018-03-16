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

void constructTrajectory(	stateIMU_isc_t* localStateIMU_isc,
							stateIMU_isc_t* localStateIMU_isc_prev,
							state_system_t* localState_system,
							state_system_t* localState_system_prev,
							stateIMU_rsc_t* localStateIMU_rsc
						) {

	// getting integration time
	float dt = (float)(localState_system->time - localState_system_prev->time);

	// getting rotation quaternion
	MadgwickAHRSupdate(localStateIMU_isc,
				localStateIMU_rsc->gyro[0]-state_zero.gyro_staticShift[0],
				localStateIMU_rsc->gyro[1]-state_zero.gyro_staticShift[1],
				localStateIMU_rsc->gyro[2]-state_zero.gyro_staticShift[2],
				localStateIMU_rsc->accel[0], localStateIMU_rsc->accel[1], localStateIMU_rsc->accel[2],
				localStateIMU_rsc->compass[0], localStateIMU_rsc->compass[1], localStateIMU_rsc->compass[2]
	);

	// rotation RSC vectors to ISC using quaternion
	vect_rotate(localStateIMU_rsc->accel, localStateIMU_isc->quaternion, localStateIMU_isc->accel);
	vect_rotate(localStateIMU_rsc->gyro, localStateIMU_isc->quaternion, localStateIMU_isc->gyro);
	vect_rotate(localStateIMU_rsc->compass, localStateIMU_isc->quaternion, localStateIMU_isc->compass);

	// integration velocities and translations
	for (int i = 0; i < 3; i++) {

		localStateIMU_isc->velocities[i] += (localStateIMU_isc->accel[i] + localStateIMU_isc_prev->accel[i]) * dt / 2;
		localStateIMU_isc->coordinates[i] += (localStateIMU_isc->velocities[i] + localStateIMU_isc_prev->velocities[i]) * dt / 2;
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


