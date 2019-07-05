#include <msp430.h>

#include "sys_config.h"
#include "interrupts.h"
#include "usart.h"
#include "sys_control.h"

int main(void)
{
	/* Stop watchdog timer */
	WDTCTL = WDTPW | WDTHOLD;

	initial_setup();
	config_uart_usart0();

	__bis_SR_register(GIE);

	while(1) {
		eval_command();
	}

	return 0;
}
