
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
//           Stepper Motor steps
//              this will do them
//              all at once
//--------------------------------------//
//5 options front 2 at once, back 2 at once, all three at once same angle, opposite angle, front angles and back angles pick same time

void step_pos(float p[3]){
  
  stepper_locked = true;
  float fl = p[0] * 180/3.14159265359;
  float fr = p[1] * 180/3.14159265359;
  float bl = p[2] * 180/3.14159265359;
  float br = p[3] * 180/3.14159265359;
  float fl_del = max(-MAX_ANGLE, min(MAX_ANGLE, fl - fl_s_pos)); 
  float fr_del = max(-MAX_ANGLE, min(MAX_ANGLE, fr - fr_s_pos));
  float bl_del = max(-MAX_ANGLE, min(MAX_ANGLE, bl - bl_s_pos));
  float br_del = max(-MAX_ANGLE, min(MAX_ANGLE, br - br_s_pos));

  bool fl_dir_ang = fl_del > 0;
  bool fr_dir_ang = fr_del > 0;
  bool bl_dir_ang = bl_del > 0;
  bool br_dir_ang = br_del > 0;

  int fl_steps = deg_to_step(fl_del);
  int fr_steps = deg_to_step(fr_del);
  int bl_steps = deg_to_step(bl_del);
  int br_steps = deg_to_step(br_del);

  fl_s_pos_update(fl_del, fl_dir_ang);
  fr_s_pos_update(fr_del, fr_dir_ang);
  bl_s_pos_update(bl_del, bl_dir_ang);
  br_s_pos_update(br_del, br_dir_ang);
//saftey first
if (fl_s_pos > 90 || fl_s_pos < -90){
    fl_steps = 0;
    fl_s_pos_update(fl_del, !fl_dir_ang);
    }
if (fr_s_pos > 90 || fr_s_pos < -90){
    fr_steps = 0;
    fr_s_pos_update(fr_del, !fr_dir_ang);
    }
if (bl_s_pos > 90 || bl_s_pos < -90){
    bl_steps = 0;
    bl_s_pos_update(bl_del, !bl_dir_ang);
    }
if (br_s_pos > 90 || br_s_pos < -90){
    br_steps = 0;
    br_s_pos_update(br_del, !br_dir_ang);
    }   

  digitalWrite(fl_s_ndir, fl_dir_ang);
  digitalWrite(fl_s_pdir, !fl_dir_ang);
  digitalWrite(fl_s_npwm, LOW);
  
  digitalWrite(fr_s_ndir, fr_dir_ang);
  digitalWrite(fr_s_pdir, !fr_dir_ang);
  digitalWrite(fr_s_npwm, LOW);

  digitalWrite(bl_s_ndir, bl_dir_ang);
  digitalWrite(bl_s_pdir, !bl_dir_ang);
  digitalWrite(bl_s_npwm, LOW);
  
  digitalWrite(br_s_ndir, br_dir_ang);
  digitalWrite(br_s_pdir, !br_dir_ang);
  digitalWrite(br_s_npwm, LOW);

  int fl_cnt = 0;
  int fr_cnt = 0;
  int bl_cnt = 0;
  int br_cnt = 0;

  while ( (fl_cnt <= abs(fl_steps))
        ||(fr_cnt <= abs(fr_steps))
        ||(bl_cnt <= abs(bl_steps))
        ||(br_cnt <= abs(br_steps))){

        if (fl_cnt < abs(fl_steps)){digitalWrite(fl_s_ppwm, HIGH); }
        if (fr_cnt < abs(fr_steps)){digitalWrite(fr_s_ppwm, HIGH); }
        if (bl_cnt < abs(bl_steps)){digitalWrite(bl_s_ppwm, HIGH); }
        if (br_cnt < abs(br_steps)){digitalWrite(br_s_ppwm, HIGH); }
        if (Serial.available() > 0){
          newComms();
            if (newData == true) {
        
            strcpy(tempChars, receivedChars);
            parseData();
            newData = false;
            }
          }
    
        delayMicroseconds(50000);
        digitalWrite(fl_s_ppwm, LOW);
        digitalWrite(fr_s_ppwm, LOW);
        digitalWrite(bl_s_ppwm, LOW);
        digitalWrite(br_s_ppwm, LOW);
        
        fl_cnt++;
        fr_cnt++;
        bl_cnt++;
        br_cnt++;
        stepper_locked = false;  
}
}//end while loop

