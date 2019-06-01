#include <msp430.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include <string.h>

#include "usart.h"
#include "sys_config.h"
#include "timers.h"

void config_uart_usart0(void)
{
	/* 
	 * Configure UART
	 *
	 * Select SMCLK (8 MHz)
	 * Adjust clk division to 9600 baud (LaunchPad maximum)
	 * Adjust modulation to fine tune the baud rate
	 * Initialize USCI
	 * Enable RX interruptions
	 * SLAU144J expected error: max TX (-0,4% 0%) max rx (-0,4% 0,1%)
	 */
	UCA0CTL1 |= UCSWRST;
	UCA0CTL0 = 0;
	UCA0CTL1 |= UCSSEL_2;
	UCA0BR0 = 52;
	UCA0BR1 = 0;
	UCA0MCTL = UCBRS_0 | UCOS16 | UCBRF_1;
	UCA0CTL1 &= ~UCSWRST;
	IFG2 &= ~(UCA0RXIFG);
	IE2 |= UCA0RXIE;
}

void send_char(char c)
{
	/* Wait while something is transmitted */
	while (!(IFG2 & UCA0TXIFG));
	
	UCA0TXBUF = c;
	
	/* Wait while UART is transmitting the byte */
	while (UCA0STAT & UCBUSY);
}
 
void send_string(char *str)
{	
	volatile int i;
	for(i = 0; (str[i] != '\0') && (i < TX_STR_SIZE); i++)
		send_char(str[i]);
}

void validate_str(void)
{
	long int t;
	
	/* change setpoint only if it is valid
	 * Check if is valid (ends in \0, doens't begin with \0)
	 * Check if all chars are nums (skip \0)
	 * Check if it won't overflow
	 * Do not accept 0 steps requests
	 */
	errno = 0;
	t = strtol(rx_data_raw, NULL, 10);
	if ((rx_data_raw != NULL) && (rx_data_raw[0] != '\0')
	&& (errno != ERANGE) && (t <= UINT_MAX)	&& (t > 0)) {
		req_steps = (unsigned int) t;
		
		/*
		 * If data is valid, configure timer and send the required
		 * pulses. The interruption will stop the Timer when the number
		 * of steps is reached.
		 */
		start_t1_a3_c0(PULSE_PERIOD_1RPM);
	} else {
		req_steps = 0;
		send_string("\r\nBad setpoint\r\n");
		memset(rx_data_raw, 0, RX_STR_SIZE);
	}
}
