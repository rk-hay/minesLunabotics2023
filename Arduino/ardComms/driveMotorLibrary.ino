// This file contains the velocity controllers. The goal is that when we input a velocity it sets 
// an intial PWM value then tracks the encoder updates and if the encoders seem too slow, it speeds 
// it up and if they are two fast, it'll slow it down.

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
  fl_s_pos = fl_rotations + pos;
  prev_pos_fl = fl_enc_pos;
  return fl_s_pos;
  }

float fr_pos(){
  static int prev_pos_fr = 0;
  static float pos = 0;
  pos += (1.0/enc_per_rev)*(fr_enc_pos-prev_pos_fr)*PI*wheel_diameter;
  fr_s_pos = fr_rotations + pos;
  prev_pos_fr = fr_enc_pos;
  return fr_s_pos;
  }

float bl_pos(){
  static int prev_pos_bl = 0;
  static float pos = 0;
  pos += (1.0/enc_per_rev)*(bl_enc_pos-prev_pos_bl)*PI*wheel_diameter;
  bl_s_pos = bl_rotations + pos;
  prev_pos_bl = bl_enc_pos;
  return bl_s_pos;
  }

float br_pos(){
  static int prev_pos_br = 0;
  static float pos = 0;
  pos += (1.0/enc_per_rev)*(br_enc_pos-prev_pos_br)*PI*wheel_diameter;
  br_s_pos = br_rotations + pos;
  prev_pos_br = br_enc_pos;
  return br_s_pos;
  }