void global_angle_select(int fl, int fr, int bl, int br){
 
  stepper_locked = true;

  int fl_del = max(-MAX_ANGLE, min(MAX_ANGLE, fl - fl_s_pos)); 
  int fr_del = max(-MAX_ANGLE, min(MAX_ANGLE, fr - fr_s_pos));
  int bl_del = max(-MAX_ANGLE, min(MAX_ANGLE, bl - bl_s_pos));
  int br_del = max(-MAX_ANGLE, min(MAX_ANGLE, br - br_s_pos));

  bool fl_dir_ang = fl_del > 0;
  bool fr_dir_ang = fr_del > 0;
  bool bl_dir_ang = bl_del > 0;
  bool br_dir_ang = br_del > 0;

  int fl_steps = deg_to_step(fl_del);
  int fr_steps = deg_to_step(fr_del);
  int bl_steps = deg_to_step(bl_del);
  int br_steps = deg_to_step(br_del);

  fl_s_pos_update(fl_del, fl_dir_ang);
  fr_s_pos_update(fr_del, fr_dir_ang);
  bl_s_pos_update(bl_del, bl_dir_ang);
  br_s_pos_update(br_del, br_dir_ang);

if (fl_s_pos > 90 || fl_s_pos < -90){
    fl_steps = 0;
    fl_s_pos_update(fl_del, !fl_dir_ang);
    }
if (fr_s_pos > 90 || fr_s_pos < -90){
    fr_steps = 0;
    fr_s_pos_update(fr_del, !fr_dir_ang);
    }
if (bl_s_pos > 90 || bl_s_pos < -90){
    bl_steps = 0;
    bl_s_pos_update(bl_del, !bl_dir_ang);
    }
if (br_s_pos > 90 || br_s_pos < -90){
    br_steps = 0;
    br_s_pos_update(br_del, !br_dir_ang);
    }   

  digitalWrite(fl_s_ndir, fl_dir_ang);
  digitalWrite(fl_s_pdir, !fl_dir_ang);
  digitalWrite(fl_s_npwm, LOW);
  
  digitalWrite(fr_s_ndir, fr_dir_ang);
  digitalWrite(fr_s_pdir, !fr_dir_ang);
  digitalWrite(fr_s_npwm, LOW);

  digitalWrite(bl_s_ndir, bl_dir_ang);
  digitalWrite(bl_s_pdir, !bl_dir_ang);
  digitalWrite(bl_s_npwm, LOW);
  
  digitalWrite(br_s_ndir, br_dir_ang);
  digitalWrite(br_s_pdir, !br_dir_ang);
  digitalWrite(br_s_npwm, LOW);

  int fl_cnt = 0;
  int fr_cnt = 0;
  int bl_cnt = 0;
  int br_cnt = 0;
            // Serial.print(fl_steps);
            // Serial.print(" ");
            // Serial.print(fr_steps);
            // Serial.print(" ");
            // Serial.print(bl_steps);
            // Serial.print(" ");
            // Serial.print(br_steps);
            // Serial.println(" ");
  while ( (fl_cnt <= abs(fl_steps))
        ||(fr_cnt <= abs(fr_steps))
        ||(bl_cnt <= abs(bl_steps))
        ||(br_cnt <= abs(br_steps))){
            // Serial.print(fl_cnt);
            // Serial.print(" ");
            // Serial.print(fr_cnt);
            // Serial.print(" ");
            // Serial.print(bl_cnt);
            // Serial.print(" ");
            // Serial.print(br_cnt);
            // Serial.println(" ");
        if (Serial.available()> 0){
        newComms();
        }
        if (fl_cnt < abs(fl_steps)){digitalWrite(fl_s_ppwm, HIGH); }
        if (fr_cnt < abs(fr_steps)){digitalWrite(fr_s_ppwm, HIGH); }
        if (bl_cnt < abs(bl_steps)){digitalWrite(bl_s_ppwm, HIGH); }
        if (br_cnt < abs(br_steps)){digitalWrite(br_s_ppwm, HIGH); }
    
        delayMicroseconds(50000);
        digitalWrite(fl_s_ppwm, LOW);
        digitalWrite(fr_s_ppwm, LOW);
        digitalWrite(bl_s_ppwm, LOW);
        digitalWrite(br_s_ppwm, LOW);
        
        fl_cnt++;
        fr_cnt++;
        bl_cnt++;
        br_cnt++;
        stepper_locked = false;  
}
}//end while loop


