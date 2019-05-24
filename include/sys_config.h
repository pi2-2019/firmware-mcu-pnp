/**
 * @file
 * @brief Defines the globally available macros, variables and routines.
 * This includes the GPIO mappings and othe system configurations.
 * @author Davi Antônio da Silva Santos
 */

#ifndef SYS_CONFIG_H
#define SYS_CONFIG_H

#include <msp430.h>

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
 * scenario for torque loss is -0,2 kgf*cm/RPM and the average between 0 and 700
 * RPM is roughly -0,027 kgf*cm/RPM.
 *
 * At 1 RPM:
 * 200 pulses/60 seconds ~ 3 Hz pulses
 * (1/32 step) 6400/60 ~ 106 Hz pulses
 *
 * With SMCLK at 8 MHz and output mode 4 (toggle) 
 * set PWM period as 37736*125ns*2 = 9,434 ms (106 Hz)
 */
#define PULSE_PERIOD_1RPM (37736-1)

/*
 * For the 24 kgf*cm motors in the Y direction
 * Simulation shows the minimum torque required is 9,4 kgf*cm.
 * In order to work at 12 kgf*cm the motors may run, considering the average
 * torque loss:
 *
 * At 444 RPM:
 * (1/32 step) 106*444 ~ 47064 Hz (21,25 us)
 *
 * With SMCLK at 8 MHz and output mode 4 (toggle) 
 * set PWM period as 85*125ns*2 = 21,25 us
 */
#define MIN_PULSE_PERIOD_XDIR (85-1)

/*
 * For the 9 kgf*cm motors in the X direction
 * Simulation shows the minimum torque required is 7 kgf*cm.
 * In order to work at 8 kgf*cm the motors may run, considering the worse case
 * torque loss:
 *
 * At 5 RPM:
 * (1/32 step) 106*5 ~ 530 Hz (1,887 ms)
 *
 * With SMCLK at 8 MHz and output mode 4 (toggle) 
 * set PWM period as 7547*125ns*2 = 1,887 ms
 */
#define MIN_PULSE_PERIOD_YDIR (7547-1)

/*
 * For the 4,2 kgf*cm motors in the Z direction
 * Simulation shows the minimum torque required is 2 kgf*cm.
 * In order to work at 2,5 kgf*cm the motors may run, considering the worse case
 * torque loss:
 *
 * At 8 RPM:
 * (1/32 step) 106*8 ~ 848 Hz (1,179 ms)
 *
 * With SMCLK at 8 MHz and output mode 4 (toggle) 
 * set PWM period as 4717*125ns*2 = 1,179 ms
 */
#define MIN_PULSE_PERIOD_ZDIR (4717-1)

#define MIN_PULSE_PERIOD_SOLDER (37736-1)
#define MIN_PULSE_PERIOD_ROT (37736-1)

/* Global vars */
/** Buffer to store raw data received by the UART */
char rx_data_raw[RX_STR_SIZE];
/** Variable to store the requires steps/usteps for the desired movement */
unsigned long int req_steps;

/**
 * @brief Initialises the system.
 *
 * System clock is set to 8 MHz using the internal oscillators.
 * @return Void.
 */
void initial_setup(void);

#endif
