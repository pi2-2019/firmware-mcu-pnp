/**
 * @file
 * @brief Defines the globally available functions for parsing commands,
 * positioning and calibrating the machine.
 * @author Davi Antônio da Silva Santos
 */

#ifndef FSM_CONTROL_H
#define FSM_CONTROL_H

struct status {
	float x;
	float y;
	float z;
	float rz;
	float solder;
	
	float zmax;
	
	char vacuum;
	char calibrated;
	char error;
	char solder_routine;
	
	char end_p_triggd;
	char end_n_triggd;
};

/** Maximum Z axis position in mm while in solder routine */
const float max_z_solder;
/** Maximum Z axis position in mm while in regular routine */
const float max_z_component;

struct status curr_status;
struct status req_status;

/**
 * @brief Calibrates the machine sending it to the zero point in the X, Y and Z
 * axis.
 *
 * The calibration routine consists in:
 * Move X axis to the + endstop and to the - endstop.
 * Move Y axis to the + endstop and to the - endstop.
 * Move Z axis to the + endstop and to the - endstop.
 * If the wrong endstop is hit, return and set #curr_status.error and leave the
 * endstop flag set.
 *
 * The endstop flags are cleared individually if all proceeds as it has been
 * foreseen.
 *
 * Also, if the routine runs without errors, the positions are updated to zero
 * for X, Y, Z axis, the calibrated flag is set and the endstop triggered and
 * error flags are unset.
 * @return Void.
 */
void calibrate();
/**
 * @brief Moves the stepper motors (X, Y, Z, C/RZ and solder Extruder). Calls
 * #bresenham_3d, #move_rz and #move_solder.
 *
 * If #curr_status.error is set no movement will be done and the machine will
 * prompt for a calibration with #calibrate, issued by G33, or a manual
 * calibration performed by manually configuring the positions in milimeters
 * through G92.
 * @return Void.
 */
void move();
/**
 * @brief Prints system status: X, Y, Z, E positions in milimeters; vacuum
 * status, calibration status and error status.
 * Calls #send_string and #print_float. The data is obtained through the struct
 * #curr_status.
 * @return Void.
 */
void status();
/**
 * @brief Validates and executes the command received by #received_data_ISR and
 * #rx_data_raw.
 * All the positions are precision limited to 6 decimal places and exponential
 * notation is not supported. Unit is fixed to milimeters in absolute mode.
 *
 * Recognized commands:
 *
 * G-codes
 * G0/G1 Xnnn Ynnn Znnn Cnnn Ennn
 *	Moves linearly to a specific point
 * G33
 *	Execute auto calibration routine through #calibrate.
 * G92 Xnnn Ynnn Cnnn Ennn
 *	Set current position (manual calibration). Will clear error flag.
 *
 * M-codes
 * M10
 *	Turn the vacuum on.
 * M11
 *	Turn the vacuum off.
 * M114
 *	Print system status through #status function.
 * If the command is unknown, return a message to the user.
 * Calls #parse_param, #move, #status, and #calibrate.
 * @return Void.
 */
void eval_command();
/**
 * @brief Moves solder extruder to a desired position.
 * @param[in] p1: Initial position in mm, absolute.
 * @param[in] p2 Desired position in mm, absolute.
 * @param[in] period: Frequency of stepper motor pulses.
 * @return Void.
 */
void move_solder(float p1, float p2, unsigned int period);
/**
 * @brief Moves the needle in the C axis (Z axis rotation).
 * @param[in] p1: Initial position in mm, absolute.
 * @param[in] p2: Desired position in mm, absolute.
 * @param[in] period: Frequency of stepper motor pulses.
 * @return Void.
 */
void move_rz(float p1, float p2, unsigned int period);
/**
 * @brief Moves the system in the X, Y and Z axis.
 * @param[in] x1: Initial position in mm, absolute.
 * @param[in] y1: Initial position in mm, absolute.
 * @param[in] z1: Initial position in mm, absolute.
 * @param[in] x2: Desired position in mm, absolute.
 * @param[in] y2: Desired position in mm, absolute.
 * @param[in] z2: Desired position in mm, absolute.
 * @param[in] period: Frequency of stepper motor pulses.
 * @return Void.
 */
void bresenham_3d(float x1, float y1, float z1,
		  float x2, float y2, float z2,
		  unsigned int period);

#endif