void opp_s_step_fast(float degree){
  
  stepper_locked = true;
  bool dir = degree > 0;
  int steps = deg_to_step(degree);
  fl_s_pos_update(degree, dir);
  fr_s_pos_update(degree, dir);
  bl_s_pos_update(degree, !dir);
  br_s_pos_update(degree, !dir);

if (fl_s_pos > 90 || fl_s_pos < -90){
    steps = 0;
    fl_s_pos_update(degree, !dir);
    }
if (fr_s_pos > 90 || fr_s_pos < -90){
    steps = 0;
    fr_s_pos_update(degree, !dir);
    }
if (bl_s_pos > 90 || bl_s_pos < -90){
    steps = 0;
    bl_s_pos_update(degree, !dir);
    }
if (br_s_pos > 90 || br_s_pos < -90){
    steps = 0;
    br_s_pos_update(degree, !dir);
    }   

  digitalWrite(fl_s_ndir, !dir);
  digitalWrite(fl_s_pdir, dir);
  digitalWrite(fl_s_npwm, LOW);
  
  digitalWrite(fr_s_ndir, !dir);
  digitalWrite(fr_s_pdir, dir);
  digitalWrite(fr_s_npwm, LOW);

  digitalWrite(bl_s_ndir, dir);
  digitalWrite(bl_s_pdir, !dir);
  digitalWrite(bl_s_npwm, LOW);
  
  digitalWrite(br_s_ndir, dir);
  digitalWrite(br_s_pdir, !dir);
  digitalWrite(br_s_npwm, LOW);

  for (int i = 0; i < abs(steps); i++) {
    digitalWrite(fl_s_ppwm, HIGH);
    digitalWrite(fr_s_ppwm, HIGH);
    digitalWrite(bl_s_ppwm, HIGH);
    digitalWrite(br_s_ppwm, HIGH);
    delayMicroseconds(50000);
    digitalWrite(fl_s_ppwm, LOW);
    digitalWrite(fr_s_ppwm, LOW);
    digitalWrite(bl_s_ppwm, LOW);
    digitalWrite(br_s_ppwm, LOW);
    }
  stepper_locked = false;  
}

