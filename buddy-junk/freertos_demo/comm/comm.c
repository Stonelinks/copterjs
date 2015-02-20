#include "comm.h"

void init_comm(){

	I2C1_init(true); // Initialize I2C1 in fast mode (400 kbps)
	UART0_init(); // Initialize UART0 (debug serial output)
	UART5_init(); // Initialize UART5 (sbus input)

}

// Printing util
//*****************************************************************************
//
//! \brief Float to ASCII
//!
//! Converts a floating point number to ASCII. Note that buf must be
//! large enough to hold
//!
//! \param f is the floating point number.
//! \param buf is the buffer in which the resulting string is placed.
//! \return None.
//!
//! \par Example:
//! ftoa(3.14) returns "3.14"
//!
//
//*****************************************************************************
char buf[8];
char* ftoa(float f)
{
    int pos=0,ix,dp,num;
    if (f<0)
    {
        buf[pos++]='-';
        f = -f;
    }
    dp=0;
    while (f>=10.0)
    {
        f=f/10.0;
        dp++;
    }
    for (ix=1;ix<4;ix++)
    {
            num = (int)f;
            f=f-num;
            if (num>9)
                buf[pos++]='#';
            else
                buf[pos++]='0'+num;
            if (dp==0) buf[pos++]='.';
            f=f*10.0;
            dp--;
    }
    return buf;
}
