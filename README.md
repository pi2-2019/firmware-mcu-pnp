# PNP Controller Firmware

This firmware was projected to control a PNP machine through G/M-codes.
The microcontroller used is the MSP430G2553, found in the LaunchPad G2
development boards. This build was tested with `MSP430-GCC 6_1_1_0` based on
GCC 7.3.2.154 from Texas Instruments website. 

This PNP has six stepper motors:
* two on the Y axis, one connected with reverse phases;
* one on the X axis;
* one on the Z axis;
* one on the C axis (Z axis rotation);
* one on the solder extruder.

Only the Y axis motors share the same DIR and STEP inputs.

The vacuum valve is also controlled by the MCU and is normally open.

## Build instructions
1. Install `MSPDebug` and `slac640y`
[https://dlbeer.co.nz/articles/slac460y/index.html]
2. Install `MSP430-GCC` from Texas Instruments website
[http://software-dl.ti.com/msp430/msp430_public_sw/mcu/msp430/MSPGCC/latest/index_FDS.html]
3. Configure the compiler and libraries location in the `Makefile`

## Makefile recipes
Update the `CC` and `CPPFLAGS` to match the your development environment.
The Makefile was configured to generate a program optimised for size and will
emit warnings for common mistakes.
* `make devredo` will clear the screen, execute `make clean` and compile
and link the source files and libraries. A map file will be generated after the
linking process.
* `make devclean` will execute `make clean` and clear the screen.
* `make clean` will clear the outputs (`*.o` and `*.elf`).
* `make` will compile and link if necessary.

## System pinout
The microcontroller pinout is:
```
Pin no.	Port no.	Function
1			DVCC (3,3V)
2	P1.0		SWX (X negative endstop INPUT with PULL-UP)
3	P1.2		UCA0RXD
4	P1.3		UCA0TXD
5	P1.4		SWY (Y negative endstop INPUT with PULL-UP)
6	P1.5		DIR_Y (Y axis stepper motors direction OUTPUT)
7	P1.6		DIR_X (X axis stepper motor direction OUTPUT)
8	P2.0		STEP_Y (Y axis stepper motors step OUTPUT)
9	P2.1		STEP_X (X axis stepper motor step OUTPUT)
10	P2.2		DIR_Z (Z axis stepper motor step OUTPUT)
```
```
Pin no.		Port no.	Function
20				DVSS (GND)
19		P2.6		SWZ (Z negative endstop INPUT with PULL-UP)
18		P2.7		VACUUM (vacuum valve OUTPUT)
17				TEST
16				RST
15		P1.7		STEP_S (solder extruder stepper motor OUTPUT)
14		P1.6		STEP_RZ (C axis stepper motor OUTPUT)
13		P2.5		STEP_Z (Z axis stepper motor OUTPUT)
12		P2.4		DIR_S (solder extruder direction OUTPUT)
11		P2.3		DIR_RZ (C axis stepper motor OUTPUT)
```

## Supported G/M-codes
All the G/M-codes supported use only absolute coordinates in millimetres with a
precision of six decimal places. Exponential notation is not supported.

The feedrate is defined internally and is not modifiable via any code.

The command will be executed if the sent string reaches 64 characters or if an
ASCII null byte (`\0`), `*`, `(`, or `;` is sent.

The machine will issue the string "done" after each command is performed
correctly.

### G-codes
* `G0 Xnnnnn.nnnnnn Ynnnnn.nnnnnn Znnnnn.nnnnnn Cnnnnn.nnnnnn Ennnnn.nnnnnn` or
`G1 Xnnnnn.nnnnnn Ynnnnn.nnnnnn Znnnnn.nnnnnn Cnnnnn.nnnnnn Ennnnn.nnnnnn` will
move the motors in a linear fashion. If the machine is in an error status
(error flag is set), it will refuse to move unless an automatic or manual
calibration is performed. The user does not need to send all the positions at
once.

* `G33` will start the auto calibration routine. If the routine is successful
the machine will clear the error flag and set an auto calibration flag. The
error flag will be set if any unexpected condition is detected in the routine.

* `G92 Xnnnnn.nnnnnn Ynnnnn.nnnnnn Znnnnn.nnnnnn Cnnnnn.nnnnnn Ennnnn.nnnnnn`
will set the current position. Useful for manual calibration. The error
flag is always cleared if this command is executed, so it must be used with
caution. The positions do not need to be sent at once.

### M-codes
* `M10` will turn the vacuum on.

* `M11` will turn the vacuum off.

* `M114` will print the system position (X, Y, Z axis and solder extruder), auto
calibration flag, error flag and vacuum valve status.