void same_s_step_fast(float degree){
  stepper_locked = true;
  bool dir = degree > 0;
  int steps = deg_to_step(degree);
  fl_s_pos_update(degree, dir);
  fr_s_pos_update(degree, dir);
  bl_s_pos_update(degree, dir);
  br_s_pos_update(degree, dir);


if (fl_s_pos > 90 || fl_s_pos < -90){
    steps = 0;
    fl_s_pos_update(degree, !dir);
    }
if (fr_s_pos > 90 || fr_s_pos < -90){
    steps = 0;
    fr_s_pos_update(degree, !dir);
    }
if (bl_s_pos > 90 || bl_s_pos < -90){
    steps = 0;
    bl_s_pos_update(degree, !dir);
    }
if (br_s_pos > 90 || br_s_pos < -90){
    steps = 0;
    br_s_pos_update(degree, !dir);
    }  

  digitalWrite(fl_s_ndir, !dir);
  digitalWrite(fl_s_pdir, dir);
  digitalWrite(fl_s_npwm, LOW);
  
  digitalWrite(fr_s_ndir, !dir);
  digitalWrite(fr_s_pdir, dir);
  digitalWrite(fr_s_npwm, LOW);

  digitalWrite(bl_s_ndir, !dir);
  digitalWrite(bl_s_pdir, dir);
  digitalWrite(bl_s_npwm, LOW);
  
  digitalWrite(br_s_ndir, !dir);
  digitalWrite(br_s_pdir, dir);
  digitalWrite(br_s_npwm, LOW);

  for (int i = 0; i < abs(steps); i++) {
    digitalWrite(fl_s_ppwm, HIGH);
    digitalWrite(fr_s_ppwm, HIGH);
    digitalWrite(bl_s_ppwm, HIGH);
    digitalWrite(br_s_ppwm, HIGH);
    delayMicroseconds(50000);
    digitalWrite(fl_s_ppwm, LOW);
    digitalWrite(fr_s_ppwm, LOW);
    digitalWrite(bl_s_ppwm, LOW);
    digitalWrite(br_s_ppwm, LOW);
    }
    stepper_locked = false;
}

void f_s_step_fast(float degree){
  stepper_locked = true;
  bool dir = degree > 0;
  int steps = deg_to_step(degree);
  fl_s_pos_update(degree, dir);
  fr_s_pos_update(degree, dir);
if (fl_s_pos > 90 || fl_s_pos < -90){
    steps = 0;
    fl_s_pos_update(degree, !dir);
    }
if (fr_s_pos > 90 || fr_s_pos < -90){
    steps = 0;
    fr_s_pos_update(degree, !dir);
    }

  digitalWrite(fl_s_ndir, !dir);
  digitalWrite(fl_s_pdir, dir);
  digitalWrite(fl_s_npwm, LOW);
  
  digitalWrite(fr_s_ndir, !dir);
  digitalWrite(fr_s_pdir, dir);
  digitalWrite(fr_s_npwm, LOW);

  for (int i = 0; i < abs(steps); i++) {
    digitalWrite(fl_s_ppwm, HIGH);
    digitalWrite(fr_s_ppwm, HIGH);
    delayMicroseconds(50000);
    digitalWrite(fl_s_ppwm, LOW);
    digitalWrite(fr_s_ppwm, LOW);
    
  }
  stepper_locked = false;
  }
  
void b_s_step_fast(float degree){
  stepper_locked = true;
  bool dir = degree > 0;
  int steps = deg_to_step(degree);
  bl_s_pos_update(degree, dir);
  br_s_pos_update(degree, dir);

if (bl_s_pos > 90 || bl_s_pos < -90){
    steps = 0;
    bl_s_pos_update(degree, !dir);
    }
if (br_s_pos > 90 || br_s_pos < -90){
    steps = 0;
    br_s_pos_update(degree, !dir);
    }  

  digitalWrite(bl_s_ndir, !dir);
  digitalWrite(bl_s_pdir, dir);
  digitalWrite(bl_s_npwm, LOW);
  
  digitalWrite(br_s_ndir, !dir);
  digitalWrite(br_s_pdir, dir);
  digitalWrite(br_s_npwm, LOW);

  for (int i = 0; i < abs(steps); i++) {
    digitalWrite(bl_s_ppwm, HIGH);
    digitalWrite(br_s_ppwm, HIGH);
    delayMicroseconds(50000);
    digitalWrite(bl_s_ppwm, LOW);
    digitalWrite(br_s_ppwm, LOW);
    
  }
  stepper_locked = false;
  }
//--------------------------------------//
//           Stepper Motor steps
//              this will do them
//              one at a time
//--------------------------------------//

//4 functions to turn each indvidually then 2 for front 2 for back and then 1 more for all together
//cc is High so setting dir to high will make front left turn left
//front and back go same angle opposite directions
void opp_s_step(float degree, bool dir){
  f_s_step(degree, dir);
  b_s_step(degree, !dir);
  }


