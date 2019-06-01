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
/** SW0 endstop input (P1.0) */
#define SW0 (BIT0)
/** SW1 endstop input (P1.3) */
#define SW1 (BIT3)
/** SW2 endstop input (P1.4) */
#define SW2 (BIT4)
/** SW3 endstop input (P1.5) */
#define SW3 (BIT5)
/** SW4 endstop input (P1.6) */
#define SW4 (BIT6)
/** SW5 endstop input (P1.7) */
#define SW5 (BIT7)
/** All endstop inputs (P1) */
#define ALLSW (SW0 | SW1 | SW2 | SW3 | SW4 | SW5)

/* UART */
/** UCA0 RX pin (P1.1) */
#define UCA0RX (BIT1)
/** UCA0 TX pin (P1.2) */
#define UCA0TX (BIT2)

/* Outputs */
/** X axis steps output (P2.0) */
#define STEPS_X (BIT0)
/** Y axis steps output (P2.1) */
#define STEPS_Y (BIT1)
/** Z axis steps output (P2.2) */
#define STEPS_Z (BIT2)
/** Z rotation driver steps output (P2.3) */
#define STEPS_RZ (BIT3)
/** Solder driver steps output (P2.4) */
#define STEPS_S (BIT4)
/** Global motor direction (P2.5). */
#define DIR (BIT5)
/** Global driver enable (P2.6). Enables drivers when set to zero. */
#define ENABLE (BIT6)
/** Vacuum enable (P2.7). Enables vacuum when one, disables when zero. */
#define VACUUM (BIT7)
/** Select all steps outputs (P2). */
#define ALLSTEPS (STEPS_X | STEPS_Y | STEPS_Z | STEPS_RZ | STEPS_S)
/** Select all outputs (P2).*/
#define ALLOUTS (ALLSTEPS | DIR | ENABLE | VACUUM)
/** Select all outputs but the global #ENABLE (P2).*/
#define CMNOUTS (ALLSTEPS | DIR | VACUUM)

/** Size in bytes (characters) for the received string */
#define RX_STR_SIZE (64)
/** Size in bytes (characters) for the transmitted string */
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
#define MIN_PULSE_PERIOD_YDIR (85-1)

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
#define MIN_PULSE_PERIOD_XDIR (7547-1)

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
