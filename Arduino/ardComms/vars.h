#ifndef VARS_H
#define VARS_H

//--------------------------------------//
//           motor Defines              //
//--------------------------------------//
//drivers pos pwm
#define fl_d_pwm 11
#define fr_d_pwm 4
#define bl_d_pwm 6
#define br_d_pwm 5

//drivers pos dir
#define fl_d_for 41
#define fr_d_for 40
#define bl_d_for 37
#define br_d_for 12

//drivers neg dir
#define fl_d_rev 39
#define fr_d_rev 38
#define bl_d_rev 36
#define br_d_rev A0

//stepper pos enable -PULL HIGH
#define fl_s_nenable 52
#define fr_s_nenable 24
#define bl_s_nenable 43
#define br_s_nenable 23
//stepper pos enable - PULL HIGH
#define fl_s_penable 50
#define fr_s_penable 26
#define bl_s_penable 45
#define br_s_penable 25
//steppers neg dir - DIR pick
#define fl_s_ndir 48
#define fr_s_ndir 28
#define bl_s_ndir 47
#define br_s_ndir 27
//steppers pos dir - PULL HIGH
#define fl_s_pdir 46
#define fr_s_pdir 30
#define bl_s_pdir 49
#define br_s_pdir 29
//steppers neg pwm - PULSE TO MOVE
#define fl_s_npwm 44
#define fr_s_npwm 32
#define bl_s_npwm 51
#define br_s_npwm 31
//steppers pos pwm - PULL HIGH
#define fl_s_ppwm 42
#define fr_s_ppwm 34
#define bl_s_ppwm 53
#define br_s_ppwm 33

//motor encoders 
#define fl_a 2
#define fl_b 9
#define fr_a 3
#define fr_b 10
#define bl_a 18
#define bl_b A3
#define br_a 19
#define br_b A1

#define MAX_PWM 200
#define MAX_LINEAR_PWM 150
#define MAX_ANGULAR_PWM 100
#define MAX_INTEGRAL 1
#define MAX_V_INTEGRAL 5

float wheel_diameter = .345; //approx .31 without spikes also this is in meters
float robot_width = .60; // measured from outer edge to outeredge of stepper motor
float robot_len = .855;
float enc_per_rev = 5281;

int32_t fl_enc_pos = 0;
int32_t fr_enc_pos = 0;
int32_t br_enc_pos = 0;
int32_t bl_enc_pos = 0;

float fl_s_pos = 0;
float fr_s_pos = 0;
float bl_s_pos = 0;
float br_s_pos = 0;

float fl_target_v = 0;
float fr_target_v = 0;
float bl_target_v = 0;
float br_target_v = 0;

float fl_target_p = 0;
float fr_target_p = 0;
float bl_target_p = 0;
float br_target_p = 0;

float fl_pwm = 0;
float fr_pwm = 0;
float bl_pwm = 0;
float br_pwm = 0;

float fl_linear_pwm = 0;
float fr_linear_pwm = 0;
float bl_linear_pwm = 0;
float br_linear_pwm = 0;

float fl_angular_pwm = 0;
float fr_angular_pwm = 0;
float bl_angular_pwm = 0;
float br_angular_pwm = 0;

bool fl_dir = HIGH;
bool fr_dir = HIGH;
bool bl_dir = HIGH;
bool br_dir = HIGH;

float fl_rotations = 0;
float fr_rotations = 0;
float bl_rotations = 0;
float br_rotations = 0;
#endif
