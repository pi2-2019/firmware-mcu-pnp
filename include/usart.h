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
 * @param[in] c: character to be sent.
 * @return Void.
 */
void send_char(char c);

/**
 * @brief Sends one string though USCIAB0 using UART. It depends internally on
 * #send_char.
 * @param[in] str: pointer to the string to be sent.
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

/**
 * @brief Parses numbers after a character in #rx_data_raw. Useful for parsing
 * parameters to G or M codes.
 *
 * The function searches for one character in #rx_data_raw and if it is found,
 * searches for a valid float number right after it. If the desired character is
 * not found, if the number found would overflow or underflow a float or if no
 * valid number is found after the character, return #dft_ret as an error.
 * @param[in] c: character to be found in #rx_data_raw.
 * @param[in] dft_ret: code to return on error.
 * @return the number right after #c or #dft_ret if an error occurs.
 */
float parse_param(char c, float dft_ret);

void print_float(float f);
#endif
