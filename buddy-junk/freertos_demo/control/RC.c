/*
 * RC.c
 *
 *  Created on: Oct 17, 2013
 *      Author: bmichini
 */

#include "control.h"

extern uint8_t sbusFrame[25];

tRCinput RCin;
uint16_t channels[16];


void RC_init(){
	RCin.chan_offset[0] = 0.0;
	RCin.chan_offset[1] = 0.0;
	RCin.chan_offset[2] = 0.0;
	RCin.chan_offset[3] = 0.0;
	RCin.chan[0] = 0.0;
	RCin.chan[1] = 0.0;
	RCin.chan[2] = -1.0;
	RCin.chan[3] = 0.0;
	RCin.sw[0] = false;
	RCin.sw[1] = false;
	RCin.TXlost = true;
}

void UpdateSbus( ){

	 // reset counters
	uint8_t byte_in_sbus = 1;
	uint8_t bit_in_sbus = 0;
	uint8_t ch = 0;
	uint8_t bit_in_channel = 0;
	uint8_t i;

	for (i=0; i<16; i++) {
		channels[i] = 0;
	}

	// process actual sbus data
	for (i=0; i<176; i++) {
		if (sbusFrame[byte_in_sbus] & (1<<bit_in_sbus)) {
		  channels[ch] |= (1<<bit_in_channel);
		}
		bit_in_sbus++;
		bit_in_channel++;

		if (bit_in_sbus == 8) {
			bit_in_sbus =0;
			byte_in_sbus++;
		}
		if (bit_in_channel == 11) {
			bit_in_channel =0;
			ch++;
		}
	}
	RCin.chan[0] = (((float)channels[0]) - 1024.0) / 652.0;
	RCin.chan[1] = (((float)channels[1]) - 1024.0) / -652.0;
	RCin.chan[2] = (((float)channels[2]) - 1024.0) / -652.0;
	RCin.chan[3] = (((float)channels[3]) - 1024.0) / 652.0;


	if( channels[4] > 1024 ){
		RCin.sw[0] = false;
	}else{
		RCin.sw[0] = true;
	}
	if( channels[5] > 1024 ){
		RCin.sw[1] = false;
	}else{
		RCin.sw[1] = true;
	}

	RCin.chan[0] -= RCin.chan_offset[0];
	RCin.chan[1] -= RCin.chan_offset[1];
	RCin.chan[3] -= RCin.chan_offset[3];

}

void RC_GetOffset( ){
	RCin.chan_offset[0] = RCin.chan[0] + RCin.chan_offset[0];
	RCin.chan_offset[1] = RCin.chan[1] + RCin.chan_offset[1];
	RCin.chan_offset[3] = RCin.chan[3] + RCin.chan_offset[3];
}
