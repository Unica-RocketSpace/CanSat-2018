/*
 * TELEMETRY_UNIT
 *
 * 		This unit will be used for transmitting and receiving data
 * 	with device and earth using radio transmitter.
 *
 * 	Authors: Korr237i, RaKetov
 */

#include "telemetry.h"
#include "state.h"


void getPackageChecksum(package_t* pack) {

	uint8_t * firstpack = (uint8_t*)&pack;
	for (size_t i = 0; i < (sizeof(package_t) - sizeof(pack->CS)); i++) {
		pack->CS += *(firstpack + i);
	}
}

void fillPackage(package_t* pack, state_t* state) {

	//	marker
	pack->marker = 0xFFFF;

	//	temperature, pressure
	pack->temp = state->raw.temp;
	pack->pressure = state->raw.pressure;

	//	IMU data
	pack->accel[0] = state->raw.accel[0];
	pack->accel[1] = state->raw.accel[1];
	pack->accel[2] = state->raw.accel[2];

	pack->gyro[0] = state->raw.gyro[0];
	pack->gyro[1] = state->raw.gyro[1];
	pack->gyro[2] = state->raw.gyro[2];

	pack->compass[0] = state->raw.compass[0];
	pack->compass[1] = state->raw.compass[1];
	pack->compass[2] = state->raw.compass[2];

	//	GPS data
	pack->GPS[0] = state->isc.coord_GPS[0];
	pack->GPS[1] = state->isc.coord_GPS[1];
	pack->GPS[2] = state->isc.coord_GPS[2];

	// positions of servo and step engine
	pack->servo_pos = state->system.servo_pos;
	pack->step_engine_pos = state->system.step_engine_pos;

	//	state, time
	pack->state = state->system.state;
	pack->time = state->system.time;

	//	filling checksum
	getPackageChecksum(pack);

}

//	TODO: sendpackage()
//void sendpackage(package_t * pack) {
//
//
//}




