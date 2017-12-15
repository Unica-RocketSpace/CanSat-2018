/*
 * quaternion.c
 *
 *  Created on: 01 нояб. 2017 г.
 *      Author: developer
 */

#include "quaternion.h"

#include <math.h>

void quat_mult(const float* quaternion, float k, float* res) {

	for (size_t i = 0; i < 4; i++) {
		res[i] = k * *(quaternion + i);
	}
}

float quat_abs(const float* quaternion) {

	float tmp = 0;
	for (int i = 0; i < 4; i++) {
		tmp += *(quaternion + i);
	}

	return sqrt(tmp);
}


void quat_normalize(const float* quaternion,  float* res) {

	quat_mult(quaternion, 1 / quat_abs(quaternion), res);
}


void quat_invert(const float * quaternion, float* res) {

	for (int i = 1; i < 4; i++) {
		*(res + i) = - *(quaternion + i);
	}
	float res_[4] = {0, 0, 0, 0};

	quat_normalize(res, res_); //FIXME: А точно надо? В статье было так
	res = res_;
}

void quat_mult_by_quat(const float * a, const float * b, float* res) {

	res[0] = a[0] * b[0] - a[1] * b[1] - a[2] * b[2] - a[3] * b[3];
	res[1] = a[0] * b[1] + a[1] * b[0] + a[2] * b[3] - a[3] * b[2];
	res[2] = a[0] * b[2] - a[1] * b[3] + a[2] * b[0] + a[3] * b[1];
	res[3] = a[0] * b[3] + a[1] * b[2] - a[2] * b[1] + a[3] * b[0];

}

void quat_mult_by_vect(const float* a, const float* b, float* res) {

	float vectQuat[4] = {0, b[1], b[2], b[3]};

	quat_mult_by_quat(a, vectQuat, res);
}


void vect_rotate(const float* vect, const float* rotation, float* res) {
	float rot_normal[4] = {0, 0, 0, 0};
	quat_normalize(rotation, rot_normal);

	float tmp[4] = {0, 0, 0, 0};
	quat_mult_by_vect(rot_normal, vect, tmp);

	float inverted[4] = {0, 0, 0, 0};
	quat_invert(rot_normal, inverted);
	quat_mult_by_quat(tmp, inverted, res);

}

