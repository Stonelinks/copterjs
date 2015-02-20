/*
 * integrator.c
 *
 *  Created on: Oct 21, 2013
 *      Author: bmichini
 */

#include "control.h"

void Int_init(tIntegrator * integ, float min_withgain, float max_withgain, float K){
	integ->K = K;
	integ->min = min_withgain / K;
	integ->max = max_withgain / K;
	integ->value = 0;
}
void Int_increment(tIntegrator * integ,  float val, float dt){
	integ->value += val*dt;
	integ->value = satf(integ->value, integ->min, integ->max);
}
void Int_reset(tIntegrator * integ){
	integ->value = 0;
}
float Int_valgain(tIntegrator * integ){
	return integ->value * integ->K;
}



