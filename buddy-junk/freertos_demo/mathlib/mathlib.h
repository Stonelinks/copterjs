/*
 * mathlib.h
 *
 *  Created on: Sep 26, 2013
 *      Author: bmichini
 */

#ifndef MATHLIB_H_
#define MATHLIB_H_

#include <math.h>
#include "inc/hw_types.h"
//#include "IQmath/IQmathLib.h"
//#include "IQmath/IQmathCPP.h"


float satf( float in, float min, float max);


// ## Quaternion library ## //
typedef struct Quaternion{
	float o;
	float x;
	float y;
	float z;
}tQuaternion;

void q2Euler(tQuaternion * q, float * roll, float * pitch, float * yaw, tBoolean deg);
tQuaternion qprod(tQuaternion a, tQuaternion b);
tQuaternion qunit();
void qpropagate(tQuaternion * a, float p, float q, float r, float dt );
void qcorrect( tQuaternion * est, tQuaternion * meas, float K, float sat);
//tQuaternion qprodconj(tQuaternion a_conj, tQuaternion b);
tQuaternion qconj(tQuaternion a);
void qnormalize(tQuaternion * a);
float qdot(tQuaternion a, tQuaternion b);
tQuaternion qmult(tQuaternion a, float b);


#endif /* MATHLIB_H_ */
