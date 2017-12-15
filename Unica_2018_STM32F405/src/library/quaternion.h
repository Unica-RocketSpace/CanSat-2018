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


void quat_mult(const float* quaternion, float k, float* res);
float quat_abs(const float* quaternion);
void quat_normalize(const float* quaternion,  float* res);
void quat_invert(const float * quaternion, float* res);
void quat_mult_by_quat(const float * a, const float * b, float* res);
void quat_mult_by_vect(const float* a, const float* b, float* res);
void vect_rotate(const float* vect, const float* rotation, float* res);


#endif /* MADGWICK_QUATERNION_H_ */
