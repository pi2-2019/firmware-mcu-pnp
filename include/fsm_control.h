/**
 * @file
 * @brief Defines the globally available FSM states which will control the
 * system though a function pointer.
 * The FSM will be started after the initial setup is done.
 * @author Davi Antônio da Silva Santos
 */

#ifndef FSM_CONTROL_H
#define FSM_CONTROL_H

void initial_state();
void find_x_limits();
void find_y_limits();
void find_z_limits();
void wait_command();
void eval_command();
void pos_control();
void vacuum_on();
void vacuum_off();
void move_x();
void move_y();
void move_z();
void move_rotz();
void move_solder();
void panic_state();

#endif
