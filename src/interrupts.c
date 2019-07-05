#include <msp430.h>
#include "sys_config.h"
#include "interrupts.h"
#include "usart.h"
#include "timers.h"
#include "sys_control.h"

void __attribute__ ((interrupt(USCIAB0RX_VECTOR))) received_data_ISR (void)
{
	/* deactivate interruptions */
	//__bic_SR_register(GIE);

	volatile char c;
	/** Automatically initialized as zero */
	static int i;

	/* Store character in buffer */
	c = UCA0RXBUF;
	rx_data_raw[i] = c;

	/*
	 * Check if any transmission is still happening 
	 * Echo received character
	 */
	while (!(IFG2 & UCA0TXIFG));
	UCA0TXBUF = UCA0RXBUF;
	/* Wait while UART is transmitting the byte */
	while (UCA0STAT & UCBUSY);

	/*
	 * if counter is greater than maximum size or '\0' was
	 * sent, allow the rx buffer to be validated. Otherwise
	 * it will increment the counter in order to fill the 
	 * next byte
	 */
	if ((i >= RX_STR_SIZE) || (c == '\0') || (c == ';') || (c == '*')
		|| (c == '(')) {
		i = 0;
		eval_command();
	} else {
		i++;
	}

	/* activate interruptions */
	//__bis_SR_register_on_exit(GIE);
}

void __attribute__ ((interrupt(PORT1_VECTOR))) port1_ISR (void)
{
	/* deactivate interruptions */
	//__bic_SR_register(GIE);

	/* Endstop sensor was triggered, kill the motors */
	stop_t1_a3_c0();

	curr_status.end_p_triggd = (P1IFG & SW0);
	curr_status.end_n_triggd = (P1IFG & SW1);

	curr_status.calibrated = 0;
	req_status.error = 1;
	curr_status.error = 1;

	if (curr_status.end_p_triggd)
		send_string("\r\nEP H\r\n");

	if (curr_status.end_n_triggd)
		send_string("\r\nEN H\r\n");

	P1IFG = 0;

	/* activate interruptions */
	//__bis_SR_register_on_exit(GIE);
}
