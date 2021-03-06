/**
 * @file
 * @brief Implements the globally available FSM states which will control the
 * system though a function pointer.
 * The FSM will be started after the initial setup is done.
 * @author Davi Antônio da Silva Santos
 */

#include <msp430.h>
#include <string.h>
#include <stdlib.h>
#include <float.h>
#include "sys_config.h"
#include "usart.h"
#include "sys_control.h"
#include "timers.h"

/** Maximum Z axis position in mm while in solder routine mm*/
const float max_z_solder = 53.2f;
/** Maximum Z axis position in mm while in regular routine 64.41 mm*/
const float max_z_component = 64.41f;
/** Maximum X axis position in mm */
const float max_x = 298.0f;
/** Maximum Y axis position in mm */
const float max_y = 370.0f;

void calibrate()
{
	/*
	 * P1IFG is reconfigured to avoid errors, but it will be reset
	 * automatically if it is set inside the interruption handler
	 */
	P1IFG = 0;
	
	curr_status.calibrated = 0;
	req_status.calibrated = 1;
	req_status.error = 0;
	curr_status.end_triggd = 0;
	
	/* Move X axis to X- */
	RESET_DIR_X;
	send_string("Goto X-\n");
	
	while((!curr_status.end_triggd)) {
		TOGGLE_STEPS_X;
		__delay_cycles(MIN_PULSE_CALIB_XYZ);
	}
	P1IE &= ~(SWX | SWY);
	bresenham_3d(0, 0, 0, 5, 0, 0, MIN_PULSE_PERIOD_XDIR);
	curr_status.end_triggd = 0;
	send_string("X- OK\n");
	P1IE |= (SWX | SWY);
	
	/* Move Y axis to zero */
	SET_DIR_Y;
	send_string("Goto Y-\n");
	
	while((!curr_status.end_triggd)) {
		TOGGLE_STEPS_Y;
		__delay_cycles(MIN_PULSE_CALIB_XYZ);
	}
	P1IE &= ~(SWX | SWY);
	bresenham_3d(0, 0, 0, 0, 5, 0, MIN_PULSE_PERIOD_YDIR);
	curr_status.end_triggd = 0;
	send_string("Y- OK\n");
	P1IE |= (SWX | SWY);
	
	/* Move Z axis to zero */
	RESET_DIR_Z;
	send_string("Goto Z-\n");
	while((!curr_status.end_triggd)) {
		TOGGLE_STEPS_Z;
		__delay_cycles(MIN_PULSE_CALIB_XYZ);
	}
	P2IE &= ~SWZ;
	bresenham_3d(0, 0, 0, 0, 0, 5, MIN_PULSE_PERIOD_ZDIR);
	curr_status.end_triggd = 0;
	send_string("Z- OK\n");
	P2IE |= SWZ;

	curr_status.calibrated = 1;
	curr_status.x = 0;
	curr_status.y = 0;
	curr_status.z = 0;
	req_status.x = 0;
	req_status.y = 0;
	req_status.z = 0;
	curr_status.end_triggd = 0;
	curr_status.error = 0;

	send_string("done\n");
}

void move()
{
	unsigned int period;
	
	if (!curr_status.error) {
		/* Move as fast as the slowest motor */
		if (curr_status.y != req_status.y) {
			/* Slowest motor */
			period = MIN_PULSE_PERIOD_YDIR;
		} else if (curr_status.x != req_status.x) {
			/* Second slowest motor */
			period = MIN_PULSE_PERIOD_XDIR;
		} else {
			/* Fastest */
			period = MIN_PULSE_PERIOD_ZDIR;
		}

		bresenham_3d(curr_status.x, curr_status.y, curr_status.z,
				req_status.x, req_status.y, req_status.z,
				period);
				
		/* Initial position must always be treated as zero */
		move_rz(0, req_status.rz, MIN_PULSE_PERIOD_ROT);
		
		move_solder(curr_status.solder, req_status.solder,
				MIN_PULSE_PERIOD_SOLDER);
				
		send_string("done\n");
	} else {
		send_string("RECAL\n");
		send_string("done\n");
	}
}

void status()
{
	char yes_str[] = "Y\n";
	char no_str[] = "N\n";
	
	send_char('\n');
	send_string("mm abs\n");
	
	send_string("X ");
	print_float(curr_status.x);
	send_char('\n');
	
	send_string("Y ");
	print_float(curr_status.y);
	send_char('\n');
	
	send_string("Z ");
	print_float(curr_status.z);
	send_char('\n');
	
	send_string("E ");
	print_float(curr_status.solder);
	send_char('\n');
	
	send_string("SDR ");
	if (curr_status.solder_routine)
		send_string(yes_str);
	else
		send_string(no_str);
	
	send_string("ZM ");
	print_float(curr_status.zmax);
	send_char('\n');
	
	send_string("VAC ");
	if (curr_status.vacuum)
		send_string(yes_str);
	else
		send_string(no_str);
	
	send_string("CAL ");
	if (curr_status.calibrated)
		send_string(yes_str);
	else
		send_string(no_str);
	
	send_string("ERR ");
	if (curr_status.error)
		send_string(yes_str);
	else
		send_string(no_str);
	
	send_string("END ");
	if(curr_status.end_triggd)
		send_string(yes_str);
	else
		send_string(no_str);
	
	send_string("done\n");
}

