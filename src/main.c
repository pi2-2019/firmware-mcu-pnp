#include <msp430.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include <string.h>

/* Endstop inputs */
#define SW0_P1 (BIT0)
#define SW1_P1 (BIT3)
#define SW2_P1 (BIT4)
#define SW3_P1 (BIT5)
#define SW4_P1 (BIT6)
#define SW5_P1 (BIT7)
#define ALLSW (SW0_P1 | SW1_P1 | SW2_P1 | SW3_P1 | SW4_P1 | SW5_P1)

/* UART */
#define UCA0RX_P1 (BIT1)
#define UCA0TX_P1 (BIT2)

/* Timer A3 CCI0 */
#define STEPS_P2 (BIT0)

/* Outputs */
#define DIR_P2 (BIT1)

#define EN0_P2 (BIT2)
#define EN1_P2 (BIT3)
#define EN2_P2 (BIT4)
#define EN3_P2 (BIT5)
#define EN4_P2 (BIT6)
#define ALLEN (EN0_P2 | EN1_P2 | EN2_P2 | EN3_P2 | EN4_P2)

#define RX_STR_SIZE (32)
#define TX_STR_SIZE (64)

/*
 * Motor rotation speed in pulses per second
 * According to the WS23-0240-20-4 stepper motor datasheet, the worse case
 * scenario for torque loss is -0,2 kgf*cm/RPM
 * At 1 RPM (for test only)
 * 200 pulses/60 seconds ~ 3 Hz pulses
 * (1/32 step) 6400/60 ~ 106 Hz pulses
 * With SMCLK at 8 MHz and output mode 4 (toggle) 
 * set PWM period as 37736*125ns*2 = 9,434 ms (106 Hz)
 */
#define PULSE_PERIOD (37736-1)

/* Global vars */
char rx_data_raw[RX_STR_SIZE];
unsigned long int req_steps = 0;

int main(void)
{
	/* Stop watchdog timer */
	WDTCTL = WDTPW | WDTHOLD;
	
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
	
	/* Configure UART */
	/* 
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
	
	__bis_SR_register(GIE);
	
	while(1);

	return 0;
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
		 *
		 * Configure and start Timer1_A3.TA0 to generate pulses
		 * Stop the clock
		 * set PWM period as 37736*125ns*2 = 9,434 ms (106 Hz)
		 * set output mode as toggle
		 * set source as SMCLK (8 MHz, up mode, clear timer control)
		 */
		TA1CTL = MC_0;
		TA1CCR0 = PULSE_PERIOD;
		TA1CCR1 = 0;
		TA1CCTL0 = OUTMOD_4 | CCIE;
		TA1CTL = TASSEL_2 | MC_1 | TACLR;
		P2OUT &= ~ALLEN;
	} else {
		req_steps = 0;
		send_string("\r\nBad setpoint\r\n");
		memset(rx_data_raw, 0, RX_STR_SIZE);
	}
}

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

void __attribute__ ((interrupt(PORT1_VECTOR))) Port_1 (void)
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
	P2OUT |= ALLEN;
	TA1CTL = MC_0 | TACLR;
	TA1CCTL0 = OUTMOD_0;
	TA1CCTL0 &= ~CCIE;
	TA1CCTL0 &= ~OUT;
	
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
		P2OUT |= ALLEN;
		TA1CTL = MC_0 | TACLR;
		TA1CCTL0 = OUTMOD_0;
		TA1CCTL0 &= ~CCIE;
		TA1CCTL0 &= ~OUT;
	}
	
}
