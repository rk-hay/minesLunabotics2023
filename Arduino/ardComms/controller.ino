//tunable variables
const int Kp_vel = 2;
const int Ki_vel = 1;
const int Kd_vel = 0;

const int Kp_pos = 2;
const int Ki_pos = 1;
const int Kd_pos = 0;

const int Kp_ang = 10;
const int Ki_ang = 1;
const int Kd_ang = 1;

//random globals I need
uint32_t last_millis = 0;
float postional_mode = false;




void control_loop(){
  
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
      fl_target_v = Kp_pos*p_error+Ki_pos*fl_p_I+fl_p_D*Kd_pos;
  
      p_error = fr_target_p - fr_pos();
      fr_p_I = max(-MAX_INTEGRAL, min(MAX_INTEGRAL, fr_p_I + p_error*time_change));
      fr_p_D = (p_error - fr_p_last_error)/time_change;
      fr_p_last_error = p_error;
      fr_target_v = Kp_pos*p_error+Ki_pos*fr_p_I+fr_p_D*Kd_pos;

      p_error = bl_target_p - bl_pos();
      bl_p_I = max(-MAX_INTEGRAL, min(MAX_INTEGRAL, bl_p_I + p_error*time_change));
      bl_p_D = (p_error - bl_p_last_error)/time_change;
      bl_p_last_error = p_error;
      bl_target_v = Kp_pos*p_error+Ki_pos*bl_p_I+bl_p_D*Kd_pos;
  
      p_error = br_target_p - br_pos();
      br_p_I = max(-MAX_INTEGRAL, min(MAX_INTEGRAL, br_p_I + p_error*time_change));
      br_p_D = (p_error - br_p_last_error)/time_change;
      br_p_last_error = p_error;
      br_target_v = Kp_pos*p_error+Ki_pos*br_p_I+br_p_D*Kd_pos;
    }
  
  //fl PD loop (no I integrated yet)
  // add derivative and integral term to velocity controller see fl above
  v_error = fl_target_v - fl_d_vel();
  fl_v_I = max(-MAX_V_INTEGRAL, min(MAX_V_INTEGRAL, fl_v_I+v_error*time_change));
  fl_v_D = (v_error - fl_v_last_error)/time_change;
  fl_v_last_error = v_error;
  float fl_updated_vel = max(0, min(MAX_PWM, Kp_vel*v_error+Kd_vel*fl_v_D+Ki_vel*fl_v_I));
  fl_motor_PWM(vel_to_pwm(fl_updated_vel));

  v_error = fr_target_v - fr_d_vel();
  fr_v_I = max(-MAX_V_INTEGRAL, min(MAX_V_INTEGRAL, fr_v_I+v_error*time_change));
  fr_v_D = (v_error - fr_v_last_error)/time_change;
  fr_v_last_error = v_error;
  float fr_updated_vel = max(0, min(MAX_PWM, Kp_vel*v_error+Kd_vel*fr_v_D+Ki_vel*fr_v_I));
  fr_motor_PWM(vel_to_pwm(fr_updated_vel));

  v_error = bl_target_v - bl_d_vel();
  bl_v_I = max(-MAX_V_INTEGRAL, min(MAX_V_INTEGRAL, bl_v_I+v_error*time_change));
  bl_v_D = (v_error - fl_v_last_error)/time_change;
  bl_v_last_error = v_error;
  float bl_updated_vel = max(0, min(MAX_PWM, Kp_vel*v_error+Kd_vel*bl_v_D+Ki_vel*fl_v_I));
  bl_motor_PWM(vel_to_pwm(bl_updated_vel));

  v_error = br_target_v - br_d_vel();
  br_v_I = max(-MAX_V_INTEGRAL, min(MAX_V_INTEGRAL, br_v_I+v_error*time_change));
  br_v_D = (v_error - br_v_last_error)/time_change;
  br_v_last_error = v_error;
  float br_updated_vel = max(0, min(MAX_PWM, Kp_vel*v_error+Kd_vel*br_v_D+Ki_vel*br_v_I));
  br_motor_PWM(vel_to_pwm(br_updated_vel));
  

  //----------------------//
  //begin angular velocity//
  //----------------------// 
  static float angular_velocity_last_error = 0;
  v_error = target_angular_velocity - ang_vel();
  angular_velocity_I = max(-MAX_ANGULAR_VEL, min(MAX_ANGULAR_VEL, angular_velocity_I+(v_error*time_change)));
  angular_velocity_D = (v_error-angular_velocity_last_error)/time_change;
  float updated_angular_vel = (v_error*Kp_ang)+(Ki_ang*angular_velocity_I)+(Kd_ang*angular_velocity_D);
  global_angle_select(updated_angular_vel, updated_angular_vel, -updated_angular_vel/3, -updated_angular_vel/3);


  }//end control loop

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
  float pwm = (255/.4)*vel;
  return pwm;
}

//goal to take angular velocities and produce a desired wheel angle
void angV_to_deg(float ang_V){
  float del = 0;
  float time_change = millis()-del;
  del = millis();
  return ang_V*time_change*.001*(180/PI);  //rads/S * mS * 1s/10^3 mS * 180 degree's/PI rads 
}
