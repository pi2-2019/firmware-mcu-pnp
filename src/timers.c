#include <msp430.h>
#include "sys_config.h"
#include "timers.h"

void start_t1_a3_c0_it(unsigned int period)
{
	/* Configure and start Timer1_A3.TA0 to generate pulses
	 * Stop the clock
	 * set interrupts period
	 * set source as SMCLK (8 MHz, up mode, clear timer control)
	 */
	TA1CTL = MC_0;
	TA1CCR0 = period;
	TA1CCR1 = 0;
	TA1CCTL0 = CCIE;
	TA1CTL = TASSEL_2 | MC_1 | TACLR;
}

void start_t1_a3_c0(unsigned int period)
{
	/* Configure and start Timer1_A3.TA0 to generate pulses
	 * Stop the clock
	 * set interrupts period
	 * set source as SMCLK (8 MHz, up mode, clear timer control)
	 */
	TA1CTL = MC_0;
	TA1CCR0 = period;
	TA1CCR1 = 0;
	TA1CTL = TASSEL_2 | MC_1 | TACLR;
}

void stop_t1_a3_c0(void)
{
	TA1CTL = MC_0 | TACLR;
	TA1CCTL0 &= ~CCIE;
}
