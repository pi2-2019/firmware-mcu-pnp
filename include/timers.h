/**
 * @file
 * @brief Defines the timer related routines
 * @author Davi Antônio da Silva Santos
 */

#ifndef TIMERS_H
#define TIMERS_H

#include <msp430.h>

/**
 * @brief Starts the Timer A3 CCR0 and enable its interruption.
 * @return Void.
 */
void start_t1_a3_c0(void);

/**
 * @brief Stops Timer A3 CCR0 and sets the output pin to low logical level.
 * @return Void.
 */
void stop_t1_a3_c0(void);

#endif