void eval_command()
{
	if (!execute_routine)
		return;

	/** G/M-code to be executed */
	int cmd = 0;
	/** Parsed G-code is unknown? 1 if yes*/
	char uknown_gc = 0;
	/** Parsed M-code is unknown? 1 if yes*/
	char uknown_mc = 0;

	/* Get the G-code */
	cmd = parse_param('G', -1);
	
	switch(cmd) {
	case 0:
	case 1:
	/* Move to a specific point */
		req_status.x = parse_param('X', curr_status.x);
		req_status.y = parse_param('Y', curr_status.y);
		req_status.z = parse_param('Z', curr_status.z);
		req_status.rz = parse_param('C', curr_status.rz);
		req_status.solder = parse_param('E', FLT_MAX);
		
		/* If no solder will be used, set Z max to vacuum tip */		
		if (req_status.solder == FLT_MAX) {
			/* Will not solder */
			req_status.solder = curr_status.solder;
			
			req_status.zmax = max_z_component;
			req_status.solder_routine = 0;
			
			curr_status.zmax = max_z_component;
			curr_status.solder_routine = 0;
		} else {
			req_status.zmax = max_z_solder;
			req_status.solder_routine = 1;
			
			curr_status.zmax = max_z_solder;
			curr_status.solder_routine = 1;
		}

		if (req_status.x >= max_x) {
			send_string("XM ");
			print_float(max_x);
			send_char('\n');
			req_status.x = max_x;
		}

		if (req_status.y >= max_y) {
			send_string("YM ");
			print_float(max_y);
			send_char('\n');
			req_status.y = max_y;
		}

		if (req_status.z >= req_status.zmax) {
			send_string("ZM ");
			print_float(curr_status.zmax);
			send_char('\n');
			req_status.z = curr_status.zmax;
		}

		move();
		break;
	case 33:
	/* Auto calibration */
		calibrate();
		break;
	case 92:
	/* Set current position (manual calibration) */
		req_status.error = 0;
		curr_status.x = parse_param('X', curr_status.x);
		curr_status.y = parse_param('Y', curr_status.y);
		curr_status.z = parse_param('Z', curr_status.z);
		curr_status.rz = parse_param('C', curr_status.rz);
		curr_status.solder = parse_param('E', curr_status.solder);
		curr_status.error = 0;
		send_string("done\n");
		break;
	default:
		uknown_gc = 1;
		break;
	}
	
	/* Get the M-code */
	cmd = parse_param('M', -1);
	
	switch(cmd) {
	case 10: /* vacuum on */
	
		/* pulse the excitor coil? */
		req_status.vacuum = 1;
		/* Normally open valve */
		SET_VACUUM;
		curr_status.vacuum = 1;
		send_string("done\n");
		break;
	case 11: /* vacuum off */
		req_status.vacuum = 0;
		/* Normally open valve */
		RESET_VACUUM;
		curr_status.vacuum = 0;
		send_string("done\n");
		break;
	case 114:
		status();
		break;
	default:
		uknown_mc = 1;
		break;
	}
	
	if (uknown_gc && uknown_mc) {
		send_string("G/M-Code?\n");
		send_string("done\n");
	}
	
	memset(rx_data_raw, 0, RX_STR_SIZE);	
	
	execute_routine = 0;
}

void move_solder(float p1f, float p2f, unsigned int period)
{
	long int p1 = p1f*STEPS_PER_MM_S;
	long int p2 = p2f*STEPS_PER_MM_S;
	
	long int ps;
	
	ps = (p2 > p1) ? 1 : -1;
	
	/* Positive downwards */
	if (ps == 1)
		RESET_DIR_S;
	else
		SET_DIR_S;
	
	start_t1_a3_c0(period);
	
	while((p1 != p2)) {
		p1 += ps;
		
		/* Move solder */
		TOGGLE_STEPS_S;
		
		/* Wait. If an endstop is hit, stop the machine */
		while(!(TA1CTL & TAIFG));
		TA1CTL &= ~TAIFG;
	}
	stop_t1_a3_c0();
	
	/* Update position */
	curr_status.solder = req_status.solder;
}

