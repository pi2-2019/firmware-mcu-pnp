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
/** X+,Y+,Z+ endstop inputs (P1.0) */
#define SW0 (BIT0)
/** X-,Y-,Z- endstop inputs (P1.3) */
#define SW1 (BIT3)
/** All endstop inputs (P1.0 and P1.3) */
#define ALLSW (SW0 | SW1)

/* UART */
/** UCA0 RX pin (P1.1) */
#define UCA0RX (BIT1)
/** UCA0 TX pin (P1.2) */
#define UCA0TX (BIT2)

/* Steps outputs */
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

/* Directions outputs */
/** X axis direction (P1.4). */
#define DIR_X (BIT4)
/** Y axis direction (P1.5). */
#define DIR_Y (BIT5)
/** Z axis direction (P1.6). */
#define DIR_Z (BIT6)
/** Z axis rotation direction (P2.5). */
#define DIR_RZ (BIT5)
/** Solder piston direction (P2.6). */
#define DIR_S (BIT6)

/* Global enable output */
/** Global driver enable (P1.7). Enables drivers when set to zero. */
#define ENABLE (BIT7)

/* Vacuum enable output */
/** Vacuum enable (P2.7). Enables vacuum when one, disables when zero. */
#define VACUUM (BIT7)

/* Output groups selection */
/** Select all steps outputs (P2). */
#define ALLSTEPS (STEPS_X | STEPS_Y | STEPS_Z | STEPS_RZ | STEPS_S)

/* String buffers sizes in bytes */
/** Size in bytes (characters) for the received string */
#define RX_STR_SIZE (64)
/** Size in bytes (characters) for the transmitted string */
#define TX_STR_SIZE (64)

/* Helper macros for STEPS outputs */
#define SET_STEPS_X (P2OUT |= STEPS_X)
#define RESET_STEPS_X (P2OUT &= ~STEPS_X)
#define TOGGLE_STEPS_X (P2OUT ^= STEPS_X)

#define SET_STEPS_Y (P2OUT |= STEPS_Y)
#define RESET_STEPS_Y (P2OUT &= ~STEPS_Y)
#define TOGGLE_STEPS_Y (P2OUT ^= STEPS_Y)

#define SET_STEPS_Z (P2OUT |= STEPS_Z)
#define RESET_STEPS_Z (P2OUT &= ~STEPS_Z)
#define TOGGLE_STEPS_Z (P2OUT ^= STEPS_Z)

#define SET_STEPS_RZ (P2OUT |= STEPS_RZ)
#define RESET_STEPS_RZ (P2OUT &= ~STEPS_RZ)
#define TOGGLE_STEPS_RZ (P2OUT ^= STEPS_RZ)

#define SET_STEPS_S (P2OUT |= STEPS_S)
#define RESET_STEPS_S (P2OUT &= ~STEPS_S)
#define TOGGLE_STEPS_S (P2OUT ^= STEPS_S)

/* Helper macros for DIRs outputs */
#define SET_DIR_X (P1OUT |= DIR_X)
#define RESET_DIR_X (P1OUT &= ~DIR_X)
#define TOGGLE_DIR_X (P1OUT ^= DIR_X)

#define SET_DIR_Y (P1OUT |= DIR_Y)
#define RESET_DIR_Y (P1OUT &= ~DIR_Y)
#define TOGGLE_DIR_Y (P1OUT ^= DIR_Y)

#define SET_DIR_Z (P1OUT |= DIR_Z)
#define RESET_DIR_Z (P1OUT &= ~DIR_Z)
#define TOGGLE_DIR_Z (P1OUT ^= DIR_Z)

#define SET_DIR_RZ (P2OUT |= DIR_RZ)
#define RESET_DIR_RZ (P2OUT &= ~DIR_RZ)
#define TOGGLE_DIR_RZ (P2OUT ^= DIR_RZ)

#define SET_DIR_S (P2OUT |= DIR_S)
#define RESET_DIR_S (P2OUT &= ~DIR_S)
#define TOGGLE_DIR_S (P2OUT ^= DIR_S)

/* Helper macros for global enable output */
#define SET_ENABLE (P1OUT |= ENABLE)
#define RESET_ENABLE (P1OUT &= ~ENABLE)
#define TOGGLE_ENABLE (P1OUT ^= ENABLE)

/* Helper macros for vacuum enable output */
#define SET_VACUUM (P2OUT |= VACUUM)
#define RESET_VACUUM (P2OUT &= ~VACUUM)
#define TOGGLE_VACUUM (P2OUT ^= VACUUM)

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
 * In order to work at 8 kgf*cm the motors may run, considering the best case
 * torque loss:
 *
 * At 37 RPM:
 * (1/32 step) 106*37 ~ 3922 Hz (255 us)
 *
 * With SMCLK at 8 MHz and output mode 4 (toggle) 
 * set PWM period as 1020*125ns*2 = 255 us
 */
#define MIN_PULSE_PERIOD_XDIR (1020-1)

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
