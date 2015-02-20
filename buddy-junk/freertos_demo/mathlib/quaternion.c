/*
 * quaternion.c
 *
 *  Created on: Sep 26, 2013
 *      Author: bmichini
 */

#include "mathlib.h"

void q2Euler(tQuaternion * q, float * roll, float * pitch, float * yaw, tBoolean deg)
{

	(*roll)	= atan2f(2*(q->o*q->x+q->y*q->z) , 1-2*(q->x*q->x+q->y*q->y));
	(*pitch)	= asinf(2*(q->o*q->y-q->z*q->x));
	(*yaw)	= atan2f(2*(q->o*q->z+q->x*q->y) , 1-2*(q->y*q->y+q->z*q->z));

	if(deg){
		(*roll)  *= 57.296;
		(*pitch) *= 57.296;
		(*yaw)   *= 57.296;
	}
}

tQuaternion qprod(tQuaternion a, tQuaternion b)
{
    tQuaternion result;
    result.o = a.o*b.o - a.x*b.x - a.y*b.y - a.z*b.z;
    result.x = a.o*b.x + a.x*b.o + a.y*b.z - a.z*b.y;
    result.y = a.o*b.y - a.x*b.z + a.y*b.o + a.z*b.x;
    result.z = a.o*b.z + a.x*b.y - a.y*b.x + a.z*b.o;
    return result;
}

void qcorrect( tQuaternion * est, tQuaternion * meas, float K, float sat){
	est->o -= satf(est->o - meas->o, -sat, sat)*K;
	est->x -= satf(est->x - meas->x, -sat, sat)*K;
	est->y -= satf(est->y - meas->y, -sat, sat)*K;
	est->z -= satf(est->z - meas->z, -sat, sat)*K;
}

void qpropagate(tQuaternion * a, float p, float q, float r, float dt){
	a->o -=  ( a->x*p +  a->y*q +  a->z*r ) * dt * 0.5;
	a->x +=  ( a->o*p -  a->z*q +  a->y*r ) * dt * 0.5;
	a->y +=  ( a->z*p +  a->o*q -  a->x*r ) * dt * 0.5;
	a->z +=  ( a->x*q -  a->y*p +  a->o*r ) * dt * 0.5;
}

tQuaternion qunit(){
	tQuaternion q;
	q.o = 1.0;
	q.x = 0.0;
	q.y = 0.0;
	q.z = 0.0;
	return q;
}

//tQuaternion qprodconj(tQuaternion a_conj, tQuaternion b){
//    tQuaternion result;
//    a_conj.x = -a_conj.x;
//    a_conj.y = -a_conj.y;
//    a_conj.z = -a_conj.z;
//    result = qprod(a_conj,b);
//    return result;
//}

tQuaternion qconj(tQuaternion a)
{
	a.x = -a.x;
	a.y = -a.y;
	a.z = -a.z;
	return a;
}

void qnormalize(tQuaternion * a){
	float den = 1.0/sqrtf(qdot(*a,*a));//a->o*a->o + a->x*a->x + a->y*a->y + a->z*a->z);
	if (den > 0.001)
	{
		a->o *= den;
		a->x *= den;
		a->y *= den;
		a->z *= den;
	}
}

float qdot(tQuaternion a, tQuaternion b){
	return a.o*b.o + a.x*b.x + a.y*b.y + a.z*b.z;
}

tQuaternion qmult(tQuaternion a, float b){
	a.o *= b;
	a.x *= b;
	a.y *= b;
	a.z *= b;
	return a;
}