void move_rz(float p1f, float p2f, unsigned int period)
{
	P1IE &= ~(SWX | SWY);
	P2IE &= ~SWZ;
	
	long int p1 = p1f*STEPS_PER_DEG_RZ;
	long int p2 = p2f*STEPS_PER_DEG_RZ;

	long int ps;
	
	ps = (p2 > p1) ? 1 : -1;
	
	/* Positive clockwise */
	if (ps == 1)
		SET_DIR_RZ;
	else
		RESET_DIR_RZ;
	
	start_t1_a3_c0(period);
	
	while((p1 != p2)) {
		p1 += ps;
		
		/* Move rz */
		TOGGLE_STEPS_RZ;
		
		/* Wait. If an endstop is hit, stop the machine */
		while(!(TA1CTL & TAIFG));
		TA1CTL &= ~TAIFG;
	}
	stop_t1_a3_c0();
	
	/* Update position */
	curr_status.rz = 0;
	req_status.rz = 0;
	
	P1IE |= (SWX | SWY);
	P2IE |= SWZ;
}

void bresenham_3d(float x1f, float y1f, float z1f,
		  float x2f, float y2f, float z2f,
		  unsigned int period)
{
	long int x1 = x1f*STEPS_PER_MM_X;
	long int y1 = y1f*STEPS_PER_MM_Y;
	long int z1 = z1f*STEPS_PER_MM_Z;
	
	long int x2 = x2f*STEPS_PER_MM_X;
	long int y2 = y2f*STEPS_PER_MM_Y;
	long int z2 = z2f*STEPS_PER_MM_Z;
	
	long int dx = labs(x2 - x1);
	long int dy = labs(y2 - y1);
	long int dz = labs(z2 - z1);
	
	long int xs;
	long int ys;
	long int zs;
	
	long int p1;
	long int p2;
	
	/* Same as direction for the drivers but with 0 and 1 */
	xs = (x2 > x1) ? 1 : -1;
	ys = (y2 > y1) ? 1 : -1;
	zs = (z2 > z1) ? 1 : -1;
	
	if (xs == 1)
		SET_DIR_X;
	else
		RESET_DIR_X;

	if (ys == 1)
		RESET_DIR_Y;
	else
		SET_DIR_Y;

	if (zs == 1)
		SET_DIR_Z;
	else
		RESET_DIR_Z;
	
	start_t1_a3_c0(period);
	
	/* Drive X axis */
	if ((dx >= dy) && (dx >= dz)) {
		p1 = 2*dy - dx;
		p2 = 2*dz - dx;
		
		while ((x1 != x2)) {
			TOGGLE_STEPS_X;
			x1 += xs;
			if (p1 >= 0) {
				TOGGLE_STEPS_Y;
				y1 += ys;
				p1 -= 2*dx;
			}
			if (p2 >= 0) {
				TOGGLE_STEPS_Z;
				z1 += zs;
				p2 -= 2*dx;
			}
			p1 += 2*dy;
			p2 += 2*dz;
			
			/* Wait. If an endstop is hit, stop the machine */
			while(!(TA1CTL & TAIFG));
			TA1CTL &= ~TAIFG;
		}
	/* Drive Y Axis */
	} else if ((dy >= dx) && (dy >= dz)) {
		p1 = 2*dx - dy;
		p2 = 2*dz - dy;
		
		while ((y1 != y2)) {
			TOGGLE_STEPS_Y;
			y1 += ys;
			if (p1 >= 0) {
				TOGGLE_STEPS_X;
				x1 += xs;
				p1 -= 2*dy;
			}
			if (p2 >= 0) {
				TOGGLE_STEPS_Z;
				z1 += zs;
				p2 -= 2*dy;
			}
			p1 += 2*dx;
			p2 += 2*dz;
			
			/* Wait. If an endstop is hit, stop the machine */
			while(!(TA1CTL & TAIFG));
			TA1CTL &= ~TAIFG;
		}
	/* Drive Z Axis */
	} else {
		p1 = 2*dy - dz;
		p2 = 2*dx - dz;
		
		while ((z1 != z2)) {
			TOGGLE_STEPS_Z;
			z1 += zs;
			if (p1 >= 0) {
				TOGGLE_STEPS_Y;
				y1 += ys;
				p1 -= 2*dz;
			}
			if (p2 >= 0) {
				TOGGLE_STEPS_X;
				x1 += xs;
				p2 -= 2*dz;
			}
			p1 += 2*dy;
			p2 += 2*dx;

			/* Wait. If an endstop is hit, stop the machine */
			while(!(TA1CTL & TAIFG));
			TA1CTL &= ~TAIFG;
		}
	}
	stop_t1_a3_c0();
	
	/* Update positions */
	curr_status.x = req_status.x;
	curr_status.y = req_status.y;
	curr_status.z = req_status.z;
}