//same direction
void same_s_step(float degree, bool dir){
  f_s_step(degree, dir);
  b_s_step(degree, dir);
  }

void f_s_step(float degree, bool dir){
  fl_s_step(degree, dir);
  fr_s_step(degree, dir);
  }

void b_s_step(float degree, bool dir){
  bl_s_step(degree, dir);
  br_s_step(degree, dir);
  }
// if dir is high go cc, cc is considered negative in counting (i.e -90 degree's)
void fl_s_step(float degree, bool dir){
  int steps = deg_to_step(degree);
  fl_s_pos_update(degree, dir);
  if (fl_s_pos > 90 || fl_s_pos < -90){
    steps = 0;
    fl_s_pos_update(degree, !dir);
    }
  digitalWrite(fl_s_ndir, !dir);
  digitalWrite(fl_s_pdir, dir);
  digitalWrite(fl_s_npwm, LOW);
  
  
  for (int i = 0; i < abs(steps); i++) {
    digitalWrite(fl_s_ppwm, HIGH);
    delayMicroseconds(50000);
    digitalWrite(fl_s_ppwm, LOW);
    
  }
}

void fr_s_step(float degree, bool dir){
  int steps = deg_to_step(degree);
  fr_s_pos_update(degree, dir);
  if (fr_s_pos > 90 || fr_s_pos < -90){
    steps = 0;
    fr_s_pos_update(degree, !dir);
    }
  digitalWrite(fr_s_ndir, !dir);
  digitalWrite(fr_s_pdir, dir);
  digitalWrite(fr_s_npwm, LOW);
  
  
  for (int i = 0; i < abs(steps); i++) {
    digitalWrite(fr_s_ppwm, HIGH);
    delayMicroseconds(50000);
    digitalWrite(fr_s_ppwm, LOW);
    
  }
}


void bl_s_step(float degree, bool dir){
  int steps = deg_to_step(degree);
  bl_s_pos_update(degree, dir);
  if (bl_s_pos > 90 || bl_s_pos < -90){
    steps = 0;
    bl_s_pos_update(degree, !dir);
    }
  digitalWrite(bl_s_ndir, !dir);
  digitalWrite(bl_s_pdir, dir);
  digitalWrite(bl_s_npwm, LOW);
  
  
  for (int i = 0; i < abs(steps); i++) {
    digitalWrite(bl_s_ppwm, HIGH);
    delayMicroseconds(50000);
    digitalWrite(bl_s_ppwm, LOW);
    
  }
}

void br_s_step(float degree, bool dir){
  int steps = deg_to_step(degree);
  br_s_pos_update(degree, dir);
  if (br_s_pos > 90 || br_s_pos < -90){
    steps = 0;
    br_s_pos_update(degree, !dir);
    }
  digitalWrite(br_s_ndir, !dir);
  digitalWrite(br_s_pdir, dir);
  digitalWrite(br_s_npwm, LOW);
  
  
  for (int i = 0; i < abs(steps); i++) {
    digitalWrite(br_s_ppwm, HIGH);
    delayMicroseconds(50000);
    digitalWrite(br_s_ppwm, LOW);
    
  }
}

int deg_to_step(float degree){
  float steps = degree*(10000/360); // 10,000 steps/360 degree's times the number of degree's = steps to take
  return steps;
  }

void fl_s_pos_update(float degree, bool dir){
  int sign = 0;
  switch(dir){
    case true:
      sign = 1;
      break;
    case false:
      sign = -1;
      break;
    }
  fl_s_pos += sign*abs(degree);
  if (fl_s_pos > 90){
    fl_s_pos = 91;
    }
  if (fl_s_pos < -90){
    fl_s_pos = -91;
    }
  }

void fr_s_pos_update(float degree, bool dir){
  int sign = 0;
  switch(dir){
    case true:
      sign = 1;
      break;
    case false:
      sign = -1;
      break;
    }
  fr_s_pos += sign*abs(degree);
  if (fr_s_pos > 90){
    fr_s_pos = 91;
    }
  if (fr_s_pos < -90){
    fr_s_pos = -91;
    }
  }

