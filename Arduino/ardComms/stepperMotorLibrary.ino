
#include "vars.h"

//--------------------------------------//
//           stepper Defines            //
//--------------------------------------//


//intialize all motors
void motors_init(){
  pinMode(fl_s_ndir, OUTPUT);
  pinMode(fr_s_ndir, OUTPUT);
  pinMode(bl_s_ndir, OUTPUT);
  pinMode(br_s_ndir, OUTPUT);

  pinMode(fl_s_pdir, OUTPUT);
  pinMode(fr_s_pdir, OUTPUT);
  pinMode(bl_s_pdir, OUTPUT);
  pinMode(br_s_pdir, OUTPUT);

  pinMode(fr_s_ppwm, OUTPUT);
  pinMode(fl_s_ppwm, OUTPUT);
  pinMode(br_s_ppwm, OUTPUT);
  pinMode(bl_s_ppwm, OUTPUT);

  pinMode(fr_s_npwm, OUTPUT);
  pinMode(fl_s_npwm, OUTPUT);
  pinMode(bl_s_npwm, OUTPUT);
  pinMode(br_s_npwm, OUTPUT);

  
  pinMode(fl_s_penable, OUTPUT);
  pinMode(fr_s_penable, OUTPUT);
  pinMode(bl_s_penable, OUTPUT);
  pinMode(br_s_penable, OUTPUT);

  pinMode(fl_s_nenable, OUTPUT);
  pinMode(fr_s_nenable, OUTPUT);
  pinMode(bl_s_nenable, OUTPUT);
  pinMode(br_s_nenable, OUTPUT);

  pinMode(fl_d_for, OUTPUT);
  pinMode(fr_d_for, OUTPUT);
  pinMode(bl_d_for, OUTPUT);
  pinMode(br_d_for, OUTPUT);
  
  pinMode(fl_d_rev, OUTPUT);
  pinMode(fr_d_rev, OUTPUT);
  pinMode(bl_d_rev, OUTPUT);
  pinMode(br_d_rev, OUTPUT);
  
  pinMode(fl_d_pwm, OUTPUT);
  pinMode(fr_d_pwm, OUTPUT);
  pinMode(bl_d_pwm, OUTPUT);
  pinMode(br_d_pwm, OUTPUT);

  pinMode(fl_a, INPUT);
  pinMode(fl_b, INPUT);
  pinMode(fr_a, INPUT);
  pinMode(fr_b, INPUT);
  pinMode(br_a, INPUT);
  pinMode(br_b, INPUT);
  pinMode(bl_a, INPUT);
  pinMode(bl_b, INPUT);
  motors_PWM(0);
  motors_float(false);
  setupEncoders();
  motors_dir(true);
  }


//--------------------------------------//
//           Stepper Mode               //
//--------------------------------------//
void turnDegrees(float degree){
  uint8_t pwm = 0;
  uint8_t dir = 0;
  
  if (degree > 5){
    dir = 1;
    pwm = 1;
  }
  else if(degree < -5){
    dir = 0;
    pwm = 1;
    }
  else if((degree > -5) & (degree < 5)){
    pwm = 0;
    }

    //front left
    digitalWrite(fl_s_penable, 1);
    digitalWrite(fl_s_pdir, 1);
    digitalWrite(fl_s_ppwm, 1);
    
    digitalWrite(fl_s_nenable, 1);
    digitalWrite(fl_s_ndir, dir);
    digitalWrite(fl_s_npwm, pwm);
    //front right
    digitalWrite(fr_s_penable, 1);
    digitalWrite(fr_s_pdir, 1);
    digitalWrite(fr_s_ppwm, 1);
    
    digitalWrite(fr_s_nenable, 1);
    digitalWrite(fr_s_ndir, dir);
    digitalWrite(fr_s_npwm, pwm);

//bl
    digitalWrite(bl_s_penable, 1);
    digitalWrite(bl_s_pdir, 1);
    digitalWrite(bl_s_ppwm, 1);
    
    digitalWrite(bl_s_nenable, 1);
    digitalWrite(bl_s_ndir, !dir);
    digitalWrite(bl_s_npwm, pwm);
    //br
    digitalWrite(br_s_penable, 1);
    digitalWrite(br_s_pdir, 1);
    digitalWrite(br_s_ppwm, 1);
    
    digitalWrite(br_s_nenable, 1);
    digitalWrite(br_s_ndir, !dir);
    digitalWrite(br_s_npwm, pwm);

    delayMicroseconds(150);
    digitalWrite(fl_s_npwm, 0);
    digitalWrite(fr_s_npwm, 0);
    digitalWrite(bl_s_npwm, 0);
    digitalWrite(br_s_npwm, 0);
    delayMicroseconds(500);
   // }
  }


//--------------------------------------//
//           motor Enables              //
//--------------------------------------//

void fl_stepper_penable(bool enabled){
  digitalWrite(fl_s_penable, enabled);
  }

void fr_stepper_penable(bool enabled){
  digitalWrite(fr_s_penable, enabled);
  }
//back left
void bl_stepper_penable(bool enabled){
  digitalWrite(bl_s_penable, enabled);
  }
//back right
void br_stepper_penable(bool enabled){
  digitalWrite(br_s_penable, enabled);
  }

