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

#include "FreeRTOS.h"
#include "task.h"

#include "state.h"
#include "MadgwickAHRS.h"
#include "quaternion.h"
#include "MPU9255.h"


void constructTrajectory(	stateIMU_isc_t* localStateIMU_isc,
							stateIMU_isc_t* localStateIMU_isc_prev,
							state_system_t* localState_system,
							state_system_t* localState_system_prev,
							stateIMU_rsc_t* localStateIMU_rsc
						) {

	// getting integration time
	float dt = (float)(localState_system->time - localState_system_prev->time);


	for (int i = 0; i < 3; i++) {
		localStateIMU_rsc->gyro[i] = M_PI * (localStateIMU_rsc->gyro[i] / 180.0f);
	}

	//FIXME: ЗАМЕНИТЬ ОБРАТНО
	// getting rotation quaternion
//	MadgwickAHRSupdate(localStateIMU_isc,
//				localStateIMU_rsc->gyro[0], localStateIMU_rsc->gyro[1],	localStateIMU_rsc->gyro[2],
//				localStateIMU_rsc->accel[0], localStateIMU_rsc->accel[1], localStateIMU_rsc->accel[2],
//				localStateIMU_rsc->compass[0], localStateIMU_rsc->compass[1], localStateIMU_rsc->compass[2], dt);
	MadgwickAHRSupdateIMU(localStateIMU_isc,
				localStateIMU_rsc->gyro[0],	localStateIMU_rsc->gyro[1],	localStateIMU_rsc->gyro[2],
				localStateIMU_rsc->accel[0], localStateIMU_rsc->accel[1], localStateIMU_rsc->accel[2], dt);

	// rotation RSC vectors to ISC using quaternion
	vect_rotate(localStateIMU_rsc->accel, localStateIMU_isc->quaternion, localStateIMU_isc->accel);
	vect_mult(localStateIMU_isc->accel, vect_abs(localStateIMU_rsc->accel), localStateIMU_isc->accel);
//	vect_rotate(localStateIMU_rsc->gyro, localStateIMU_isc->quaternion, localStateIMU_isc->gyro);
	vect_rotate(localStateIMU_rsc->compass, localStateIMU_isc->quaternion, localStateIMU_isc->compass);

//	localStateIMU_isc->accel[0] -= X_ACCEL_ADDITIONAL_OFFSET;
//	localStateIMU_isc->accel[1] -= Y_ACCEL_ADDITIONAL_OFFSET;
//	localStateIMU_isc->accel[2] -= (G_VECTOR / 10 + Z_ACCEL_ADDITIONAL_OFFSET);
taskENTER_CRITICAL();
	for (int i = 0; i < 3; i++) {
		localStateIMU_isc->accel[i] -= state_zero.accel_staticShift[i];
	}
taskEXIT_CRITICAL();
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


