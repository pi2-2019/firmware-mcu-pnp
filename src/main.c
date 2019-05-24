#include <msp430.h>

#include "sys_config.h"
#include "interrupts.h"
#include "usart.h"

int main(void)
{
	/* Stop watchdog timer */
	WDTCTL = WDTPW | WDTHOLD;
	
	initial_setup();
	config_uart_usart0();
	
	__bis_SR_register(GIE);
	
	while(1);

	return 0;
}
