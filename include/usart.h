/**
 * @file
 * @brief Defines the globally available USART functions for configuring the
 * peripherals, validating and transmiting data for all translation units which
 * include this header.
 * @author Davi Antônio da Silva Santos
 */

#ifndef USART_H
#define USART_H

/**
 * @brief Configures USCIAB0 in UART mode with 9600 bps, 8 bits, 1 stop bit and
 * with the RX interruption.
 * System clock is supposed to be 8 MHz, according to the configurations made in
 * #initial_setup.
 * @return Void.
 */
void config_uart_usart0(void);

/**
 * @brief Sends one byte through USCIAB0 using UART.
 * @return Void.
 */
void send_char(char c);

/**
 * @brief Sends one string though USCIAB0 using UART. It depends internally on
 * #send_char.
 * @return Void.
 */
void send_string(char *str);

/**
 * @brief Validates the buffer #rx_data_raw.
 *
 * The validation verifies wether #rx_data_raw is a valid C string and checks if
 * the issued steps are greater than zero and smaller then the maximum range of
 * one unsigned integer (16 bits in this MCU). 
 * @return Void.
 */
void validate_str(void);

#endif
