/*
 * quaternion.h
 *
 *  Created on: 01 нояб. 2017 г.
 *      Author: developer
 */

#ifndef MADGWICK_QUATERNION_H_
#define MADGWICK_QUATERNION_H_
/*

typedef struct {
	double x, y, z;
} vector_t;

typedef struct {
	double w, x, y, z;
} quaternion_t;
*/


void quat_mult(float* quaternion, float k, float* res);
float quat_abs(float* quaternion);
void quat_normalize(float* quaternion,  float* res);
void quat_invert(float * quaternion, float* res);
void quat_mult_by_quat(float * a, float * b, float* res);
void quat_mult_by_vect(float* a, float* b, float* res);
void vect_rotate(float* vect, float* rotation, float* res);


#endif /* MADGWICK_QUATERNION_H_ */
