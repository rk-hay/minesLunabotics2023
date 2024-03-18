//tunable variables
const int aKp_vel = 2;
const int aKi_vel = 0;
const int aKd_vel = 0;

const int aKp_pos = 2;
const int aKi_pos = 0;
const int aKd_pos = 0;

const int aKp_ang = 2;
const int aKi_ang = 0;
const int aKd_ang = 0;


//tunable variables
const float cKp_vel = 1; // kP shouldn’t be greater than 1/(max error) max errors is 2m/s so 500 PWM? (target is 1/-1 m/s robot is going -1/1 m/s difference is 500 (255 to -255))
const float cKi_vel = 0.0; ////change to feed forward????
const float cKd_vel = 0.000;

const int cKp_pos = 1/10; // Max error is 10 becauce arena size is 10... ig soooo 1/10?
const int cKi_pos = 1;
const int cKd_pos = 0;

const int cKp_ang = 2;
const int cKi_ang = 0;
const int cKd_ang = 0;

//random globals I need
uint32_t last_millis = 0;
float postional_mode = false;




void auto_control_loop(){
  
  float v_error = 0; //reusable P term for vel
  float p_error = 0;  //reusable P term for postion

  static float fl_p_I = 0;    // I terms
  static float fr_p_I = 0;
  static float bl_p_I = 0;
  static float br_p_I = 0;
  static float fl_v_I = 0;
  static float fr_v_I = 0;
  static float bl_v_I = 0;
  static float br_v_I = 0;

  static float fl_v_D = 0;  // D terms
  static float fr_v_D = 0; 
  static float bl_v_D = 0; 
  static float br_v_D = 0; 

  static float fl_p_D = 0; //
  static float fr_p_D = 0; 
  static float bl_p_D = 0; 
  static float br_p_D = 0; 

  static float fl_p_last_error = 0; // d term prev error for pos
  static float fr_p_last_error = 0;
  static float bl_p_last_error = 0;
  static float br_p_last_error = 0;

  static float fl_v_last_error = 0; // d term prev error for vel
  static float fr_v_last_error = 0;
  static float bl_v_last_error = 0;
  static float br_v_last_error = 0;
  
  static float angular_velocity_I = 0;
  static float angular_velocity_D = 0;

  float time_change = (millis() - last_millis)/1000.0f; // del D
  last_millis = millis(); 
  //Postional controller // needs work //
  if (postional_mode == true){
    
      p_error = fl_target_p - fl_pos();
      fl_p_I = max(-MAX_INTEGRAL, min(MAX_INTEGRAL, fl_p_I + p_error*time_change));
      fl_p_D = (p_error - fl_p_last_error)/time_change;
      fl_p_last_error = p_error;
      fl_target_v = aKp_pos*p_error+aKi_pos*fl_p_I+fl_p_D*aKd_pos;
  
      p_error = fr_target_p - fr_pos();
      fr_p_I = max(-MAX_INTEGRAL, min(MAX_INTEGRAL, fr_p_I + p_error*time_change));
      fr_p_D = (p_error - fr_p_last_error)/time_change;
      fr_p_last_error = p_error;
      fr_target_v = aKp_pos*p_error+aKi_pos*fr_p_I+fr_p_D*aKd_pos;

      p_error = bl_target_p - bl_pos();
      bl_p_I = max(-MAX_INTEGRAL, min(MAX_INTEGRAL, bl_p_I + p_error*time_change));
      bl_p_D = (p_error - bl_p_last_error)/time_change;
      bl_p_last_error = p_error;
      bl_target_v = aKp_pos*p_error+aKi_pos*bl_p_I+bl_p_D*aKd_pos;
  
      p_error = br_target_p - br_pos();
      br_p_I = max(-MAX_INTEGRAL, min(MAX_INTEGRAL, br_p_I + p_error*time_change));
      br_p_D = (p_error - br_p_last_error)/time_change;
      br_p_last_error = p_error;
      br_target_v = aKp_pos*p_error+aKi_pos*br_p_I+br_p_D*aKd_pos;
    }
  
  //fl PD loop (no I integrated yet)
  // add derivative and integral term to velocity controller see fl above
  v_error = fl_target_v - fl_d_vel();
  fl_v_I = max(-MAX_V_INTEGRAL, min(MAX_V_INTEGRAL, fl_v_I+v_error*time_change));
  fl_v_D = (v_error - fl_v_last_error)/time_change;
  fl_v_last_error = v_error;
  float fl_updated_vel = max(-MAX_PWM, min(MAX_PWM, aKp_vel*v_error+aKd_vel*fl_v_D+aKi_vel*fl_v_I));
  fl_motor_PWM(vel_to_pwm(fl_updated_vel));

  v_error = fr_target_v - fr_d_vel();
  fr_v_I = max(-MAX_V_INTEGRAL, min(MAX_V_INTEGRAL, fr_v_I+v_error*time_change));
  fr_v_D = (v_error - fr_v_last_error)/time_change;
  fr_v_last_error = v_error;
  float fr_updated_vel = max(-MAX_PWM, min(MAX_PWM, aKp_vel*v_error+aKd_vel*fr_v_D+aKi_vel*fr_v_I));
  fr_motor_PWM(vel_to_pwm(fr_updated_vel));

  v_error = bl_target_v - bl_d_vel();
  bl_v_I = max(-MAX_V_INTEGRAL, min(MAX_V_INTEGRAL, bl_v_I+v_error*time_change));
  bl_v_D = (v_error - fl_v_last_error)/time_change;
  bl_v_last_error = v_error;
  float bl_updated_vel = max(-MAX_PWM, min(MAX_PWM, aKp_vel*v_error+aKd_vel*bl_v_D+aKi_vel*fl_v_I));
  bl_motor_PWM(vel_to_pwm(bl_updated_vel));

  v_error = br_target_v - br_d_vel();
  br_v_I = max(-MAX_V_INTEGRAL, min(MAX_V_INTEGRAL, br_v_I+v_error*time_change));
  br_v_D = (v_error - br_v_last_error)/time_change;
  br_v_last_error = v_error;
  float br_updated_vel = max(-MAX_PWM, min(MAX_PWM, aKp_vel*v_error+aKd_vel*br_v_D+aKi_vel*br_v_I));
  br_motor_PWM(vel_to_pwm(br_updated_vel));
  

  //----------------------//
  //begin angular velocity//
  //----------------------// 
  // static float angular_velocity_last_error = 0;
  // v_error = target_angular_velocity - ang_vel();
  // angular_velocity_I = max(-MAX_ANGULAR_VEL, min(MAX_ANGULAR_VEL, angular_velocity_I+(v_error*time_change)));
  // angular_velocity_D = (v_error-angular_velocity_last_error)/time_change;
  // float updated_angular_vel = (v_error*aKp_ang)+(aKi_ang*angular_velocity_I)+(aKd_ang*angular_velocity_D);
  // global_angle_select(updated_angular_vel, updated_angular_vel, -updated_angular_vel/3, -updated_angular_vel/3);


  }//end control loop


