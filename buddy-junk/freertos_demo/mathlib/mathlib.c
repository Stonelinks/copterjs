/*
 * mathlib.c
 *
 *  Created on: Oct 17, 2013
 *      Author: bmichini
 */



float satf( float in, float min, float max){
	if( in < min ){
		return min;
	}else if(in > max){
		return max;
	}else{
		return in;
	}
}
