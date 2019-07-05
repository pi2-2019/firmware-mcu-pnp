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

/* Global enable output */
/** Global driver enable (P2.6). Enables drivers when set to zero. */
#define ENABLE (BIT6)

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

#define CW (1)
#define CCW (0)

/* Helper macros for global enable output */
#define SET_ENABLE (P2OUT |= ENABLE)
#define RESET_ENABLE (P2OUT &= ~ENABLE)
#define TOGGLE_ENABLE (P2OUT ^= ENABLE)

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
 * set PWM period as 608*125ns*2 = 6,5789 ms
 */
#define MIN_PULSE_PERIOD_ZDIR (608-1)

/**
 * @brief Max RPM for solder extruder
 * Same configurations as #MIN_PULSE_PERIOD_ZDIR
 */
#define MIN_PULSE_PERIOD_SOLDER (608-1)

/**
 * @brief Max RPM for C axis
 * Negligible load on the 0,22 kgf*cm motor. In order to work with
 * 0,1 kgf*cm in the best case scenario there will be a loss of 0,12 kgf*cm.
 *
 * At 4 RPM:
 * (1/32 step) 106*4 ~ 424 Hz (2,358 ms)
 *
 * With SMCLK at 8 MHz and output mode 4 (toggle) 
 * set PWM period as 9434*125ns*2 = 1,179 ms
 */
#define MIN_PULSE_PERIOD_ROT (9434-1)

/**
 * @brief Max RPM for XYZ calibration routine
 * 10 RPM (see #MIN_PULSE_PERIOD_ZDIR)
 */
#define MIN_PULSE_CALIB_XYZ (MIN_PULSE_PERIOD_YDIR*2 - 1)

/* Motors' steps per mm constants */

/** @brief X axis steps per mm constant
 * GT-2 belt provides 5 mm per motor rotation. The system is configured to run
 * with 1/32 microstep, with 200 steps per rotation step motors, so:
 * 5 mm = 32*200 steps = 6400 steps 
 * 1 mm = 1280 steps
 */
#define STEPS_PER_MM_X (324)
/** @brief Y axis steps per mm constant
 * GT-2 belt provides 14 mm per motor rotation. The system is configured to run
 * with 1/32 microstep, with 200 steps per rotation step motors, so:
 * 14 mm = 32*200 steps = 6400 steps
 * 1 mm = 3200/7 steps ~ 457 steps
 */
#define STEPS_PER_MM_Y (319)
/** @brief Z axis steps per mm constant
 * Fuse provides 1,5 mm per motor rotation. The system is configured to run
 * with 1/32 microstep, with 200 steps per rotation step motors, so:
 * 1,5 mm = 32*200 steps = 6400 steps
 * 1 mm ~ 4267 steps
 */
#define STEPS_PER_MM_Z (4267)
/** @brief Solder extruder steps per mm constant
 * Fuse provides 1,5 mm per motor rotation. The system is configured to run
 * with 1/32 microstep, with 200 steps per rotation step motors, so:
 * 1,5 mm = 32*200 steps = 6400 steps
 * 1 mm ~ 4267 steps
 */
#define STEPS_PER_MM_S (4267)
/** @brief C axis (RZ) steps per mm constant
 * Motor completes 360 degrees in 200 steps, but system uses 1/32 microsteps.
 * 360° = 32*200 steps = 6400 steps
 * 1° = 160/9 steps ~ 18 steps
 */
#define STEPS_PER_DEG_RZ (18)

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
