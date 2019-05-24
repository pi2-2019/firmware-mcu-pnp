#include <msp430.h>
#include "sys_config.h"
#include "interrupts.h"
#include "usart.h"
#include "timers.h"

void __attribute__ ((interrupt(USCIAB0RX_VECTOR))) received_data_ISR (void)
{
	/* deactivate interruptions */
	__bic_SR_register(GIE);
	
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
	if ((i >= RX_STR_SIZE) || (c == '\0')) {
		i = 0;
		validate_str();
	} else {
		i++;
	}
	
	/* activate interruptions */
	__bis_SR_register_on_exit(GIE);
}

void __attribute__ ((interrupt(PORT1_VECTOR))) port1_ISR (void)
{
	/* deactivate interruptions */
	__bic_SR_register(GIE);
	
	P1IFG = 0;
	send_string("\r\nP1ISR\r\n");
	
	/*
	 * Endstop sensor was triggered, kill the motors.
	 * Stop pulses
	 * Set output to low logic level
	 */
	stop_t1_a3_c0();
	
	/* activate interruptions */
	__bis_SR_register_on_exit(GIE);
}

void __attribute__ ((interrupt(TIMER1_A0_VECTOR))) Step_Toggle (void)
{
	static unsigned int toggle_count;
	
	if (toggle_count < (2*req_steps)) {
		toggle_count++;
	} else {
		req_steps = 0;
		toggle_count = 0;
		send_string("\r\ndone\r\n");
		
		/*
		 * Stop Timer1 A3, finished counting
		 * Stop pulses
	 	 * Set output to low logic level
		 */
		stop_t1_a3_c0();
	}	
}