void bl_s_pos_update(float degree, bool dir){
  int sign = 0;
  switch(dir){
    case true:
      sign = 1;
      break;
    case false:
      sign = -1;
      break;
    }
  bl_s_pos += sign*abs(degree);
    if (bl_s_pos > 90){
    bl_s_pos = 91;
    }
  if (bl_s_pos < -90){
    bl_s_pos = -91;
    }
  }

void br_s_pos_update(float degree, bool dir){
  int sign = 0;
  switch(dir){
    case true:
      sign = 1;
      break;
    case false:
      sign = -1;
      break;
    }
  br_s_pos += sign*abs(degree);
    if (br_s_pos > 90){
    br_s_pos = 91;
    }
  if (br_s_pos < -90){
    br_s_pos = -91;
    }
  }

  
//--------------------------------------//
//           motor Enables   
//              archive?
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
  bool dir = duty > 0;
  fl_motor_dir(dir);
  analogWrite(fl_d_pwm, abs(duty));
  }

void fr_motor_PWM(float duty){
  bool dir = duty > 0;
  fr_motor_dir(dir);
  analogWrite(fr_d_pwm, abs(duty));
  }
//back left
void bl_motor_PWM(float duty){
  bool dir = duty > 0;
  bl_motor_dir(dir);
  analogWrite(bl_d_pwm, abs(duty));
  }
//back right
void br_motor_PWM(float duty){
  bool dir = duty > 0;
  br_motor_dir(dir);
  analogWrite(br_d_pwm, abs(duty));
  }
//
////--------------------------------------//
////           steppers Dir      
////              Archive?
////--------------------------------------//
//
//void front_steppers_dir(bool reverse){
//  
//  }
//
//void back_steppers_dir(bool reverse){
//  
//  }
//
//void fl_stepper_ndir(bool reverse){
//  digitalWrite(fl_s_ndir, reverse);
//  }
//
//void fr_stepper_ndir(bool reverse){
//   digitalWrite(fr_s_ndir, reverse);
//  }
////back left
//void bl_stepper_ndir(bool reverse){
//   digitalWrite(bl_s_ndir, reverse);
//  }
////back right
//void br_stepper_ndir(bool reverse){
//   digitalWrite(br_s_ndir, reverse);
//  }
//
//
//void fl_stepper_pdir(bool reverse){
//  digitalWrite(fl_s_pdir, reverse);
//  }
//
//void fr_stepper_pdir(bool reverse){
//   digitalWrite(fr_s_pdir, reverse);
//  }
////back left
//void bl_stepper_pdir(bool reverse){
//   digitalWrite(bl_s_pdir, reverse);
//  }
////back right
//void br_stepper_pdir(bool reverse){
//   digitalWrite(br_s_pdir, reverse);
//  }
//
////--------------------------------------//
////                Stepper PWM       
////                  Archive?
////--------------------------------------//
//
//
//void fl_stepper_PPWM(bool duty){
//  digitalWrite(fl_s_ppwm, duty);
//  }
//
//void fr_stepper_PPWM(bool duty){
//  digitalWrite(fr_s_ppwm, duty);
//  }
////back left
//void bl_stepper_PPWM(bool duty){
//  digitalWrite(bl_s_ppwm, duty);
//  }
////back right
//void br_stepper_PPWM(bool duty){
//  digitalWrite(br_s_ppwm, duty);
//  }
//
//
//void fl_stepper_NPWM(bool duty){
//  digitalWrite(fl_s_npwm, duty);
//  }
//
//void fr_stepper_NPWM(bool duty){
//  digitalWrite(fr_s_npwm, duty);
//  }
////back left
//void bl_stepper_NPWM(bool duty){
//  digitalWrite(bl_s_npwm, duty);
//  }
////back right
//void br_stepper_NPWM(bool duty){
//  digitalWrite(br_s_npwm, duty);
//  }




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
