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
#define fl_d_for 39 //was 41
#define fr_d_for 40
#define bl_d_for 36 //was 37
#define br_d_for 12

//drivers neg dir
#define fl_d_rev 41 //was 39
#define fr_d_rev 38
#define bl_d_rev 37 // was 36
#define br_d_rev A0

//stepper pos enable -PULL HIGH
#define fl_s_nenable 22  //was 52
#define fr_s_nenable 23  //was 34
#define bl_s_nenable 52 //was 43
#define br_s_nenable 43 //was 23
//stepper pos enable - PULL HIGH
#define fl_s_penable 24  // was 50
#define fr_s_penable 25  //was26
#define bl_s_penable 50 //was 45
#define br_s_penable 45 //was 25
//steppers neg dir - DIR pick
#define fl_s_ndir 26  //was 48
#define fr_s_ndir 27  //was 28
#define bl_s_ndir 48 //was 47
#define br_s_ndir 47 //was 27
//steppers pos dir - PULL HIGH
#define fl_s_pdir 28  //was 46
#define fr_s_pdir 29  //was 30
#define bl_s_pdir 46 //was 49
#define br_s_pdir 49 //was 29
//steppers neg pwm - PULSE TO MOVE
#define fl_s_npwm 30 //was 44
#define fr_s_npwm 31 //was 32
#define bl_s_npwm 44 //was 51
#define br_s_npwm 51 //was 31
//steppers pos pwm - PULL HIGH
#define fl_s_ppwm 32 //was 42
#define fr_s_ppwm 33 //was 34
#define bl_s_ppwm 42 //was 53
#define br_s_ppwm 53 //was 33

//motor encoders 
#define fl_a 2
#define fl_b 9
#define fr_a 3
#define fr_b 10
#define bl_a 18
#define bl_b A3
#define br_a 19
#define br_b A1

#define MAX_PWM 255
#define MAX_LINEAR_PWM 255
#define MAX_ANGULAR_PWM 255
#define MAX_INTEGRAL 1
#define MAX_V_INTEGRAL 5
#define MAX_ANGLE 90.0 
#define MAX_ANGULAR_VEL .99
float wheel_diameter = .345; //approx .31 without spikes also this is in meters
float robot_width = .60; // measured from outer edge to outeredge of stepper motor
float robot_len = .855;
float enc_per_rev = 5281;

float fl_enc_pos = 0;
float fr_enc_pos = 0;
float br_enc_pos = 0;
float bl_enc_pos = 0;

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

float fl_rotations = 0;
float fr_rotations = 0;
float bl_rotations = 0;
float br_rotations = 0;

float target_angular_velocity = 0;

bool toggleAuto = false;
bool stepper_locked = false;

float lin_vels[4] = {0, 0, 0, 0}; 
float pos_angs[4] = {0, 0, 0, 0}; 
#endif
