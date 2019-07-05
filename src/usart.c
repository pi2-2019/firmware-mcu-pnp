#include <msp430.h>
#include <stdlib.h>
#include <float.h>
#include <string.h>
#include <ctype.h>

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

void print_float(float f)
{
	/** Integer part from float number */
	int integer;
	/** Position counter for array limits */
	int pos;
	/** Size in bytes for the integer part */
	int size;
	/** Temporary pointer to the last position of #tx_data_raw */
	char *c;
	
	memset(tx_data_raw, 0, TX_STR_SIZE);
	
	integer = f;
	
	/* snprintf(tx_data_raw, TX_STR_SIZE, "%d", integer); */
	itoa(integer, tx_data_raw, 10);
	
	/* Fix subtraction for negative numbers */
	if (f < 0) {
		f *= -1;
		integer *= -1;
	}	
	
	/* Get the last position */
	pos = strlen(tx_data_raw);
	/* Store the current size */
	size = pos;
	
	tx_data_raw[pos++] = '.';
	
	while ((pos < size + FLT_DIG + 1) && (pos < TX_STR_SIZE)) {
		f = f - (float) integer;
		f *= (float) 10;
		integer = (int) f;
		c = &tx_data_raw[pos];
		/* snprintf(c, 2, "%d", integer); */
		itoa(integer, c, 10);
		pos++;
	}	
	
	send_string(tx_data_raw);
}

float parse_param(char c, float dft_ret)
{
	/** Pointer to buffer to be read. Used to parse arguments */
	char *tmp_str = NULL;
	/** Number found */
	float num = 0;
	/** Expoent */
	float expo = 0;
	/** Temporary counter */
	int i = 0;
	/** Used to flip the signal after the parsing */
	char negative = 0;
	
	if (rx_data_raw[0] == '\0')
		return dft_ret;
	
	/* Seek for the desired char */
	tmp_str = strchr(rx_data_raw, c);
	
	/* If the char was not found, return Not a Number as an error */
	if (tmp_str == NULL)
		return dft_ret;
	/*
	 * If the number was found, increment to advance to the next char after
	 * its place.
	 */
	
	tmp_str++;
	
	if (*tmp_str == '+') {
		negative = 0;
		tmp_str++;
	} else if (*tmp_str == '-') {
		negative = 1;
		tmp_str++;
	} else if (*tmp_str == '\0') {
		return dft_ret;
	}
	
	/* If the number overflows or no number is read return an error */ 	
	while ((*tmp_str) && (i < RX_STR_SIZE - 3)) {
		if (isdigit(*tmp_str)) {
			num = 10.0 * num + (float)(*tmp_str - '0');
			expo *= 10.0;
		} else if (*tmp_str == '.') {
			expo = 1.0;
		}else if(isspace(*tmp_str) || (*tmp_str == ';')
			|| (*tmp_str == '*') || (*tmp_str == '(')) {
			break;
		} else {
			send_string("PARSE?\r\n");
			memset(rx_data_raw, 0, RX_STR_SIZE);
			return dft_ret;
		}
		tmp_str++;
		i++;
	}
	num = num / ((expo == 0.0) ? 1.0 : expo);
	num = (negative ? -num : num);
	
	return num;
}
