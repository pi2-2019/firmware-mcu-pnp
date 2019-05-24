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
	P1SEL |= UCA0RX_P1;
	P1SEL2 |= UCA0RX_P1;
	P1SEL |= UCA0TX_P1;
	P1SEL2 |= UCA0TX_P1;
	
	/*
	 * P1.0, P1.3, P1.4, P1.5, P1.6, P1.7 are pull-up inputs for SW_0, SW_1,
	 * SW_2, SW_3, SW_4 and SW_5
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
	
	/* Set P2.0 as Timer1_A3.TA0 output */
	P2DIR |= STEPS_P2;
	P2SEL |= STEPS_P2;
	P2SEL2 &= ~STEPS_P2;
	
	/*
	 * P2.1 is the direction output
	 * P2.2, P2.3, P2.4, P2.5 and P2.6 are EN_0 (X0 and X1), EN_1 (Y),
	 * EN_2 (Z), EN_3 (ZROT), EN_4 (SOLDER).
	 * High logic level disables the drivers (ENABLE' pin)
	 */
	P2DIR |= DIR_P2;
	P2OUT &= ~DIR_P2;
	P2DIR |= ALLEN;
	P2OUT |= ALLEN;
}
