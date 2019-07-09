/**
 * @file
 * @brief Implements the globally available macros, variables and routines.
 * This includes the GPIO mappings and othe system configurations.
 * @author Davi Antônio da Silva Santos
 */

#include <msp430.h>
#include <string.h>
#include "sys_config.h"
#include "sys_control.h"

volatile char execute_routine = 0;

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
	 * P1.0 is the pull-up input for X-
	 * P1.3 is the pull-up input for Y-
	 * P2.6 is the pull-up input for Z-
	 * High logic level means endstops (X, Y, Z) triggered (untriggered
	 * endstops must force low level).
	 * Set rising edge interruption to watch end stop events
	 * Clear interruption flag
	 */
	P1DIR &= ~(SWX | SWY);
	P1REN |= (SWX | SWY);
	P1OUT |= (SWX | SWY);
	P1IES &= ~(SWX | SWY);
	P1IE |= (SWX | SWY);
	P1IFG = 0;

	P2DIR &= ~SWZ;
	P2SEL &= ~SWZ;
	P2SEL2 &= ~SWZ;
	P2REN |= SWZ;
	P2OUT |= SWZ;
	P2IES &= ~SWZ;
	P2IE |= SWZ;
	P2IFG = 0;

	/*
	 * Set the other pins as outputs
	 * P2.0 (Y), P2.1 (X), P1.7 (S), P1.6 (RZ), P2.5 (Z) are the STEPS
	 * outputs and will be initially set to zero.
	 *
	 * P1.4 (DIRY), P1.5 (DIRX), P2.2 (DIRZ), P2.4 (DIRSOLDER), P2.3 (DIRRZ)
	 * are DIR outputs and will be initially set to zero.
	 *
	 * P2.7 controls the vacuum pump and will be set initially as zero
	 * (vacuum on).
	 */
	P1DIR |= (STEPS_S | STEPS_RZ);
	P1OUT &= ~(STEPS_S | STEPS_RZ);
	P2DIR |= (STEPS_Y | STEPS_X | STEPS_Z);
	P2OUT &= ~(STEPS_Y | STEPS_X | STEPS_Z);
	
	P1DIR |= (DIR_X | DIR_Y);
	P1OUT &= ~(DIR_X | DIR_Y);
	P2DIR |= (DIR_RZ | DIR_S | DIR_Z);
	P2OUT &= ~(DIR_RZ | DIR_S | DIR_Z);
	
	P2DIR |= VACUUM;
	P2SEL &= ~VACUUM;
	P2SEL2 &= ~VACUUM;
	P2OUT &= ~VACUUM;
	
	/* Reset the TX and RX buffers */
	memset(rx_data_raw, 0, RX_STR_SIZE);
	memset(tx_data_raw, 0, TX_STR_SIZE);
	
	/* Initialise the control vars */
	memset(&req_status, 0, sizeof(struct status));
	memset(&curr_status, 0, sizeof(struct status));
}
