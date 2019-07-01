/**
 * @file
 * @brief Defines the globally available interruption handlers for all
 * translation units which include this header.
 * @author Davi Antônio da Silva Santos
 */
#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include <msp430.h>

/**
 * @brief Receives data from USCIAB0 in UART mode.
 * The maskable interruptions will be turned off while this handler is
 * executing.It will record each character sent through UART on USCIAB0 in the
 * raw receiver buffer #rx_data_raw and it will call the validation function
 * #validate_str when a null terminator is received or when the buffer has
 * reached its limit.
 * @return Void.
 */
void __attribute__ ((interrupt(USCIAB0RX_VECTOR))) received_data_ISR (void);

/**
 * @brief Detects edges on the configured pins on the Port1.
 * The endstops are connected in the PORT1 and are configured to trigger an
 * interruption through this ISR. It will deactivate all the maskable
 * interruptions while it is being executed. When an interruption is detected
 * the motors are stopped by clearing the TIMERA1 registers and blocking the
 * generation of the PWM signal.
 * @return Void.
 */
void __attribute__ ((interrupt(PORT1_VECTOR))) port1_ISR (void);

#endif
