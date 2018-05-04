/*
 * quaternion.c
 *
 *  Created on: 01 нояб. 2017 г.
 *      Author: developer
 */

#include "quaternion.h"

#include <math.h>

void quat_mult(float* quaternion, float k, float* res) {

	for (size_t i = 0; i < 4; i++) {
		*(res + i) = k * *(quaternion + i);
	}
}

float quat_abs(float* quaternion) {

	float tmp = 0;
	for (int i = 0; i < 4; i++) {
		tmp += pow(*(quaternion + i), 2);
	}

	return sqrt(tmp);
}


void quat_normalize(float* quaternion,  float* res) {

	quat_mult(quaternion, 1 / quat_abs(quaternion), res);
}

void vect_normalise(float* vect, float* res){
	float mod = sqrt(pow(*(vect+0),2) + pow(*(vect+1),2) + pow(*(vect+2),2));
	for (int i = 0; i < 3; i++) {
		*(res + i) /= mod;
	}
}


void quat_invert(float * quaternion, float* res) {

	for (int i = 0; i < 4; i++) {
		*(res + i) = - *(quaternion + i);
	}
	*res = *quaternion;

	float res_[4] = {0, 0, 0, 0};
	quat_normalize(res, res_); //FIXME: А точно надо? В статье было так
	res = res_;
}

void quat_mult_by_quat(float * a, float * b, float* res) {

	res[0] = a[0] * b[0] - a[1] * b[1] - a[2] * b[2] - a[3] * b[3];
	res[1] = a[0] * b[1] + a[1] * b[0] + a[2] * b[3] - a[3] * b[2];
	res[2] = a[0] * b[2] - a[1] * b[3] + a[2] * b[0] + a[3] * b[1];
	res[3] = a[0] * b[3] + a[1] * b[2] - a[2] * b[1] + a[3] * b[0];

	float res_[4] = {0, 0, 0, 0};
	quat_normalize(res, res_);
	res = res_;
}

void quat_mult_by_vect(float* a, float* b, float* res) {

	float vectQuat[4] = {0, b[1], b[2], b[3]};
	float res_[4] = {0, 0, 0, 0};

	quat_mult_by_quat(a, vectQuat, res_);
	for(int i = 0; i < 3; i++) {
		*(res + i) = res[i+1];
	}
	float res_n[3] = {0, 0, 0};
	vect_normalise(res, res_n);
	res = res_n;
}


void vect_rotate(float* vect, float* rotation, float* res) {
	float rot_normal[4] = {0, 0, 0, 0};
	quat_normalize(rotation, rot_normal);

	float tmp[4] = {0, 0, 0, 0};
	quat_mult_by_vect(rot_normal, vect, tmp);

	float inverted[4] = {0, 0, 0, 0};
	quat_invert(rot_normal, inverted);
//	quat_mult_by_quat(tmp, inverted, res);
	quat_mult_by_vect(inverted, vect, res);

}