void controller_control_loop(){
  
  float v_error = 0; //reusable P term for vel
  float p_error = 0;  //reusable P term for postion

  static float fl_p_I = 0;    // I terms
  static float fr_p_I = 0;
  static float bl_p_I = 0;
  static float br_p_I = 0;
  static float fl_v_I = 0;
  static float fr_v_I = 0;
  static float bl_v_I = 0;
  static float br_v_I = 0;

  static float fl_v_D = 0;  // D terms
  static float fr_v_D = 0; 
  static float bl_v_D = 0; 
  static float br_v_D = 0; 

  static float fl_p_D = 0;
  static float fr_p_D = 0; 
  static float bl_p_D = 0; 
  static float br_p_D = 0; 

  static float fl_p_last_error = 0; // d term prev error for pos
  static float fr_p_last_error = 0;
  static float bl_p_last_error = 0;
  static float br_p_last_error = 0;

  static float fl_v_last_error = 0; // d term prev error for vel
  static float fr_v_last_error = 0;
  static float bl_v_last_error = 0;
  static float br_v_last_error = 0;
  
  static float angular_velocity_I = 0;
  static float angular_velocity_D = 0;

  float time_change = (millis() - last_millis)/1000.0f; // del D
  last_millis = millis(); 
  //Postional controller // needs work //
  if (postional_mode == true){
    
      p_error = fl_target_p - fl_pos();
      fl_p_I = max(-MAX_INTEGRAL, min(MAX_INTEGRAL, fl_p_I + p_error*time_change));
      fl_p_D = (p_error - fl_p_last_error)/time_change;
      fl_p_last_error = p_error;
      fl_target_v = cKp_pos*p_error+cKi_pos*fl_p_I+fl_p_D*cKd_pos;
  
      p_error = fr_target_p - fr_pos();
      fr_p_I = max(-MAX_INTEGRAL, min(MAX_INTEGRAL, fr_p_I + p_error*time_change));
      fr_p_D = (p_error - fr_p_last_error)/time_change;
      fr_p_last_error = p_error;
      fr_target_v = cKp_pos*p_error+cKi_pos*fr_p_I+fr_p_D*cKd_pos;

      p_error = bl_target_p - bl_pos();
      bl_p_I = max(-MAX_INTEGRAL, min(MAX_INTEGRAL, bl_p_I + p_error*time_change));
      bl_p_D = (p_error - bl_p_last_error)/time_change;
      bl_p_last_error = p_error;
      bl_target_v = cKp_pos*p_error+cKi_pos*bl_p_I+bl_p_D*cKd_pos;
  
      p_error = br_target_p - br_pos();
      br_p_I = max(-MAX_INTEGRAL, min(MAX_INTEGRAL, br_p_I + p_error*time_change));
      br_p_D = (p_error - br_p_last_error)/time_change;
      br_p_last_error = p_error;
      br_target_v = cKp_pos*p_error+cKi_pos*br_p_I+br_p_D*cKd_pos;
    }
  
  //fl PD loop (no I integrated yet)
  // add derivative and integral term to velocity controller see fl above
  v_error = fl_target_v - fl_d_vel();
  fl_v_I = max(-MAX_V_INTEGRAL, min(MAX_V_INTEGRAL, fl_v_I+v_error*time_change));
  fl_v_D = (v_error - fl_v_last_error)/time_change;
  fl_v_last_error = v_error;
  float fl_updated_vel = max(-MAX_PWM, min(MAX_PWM, cKp_vel*v_error+cKd_vel*fl_v_D+cKi_vel*fl_v_I));
  fl_motor_PWM(vel_to_pwm(fl_updated_vel));

  v_error = fr_target_v - fr_d_vel();
  fr_v_I = max(-MAX_V_INTEGRAL, min(MAX_V_INTEGRAL, fr_v_I+v_error*time_change));
  fr_v_D = (v_error - fr_v_last_error)/time_change;
  fr_v_last_error = v_error;
  float fr_updated_vel = max(-MAX_PWM, min(MAX_PWM, cKp_vel*v_error+cKd_vel*fr_v_D+cKi_vel*fr_v_I));
  fr_motor_PWM(vel_to_pwm(fr_updated_vel));

  v_error = bl_target_v - bl_d_vel();
  bl_v_I = max(-MAX_V_INTEGRAL, min(MAX_V_INTEGRAL, bl_v_I+v_error*time_change));
  bl_v_D = (v_error - fl_v_last_error)/time_change;
  bl_v_last_error = v_error;
  float bl_updated_vel = max(-MAX_PWM, min(MAX_PWM, cKp_vel*v_error+cKd_vel*bl_v_D+cKi_vel*fl_v_I));
  bl_motor_PWM(vel_to_pwm(bl_updated_vel));

  v_error = br_target_v - br_d_vel();
  br_v_I = max(-MAX_V_INTEGRAL, min(MAX_V_INTEGRAL, br_v_I+v_error*time_change));
  br_v_D = (v_error - br_v_last_error)/time_change;
  br_v_last_error = v_error;
  float br_updated_vel = max(-MAX_PWM, min(MAX_PWM, cKp_vel*v_error+cKd_vel*br_v_D+cKi_vel*br_v_I));
  br_motor_PWM(vel_to_pwm(br_updated_vel));
  

  //----------------------//
  //begin angular velocity//
  //----------------------// 
  // static float angular_velocity_last_error = 0;
  // v_error = target_angular_velocity - ang_vel();
  // angular_velocity_I = max(-MAX_ANGULAR_VEL, min(MAX_ANGULAR_VEL, angular_velocity_I+(v_error*time_change)));
  // angular_velocity_D = (v_error-angular_velocity_last_error)/time_change;
  // float updated_angular_vel = (v_error*cKp_ang)+(cKi_ang*angular_velocity_I)+(cKd_ang*angular_velocity_D);
  // global_angle_select(updated_angular_vel, updated_angular_vel, -updated_angular_vel/3, -updated_angular_vel/3);


  }//end control loop

