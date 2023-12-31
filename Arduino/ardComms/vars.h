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
#define fl_s_nenable 22
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


int32_t fl_pos = 0;
int32_t fr_pos = 0;
int32_t br_pos = 0;
int32_t bl_pos = 0;

#endif
