#include <msp430.h>
#include "sys_config.h"

void initial_setup(void)
{
	/*
	 * If calibration constant erased, trap CPU
	 * Select lowest DCOx and MODx and then set DCO frequency
	 * Will affect SMCLK and MCLK frequency
	 * Set SMCLK and MCLK to 8 MHz
	 */
	if (CALBC1_8MHZ == 0xFF)
        	while(1);
	DCOCTL = 0;
	BCSCTL1 = CALBC1_8MHZ;
	DCOCTL = CALDCO_8MHZ;
	
	/* Set P1.1 as UCA0RX and P1.2 as UCA0TX */
	P1SEL |= UCA0RX;
	P1SEL2 |= UCA0RX;
	P1SEL |= UCA0TX;
	P1SEL2 |= UCA0TX;
	
	/*
	 * P1.0, P1.3, are pull-up inputs for SW0 (X+,Y+,Z+) and SW1 (X-,Y-,Z-)
	 * High logic level means end stop triggered
	 * Set rising edge interruption to watch end stop events
	 * Clear interruption flag
	 */
	P1DIR &= ~ALLSW;
	P1REN |= ALLSW;
	P1OUT |= ALLSW;
	P1IES &= ~ALLSW;
	P1IE |= ALLSW;
	P1IFG = 0;
	
	/*
	 * Set the other pins as outputs
	 * P2.0, P2.1, P2.2, P2.3 and P2.4 are the STEPS outputs and will be
	 * initially set to zero.
	 *
	 * P1.4, P1.5, P1.6, P2.5, P2.6 are DIR outputs and will be initially
	 * set to zero
	 *
	 * P1.7 is the global driver enable pin and will be set to one in order
	 * to start the system with the drivers turned off (ENABLE' pin).
	 *
	 * P2.7 controls the vacuum pump and will be set initially as zero (no
	 * vacuum).
	 */
	P2DIR |= ALLSTEPS;
	P2OUT &= ~ALLSTEPS;
	
	P1DIR |= (DIR_X | DIR_Y| DIR_Z);
	P2DIR |= (DIR_RZ | DIR_S);
	P1OUT &= ~(DIR_X | DIR_Y| DIR_Z);
	P2OUT &= ~(DIR_RZ | DIR_S);
	
	P1DIR |= ENABLE;
	P1OUT |= ENABLE;
	
	P2DIR |= VACUUM;
	P2OUT &= ~VACUUM;
}