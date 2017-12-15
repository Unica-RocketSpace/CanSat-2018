/*
 * telemetry.h
 *
 *  Created on: 11 нояб. 2017 г.
 *      Author: developer
 */

#ifndef TELEMETRY_H_
#define TELEMETRY_H_

#include <stdint.h>
#include "state.h"

//	Default structure for transmitting data
typedef struct {

	uint16_t marker;	// marker of package
	uint16_t number;	// number of package

	int16_t temp;		// raw temperature data
	int16_t pressure;	// raw pressure data

	int16_t accel[3];	// raw accelerometer data
	int16_t gyro[3];	// raw gyroscope data
	int16_t compass[3];	// raw magnetometer data
	float GPS[3];		// coordinates from GPS

	float servo_pos;		// position of servo
	float step_engine_pos;	// position of step engine

	uint16_t state;		// state of device
	float time;			// time of transmitting

	uint32_t CS;		// checksum

} package_t;


/*############# FUNCTIONS #############*/

/*
 * 	Getting checksum of current (PACK) package
 */
void getPackageChecksum(package_t * PACK);

/*
 * 	This function fills package with data got from STATE global data structure
 */
void fillPackage(package_t * PACK, state_t * STATE);



#endif /* TELEMETRY_H_ */