void control_vel_updated(float v[4]){
  control_vel_fl(v[0]);
  control_vel_fr(v[1]);
  control_vel_bl(v[2]);
  control_vel_br(v[3]);
}

void control_vel(float v_linear, float v_angular){
  control_vel_fl(v_linear + (robot_width/2.0f)*v_angular);
  control_vel_fr(v_linear + (robot_width/2.0f)*v_angular);
  control_vel_br(v_linear + (robot_width/2.0f)*v_angular);
  control_vel_bl(v_linear + (robot_width/2.0f)*v_angular);
  }

void control_ang_v(float v){
  target_angular_velocity = v;
}

void control_vel_fl(float v){
  fl_target_v = v;
  postional_mode = false;
  }

void control_vel_fr(float v){
  fr_target_v = v;
  postional_mode = false;
  }

void control_vel_bl(float v){
  bl_target_v = v;
  postional_mode = false;
  }

void control_vel_br(float v){
  br_target_v = v;
  postional_mode = false;
  }


void control_pos(float p){
  control_pos_fl(p);
  control_pos_fr(p);
  control_pos_br(p);
  control_pos_bl(p);
  }


void control_pos_fl(float p){
  fl_target_p = p;
  postional_mode = true;
  }

void control_pos_fr(float p){
  fr_target_p = p;
  postional_mode = true;
  }

void control_pos_bl(float p){
  bl_target_p = p;
  postional_mode = true;
  }

void control_pos_br(float p){
  br_target_p = p;
  postional_mode = true;
  }

//UPDATE FROM .4
float vel_to_pwm(float vel){
  float pwm = (255/1)*vel;
  return pwm;
}
