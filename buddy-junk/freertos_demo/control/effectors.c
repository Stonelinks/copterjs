/*
 * effectors.c
 *
 *  Created on: Sep 27, 2013
 *      Author: bmichini
 */

#include "control.h"

extern tRCinput RCin;
// Sets up four-channel PWM with:
// Effector1: PD0, Wide timer 2, CCP 0
// Effector2: PD0, Wide timer 2, CCP 1
// Effector3: PD0, Wide timer 3, CCP 0
// Effector4: PD0, Wide timer 3, CCP 1
float PWM_TICKS_PER_MS;
void EffectorConfigure_QuadPWM(){
    // Enable the GPIO Port and Timer for each PWM
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_WTIMER2);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_WTIMER3);

    ROM_GPIOPinConfigure(GPIO_PD0_WT2CCP0);
    ROM_GPIOPinConfigure(GPIO_PD1_WT2CCP1);
    ROM_GPIOPinConfigure(GPIO_PD2_WT3CCP0);
    ROM_GPIOPinConfigure(GPIO_PD3_WT3CCP1);
    ROM_GPIOPinTypeTimer(GPIO_PORTD_BASE, (GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3));
    ROM_GPIOPadConfigSet(GPIO_PORTD_BASE, GPIO_PIN_0, GPIO_STRENGTH_8MA_SC, GPIO_PIN_TYPE_STD);
    ROM_GPIOPadConfigSet(GPIO_PORTD_BASE, GPIO_PIN_1, GPIO_STRENGTH_8MA_SC, GPIO_PIN_TYPE_STD);
    ROM_GPIOPadConfigSet(GPIO_PORTD_BASE, GPIO_PIN_2, GPIO_STRENGTH_8MA_SC, GPIO_PIN_TYPE_STD);
    ROM_GPIOPadConfigSet(GPIO_PORTD_BASE, GPIO_PIN_3, GPIO_STRENGTH_8MA_SC, GPIO_PIN_TYPE_STD);

    ROM_TimerConfigure( WTIMER2_BASE, (TIMER_CFG_SPLIT_PAIR|TIMER_CFG_A_PWM|TIMER_CFG_B_PWM) );
    ROM_TimerConfigure( WTIMER3_BASE, (TIMER_CFG_SPLIT_PAIR|TIMER_CFG_A_PWM|TIMER_CFG_B_PWM) );
    ROM_TimerControlLevel(WTIMER2_BASE, TIMER_BOTH, true);
    ROM_TimerControlLevel(WTIMER3_BASE, TIMER_BOTH, true);
    ROM_TimerLoadSet(WTIMER2_BASE, TIMER_BOTH, SysCtlClockGet() / PWM_FREQUENCY);
    ROM_TimerLoadSet(WTIMER3_BASE, TIMER_BOTH, SysCtlClockGet() / PWM_FREQUENCY);
    PWM_TICKS_PER_MS = ((float)SysCtlClockGet()) / 1000.0;

    ROM_TimerMatchSet(WTIMER2_BASE, TIMER_BOTH, (long int) PWM_TICKS_PER_MS );
    ROM_TimerMatchSet(WTIMER3_BASE, TIMER_BOTH, (long int) PWM_TICKS_PER_MS );

    ROM_TimerEnable(WTIMER2_BASE, TIMER_BOTH);
    ROM_TimerEnable(WTIMER3_BASE, TIMER_BOTH);
}

float m1,m2,m3,m4;
void EffectorSet_QuadPWM( tEffectorSetpoint *effectors){

	if( effectors->kill ){
		m1 = m2 = m3 = m4 = 0.0;
	}else{

#ifdef BUDDYQUAD
		m1 = satf( effectors->thrust - effectors->pitch - effectors->roll - effectors->yaw, 0.1, 1.0);
		m2 = satf( effectors->thrust + effectors->pitch + effectors->roll - effectors->yaw,  0.1, 1.0);
		m3 = satf( effectors->thrust - effectors->pitch + effectors->roll + effectors->yaw, 0.1, 1.0);
		m4 = satf( effectors->thrust + effectors->pitch - effectors->roll + effectors->yaw,  0.1, 1.0);
#endif

#ifdef MINIQUAD
		m1 = satf( effectors->thrust + effectors->pitch - effectors->yaw, 0.1, 1.0);
		m2 = satf( effectors->thrust - effectors->roll + effectors->yaw,  0.1, 1.0);
		m3 = satf( effectors->thrust - effectors->pitch - effectors->yaw, 0.1, 1.0);
		m4 = satf( effectors->thrust + effectors->roll + effectors->yaw,  0.1, 1.0);
#endif

	}

	ROM_TimerMatchSet(WTIMER2_BASE, TIMER_A, (long int) (PWM_TICKS_PER_MS + PWM_TICKS_PER_MS * m1));
	ROM_TimerMatchSet(WTIMER2_BASE, TIMER_B, (long int) (PWM_TICKS_PER_MS + PWM_TICKS_PER_MS * m2));
	ROM_TimerMatchSet(WTIMER3_BASE, TIMER_A, (long int) (PWM_TICKS_PER_MS + PWM_TICKS_PER_MS * m3));
	ROM_TimerMatchSet(WTIMER3_BASE, TIMER_B, (long int) (PWM_TICKS_PER_MS + PWM_TICKS_PER_MS * m4));

}