void fl_stepper_nenable(bool enabled){
  digitalWrite(fl_s_nenable, enabled);
  }

void fr_stepper_nenable(bool enabled){
  digitalWrite(fr_s_nenable, enabled);
  }
//back left
void bl_stepper_nenable(bool enabled){
  digitalWrite(bl_s_penable, enabled);
  }
//back right
void br_stepper_nenable(bool enabled){
  digitalWrite(br_s_nenable, enabled);
  }

//--------------------------------------//
//           Drive motors Dir 
//              false for rev
//--------------------------------------//

void motors_dir(bool reverse){
  fl_motor_dir(reverse);
  fr_motor_dir(reverse);
  bl_motor_dir(reverse);
  br_motor_dir(reverse);
  }

void fl_motor_dir(bool reverse){
  digitalWrite(fl_d_for, !reverse);
  digitalWrite(fl_d_rev, reverse);
  }

void fr_motor_dir(bool reverse){
  digitalWrite(fr_d_for, !reverse);
  digitalWrite(fr_d_rev, reverse);
  }
//back left
void bl_motor_dir(bool reverse){
  digitalWrite(bl_d_for, !reverse);
  digitalWrite(bl_d_rev, reverse);
  }
//back right
void br_motor_dir(bool reverse){
  digitalWrite(br_d_for, !reverse);
  digitalWrite(br_d_rev, reverse);
  }



//--------------------------------------//
//           Drive Motors PWM           //
//--------------------------------------//
void motors_PWM(float duty){
  front_motors_PWM(duty);
  back_motors_PWM(duty);
  }

void front_motors_PWM(float duty){
  fl_motor_PWM(duty);
  fr_motor_PWM(duty);
  }

void back_motors_PWM(float duty){
  bl_motor_PWM(duty);
  br_motor_PWM(duty);
  }


void fl_motor_PWM(float duty){
  analogWrite(fl_d_pwm, duty);
  }

void fr_motor_PWM(float duty){
  analogWrite(fr_d_pwm, duty);
  }
//back left
void bl_motor_PWM(float duty){
  analogWrite(bl_d_pwm, duty);
  }
//back right
void br_motor_PWM(float duty){
  analogWrite(br_d_pwm, duty);
  }

//--------------------------------------//
//           steppers Dir               //
//--------------------------------------//

void front_steppers_dir(bool reverse){
  
  }

void back_steppers_dir(bool reverse){
  
  }

void fl_stepper_ndir(bool reverse){
  digitalWrite(fl_s_ndir, reverse);
  }

void fr_stepper_ndir(bool reverse){
   digitalWrite(fr_s_ndir, reverse);
  }
//back left
void bl_stepper_ndir(bool reverse){
   digitalWrite(bl_s_ndir, reverse);
  }
//back right
void br_stepper_ndir(bool reverse){
   digitalWrite(br_s_ndir, reverse);
  }


void fl_stepper_pdir(bool reverse){
  digitalWrite(fl_s_pdir, reverse);
  }

void fr_stepper_pdir(bool reverse){
   digitalWrite(fr_s_pdir, reverse);
  }
//back left
void bl_stepper_pdir(bool reverse){
   digitalWrite(bl_s_pdir, reverse);
  }
//back right
void br_stepper_pdir(bool reverse){
   digitalWrite(br_s_pdir, reverse);
  }

//--------------------------------------//
//                Stepper PWM           //
//--------------------------------------//


void fl_stepper_PPWM(bool duty){
  digitalWrite(fl_s_ppwm, duty);
  }

void fr_stepper_PPWM(bool duty){
  digitalWrite(fr_s_ppwm, duty);
  }
//back left
void bl_stepper_PPWM(bool duty){
  digitalWrite(bl_s_ppwm, duty);
  }
//back right
void br_stepper_PPWM(bool duty){
  digitalWrite(br_s_ppwm, duty);
  }


void fl_stepper_NPWM(bool duty){
  digitalWrite(fl_s_npwm, duty);
  }

void fr_stepper_NPWM(bool duty){
  digitalWrite(fr_s_npwm, duty);
  }
//back left
void bl_stepper_NPWM(bool duty){
  digitalWrite(bl_s_npwm, duty);
  }
//back right
void br_stepper_NPWM(bool duty){
  digitalWrite(br_s_npwm, duty);
  }




  //--------------------------------------//
//     Drive Motors float and Brake 
//          1 - float
//          0 - brake
//--------------------------------------//
void motors_float(bool floating){
  front_motors_float(floating);
  back_motors_float(floating);
  }

void front_motors_float(bool floating){
  fl_motor_float(floating);
  fr_motor_float(floating);
  }

void back_motors_float(bool floating){
  bl_motor_float(floating);
  br_motor_float(floating);
  }


void fl_motor_float(bool floating){
  digitalWrite(fl_d_for, floating);
  digitalWrite(fl_d_rev, floating);
  }

void fr_motor_float(bool floating){
  digitalWrite(fr_d_for, floating);
  digitalWrite(fr_d_rev, floating);
  }
//back left
void bl_motor_float(bool floating){
  digitalWrite(bl_d_for, floating);
  digitalWrite(bl_d_rev, floating);
  }
//back right
void br_motor_float(bool floating){
  digitalWrite(br_d_for, floating);
  digitalWrite(br_d_rev, floating);
  }
