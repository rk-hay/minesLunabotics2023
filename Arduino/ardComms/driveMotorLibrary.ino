// This file contains the velocity controllers. The goal is that when we input a velocity it sets 
// an intial PWM value then tracks the encoder updates and if the encoders seem too slow, it speeds 
// it up and if they are two fast, it'll slow it down.

//return the angular speed *aprox*
float ang_vel(){
  float front_angle = PI/180*((fl_s_pos+fr_s_pos)/2);
  float rear_angle = PI/180*((bl_s_pos+br_s_pos)/2);
  float linear_velocity_approx = (fl_d_vel()*cos(fl_s_pos*PI/180)+fr_d_vel()*cos(fr_s_pos*PI/180)+bl_d_vel()*cos(-bl_s_pos*PI/180)+br_d_vel()*cos(-br_s_pos*PI/180))/4;
  float angular_velocity = ((linear_velocity_approx*cos((front_angle+rear_angle)/2))/(front_angle + rear_angle))*(front_angle - rear_angle);
  if (isnan(angular_velocity)) {angular_velocity = 0;}
  return angular_velocity;
}




float fl_d_vel(){
    unsigned long myTime = micros();
    static float prevTime = 0;
    static int prev_pos_fl = 0;
    readEncoders();
    float velocity = (1.0/enc_per_rev)*(PI*wheel_diameter)*((fl_enc_pos-prev_pos_fl)/((myTime-prevTime) * .000001));
    prev_pos_fl = fl_enc_pos;
    prevTime = myTime;

  return velocity;
  }


float fr_d_vel(){
  
  
    unsigned long myTime = micros();
    static float prevTime = 0;
    static int prev_pos_fr = 0;
    readEncoders();
    float velocity = (1.0/enc_per_rev)*(PI*wheel_diameter)*((fr_enc_pos-prev_pos_fr)/((myTime-prevTime) * .000001));
    prev_pos_fr = fr_enc_pos;
    prevTime = myTime;
  
  
  return velocity;
  }

float bl_d_vel(){
  
  
    unsigned long myTime = micros();
    static float prevTime = 0;
    static int prev_pos_bl = 0;
    readEncoders();
    float velocity = (1.0/enc_per_rev)*(PI*wheel_diameter)*((bl_enc_pos-prev_pos_bl)/((myTime-prevTime) * .000001));
    prev_pos_bl = bl_enc_pos;
    prevTime = myTime;
    
  
  return velocity;
  }


float br_d_vel(){
    
    unsigned long myTime = micros();
    static float prevTime = 0;
    static int prev_pos_br = 0;
    readEncoders();
    float velocity = (1.0/enc_per_rev)*(PI*wheel_diameter)*((br_enc_pos-prev_pos_br)/((myTime-prevTime) * .000001));
    prev_pos_br = br_enc_pos;
    prevTime = myTime;
  
  
  return velocity;
  }
  // m/s = (1 rev / 5281 counts) * (pi*.345 m)/1 rev*((number of current counts - number of previous counts)/seconds passed)  
 
float fl_pos(){
  static int prev_pos_fl = 0;
  static float pos = 0;
  pos += ((1.0/enc_per_rev)*(fl_enc_pos-prev_pos_fl)*PI*wheel_diameter);
  float fl_s_pos2 = fl_rotations + pos;
  prev_pos_fl = fl_enc_pos;
  return fl_s_pos2;
  }

float fr_pos(){
  static int prev_pos_fr = 0;
  static float pos = 0;
  pos += (1.0/enc_per_rev)*(fr_enc_pos-prev_pos_fr)*PI*wheel_diameter;
  float fr_s_pos2 = fr_rotations + pos;
  prev_pos_fr = fr_enc_pos;
  return fr_s_pos2;
  }

float bl_pos(){
  static int prev_pos_bl = 0;
  static float pos = 0;
  pos += (1.0/enc_per_rev)*(bl_enc_pos-prev_pos_bl)*PI*wheel_diameter;
  float bl_s_pos2 = bl_rotations + pos;
  prev_pos_bl = bl_enc_pos;
  return bl_s_pos2;
  }

float br_pos(){
  static int prev_pos_br = 0;
  static float pos = 0;
  pos += (1.0/enc_per_rev)*(br_enc_pos-prev_pos_br)*PI*wheel_diameter;
  float br_s_pos2 = br_rotations + pos;
  prev_pos_br = br_enc_pos;
  return br_s_pos2;
  }
