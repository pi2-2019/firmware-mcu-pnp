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
/** X- endstop input (P1.0) */
#define SWX (BIT0)

/** Y- endstop input (P1.3) */
#define SWY (BIT3)

/* UART */
/** UCA0 RX pin (P1.1) */
#define UCA0RX (BIT1)
/** UCA0 TX pin (P1.2) */
#define UCA0TX (BIT2)

/* Steps outputs */
/** X axis steps output (P2.0) */
#define STEPS_Y (BIT0)

/** Y axis steps output (P2.1) */
#define STEPS_X (BIT1)

/** Z axis steps output (P2.5) */
#define STEPS_Z (BIT5)

/** Z rotation driver steps output (1.6) */
#define STEPS_RZ (BIT6)

/** Solder driver steps output (P1.7) */
#define STEPS_S (BIT7)

/* Directions outputs */
/** X axis direction (P1.5). */
#define DIR_X (BIT5)

/** Y axis direction (P1.4). */
#define DIR_Y (BIT4)

/** Z axis direction (P2.2). */
#define DIR_Z (BIT2)

/** Z axis rotation direction (P2.3). */
#define DIR_RZ (BIT3)

/** Solder piston direction (P2.4). */
#define DIR_S (BIT4)

/** Z- endstop input (P2.6) */
#define SWZ (BIT6)

/* Vacuum enable output */
/** Vacuum enable (P2.7). Enables vacuum when one, disables when zero. */
#define VACUUM (BIT7)

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

#define SET_STEPS_RZ (P1OUT |= STEPS_RZ)
#define RESET_STEPS_RZ (P1OUT &= ~STEPS_RZ)
#define TOGGLE_STEPS_RZ (P1OUT ^= STEPS_RZ)

#define SET_STEPS_S (P1OUT |= STEPS_S)
#define RESET_STEPS_S (P1OUT &= ~STEPS_S)
#define TOGGLE_STEPS_S (P1OUT ^= STEPS_S)

/* Helper macros for DIRs outputs */
#define SET_DIR_X (P1OUT |= DIR_X)
#define RESET_DIR_X (P1OUT &= ~DIR_X)
#define TOGGLE_DIR_X (P1OUT ^= DIR_X)

#define SET_DIR_Y (P1OUT |= DIR_Y)
#define RESET_DIR_Y (P1OUT &= ~DIR_Y)
#define TOGGLE_DIR_Y (P1OUT ^= DIR_Y)

#define SET_DIR_Z (P2OUT |= DIR_Z)
#define RESET_DIR_Z (P2OUT &= ~DIR_Z)
#define TOGGLE_DIR_Z (P2OUT ^= DIR_Z)

#define SET_DIR_RZ (P2OUT |= DIR_RZ)
#define RESET_DIR_RZ (P2OUT &= ~DIR_RZ)
#define TOGGLE_DIR_RZ (P2OUT ^= DIR_RZ)

#define SET_DIR_S (P2OUT |= DIR_S)
#define RESET_DIR_S (P2OUT &= ~DIR_S)
#define TOGGLE_DIR_S (P2OUT ^= DIR_S)

/* DIR Inputs for DRV8825 */
#define CW (1)
#define CCW (0)

/* Helper macros for vacuum enable output */
#define SET_VACUUM (P2OUT |= VACUUM)
#define RESET_VACUUM (P2OUT &= ~VACUUM)
#define TOGGLE_VACUUM (P2OUT ^= VACUUM)

/**
 * @brief 1 RPM speed for all motors
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

/**
 * @brief Max RPM for Y axis
 * For the 24 kgf*cm motors in the Y direction
 * Simulation shows the minimum torque required is 9,4 kgf*cm.
 * In order to work at 23,4 kgf*cm the motors may run, considering the average
 * torque loss (0,6 kgf*cm):
 *
 * At 20 RPM:
 * (1/32 step) 106*20 ~ 2120 Hz (471,70 us)
 *
 * With SMCLK at 8 MHz and output mode 4 (toggle) 
 * set PWM period as 1887*125ns*2 = 471,75 us
 */
#define MIN_PULSE_PERIOD_YDIR (1887-1)

/**
 * @brief Max RPM for X axis
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

/**
 * @brief Max RPM for Z axis
 * For the 4,2 kgf*cm motors in the Z direction
 * Simulation shows the minimum torque required is 2 kgf*cm.
 * In order to work at 2,5 kgf*cm the motors may run, considering the best case
 * torque loss of 1,7 kgf*cm:
 *
 * At 62 RPM:
 * (1/32 step) 106*62 ~ 6572 Hz (152,16 us)
 *
 * With SMCLK at 8 MHz and output mode 4 (toggle) 
 * set PWM period as 608*125ns*2 = 152 us
 */
#define MIN_PULSE_PERIOD_ZDIR (608-1)

/**
 * @brief Max RPM for solder extruder
 * Same configurations as #MIN_PULSE_PERIOD_ZDIR
 */
#define MIN_PULSE_PERIOD_SOLDER (608-1)

/**
 * @brief Max RPM for C axis
 * Negligible load on the 0,22 kgf*cm motor.
 *
 * At 31 RPM:
 * (1/32 step) 106*31 ~ 3286 Hz (304 us)
 *
 * With SMCLK at 8 MHz and output mode 4 (toggle) 
 * set PWM period as 1216*125ns*2 = 304 us
 */
#define MIN_PULSE_PERIOD_ROT (1216-1)

/**
 * @brief Max RPM for XYZ calibration routine. Speed of the slowest motor.
 * 20 RPM (see #MIN_PULSE_PERIOD_YDIR)
 */
#define MIN_PULSE_CALIB_XYZ (MIN_PULSE_PERIOD_YDIR)

/* Motors' steps per mm constants */

/** @brief X axis steps per mm constant
 */
#define STEPS_PER_MM_X (324)
/** @brief Y axis steps per mm constant
 */
#define STEPS_PER_MM_Y (317)
/** @brief Z axis steps per mm constant
 */
#define STEPS_PER_MM_Z (1039)
/** @brief Solder extruder steps per mm constant
 */
#define STEPS_PER_MM_S (4267)
/** @brief C axis (RZ) steps per mm constant
 */
#define STEPS_PER_DEG_RZ (36)

/* Global vars */
/** Buffer to store raw data received by the UART */
char rx_data_raw[RX_STR_SIZE];
/** Buffer to send data though UART */
char tx_data_raw[TX_STR_SIZE];
/** Variable to store the requires steps/usteps for the desired movement */
unsigned long int req_steps;

/* Global variable to run all routines after parsing */
volatile char execute_routine;

/**
 * @brief Initialises the system.
 *
 * System clock is set to 8 MHz using the internal oscillators.
 * @return Void.
 */
void initial_setup(void);

#endif
