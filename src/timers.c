#include <msp430.h>
#include "sys_config.h"
#include "timers.h"

void start_t1_a3_c0(void)
{
	/* Configure and start Timer1_A3.TA0 to generate pulses
	 * Stop the clock
	 * set PWM period as 37736*125ns*2 = 9,434 ms (106 Hz)
	 * set output mode as toggle
	 * set source as SMCLK (8 MHz, up mode, clear timer control)
	 */
	TA1CTL = MC_0;
	TA1CCR0 = PULSE_PERIOD_1RPM;
	TA1CCR1 = 0;
	TA1CCTL0 = OUTMOD_4 | CCIE;
	TA1CTL = TASSEL_2 | MC_1 | TACLR;
	P2OUT &= ~ALLEN;
}

void stop_t1_a3_c0(void)
{
	P2OUT |= ALLEN;
	TA1CTL = MC_0 | TACLR;
	TA1CCTL0 = OUTMOD_0;
	TA1CCTL0 &= ~CCIE;
	TA1CCTL0 &= ~OUT;
}
