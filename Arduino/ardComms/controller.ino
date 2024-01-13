//
const float Kp_vel = 2;
const float Ki_vel = 0;
const float Kd_vel = 0;

const float Kp_pos = 2;
const float Ki_pos = 1;
const float Kd_pos = 5;
uint32_t last_millis = 0;
float postional_mode = false;

void control_loop(){
  
  float v_error = 0;
  float p_error = 0;
  static float fl_int = 0;
  static float fr_int = 0;
  static float bl_int = 0;
  static float br_int = 0;
  static float fl_v_int = 0;
  static float fr_v_int = 0;
  static float bl_v_int = 0;
  static float br_v_int = 0;
  static float fl_v_error = 0; 
  static float fr_v_error = 0; 
  static float bl_v_error = 0; 
  static float br_v_error = 0; 
  static float fl_p_error = 0; 
  static float fr_p_error = 0; 
  static float bl_p_error = 0; 
  static float br_p_error = 0; 
  
  static float fl_last_error = 0;
  static float fr_last_error = 0;
  static float bl_last_error = 0;
  static float br_last_error = 0;

  static float fl_v_last_error = 0;
  static float fr_v_last_error = 0;
  static float bl_v_last_error = 0;
  static float br_v_last_error = 0;
  
  float time_change = (millis() - last_millis)/1000.0f;
  last_millis = millis();
  if (postional_mode == true){
    
      p_error = fl_target_p - fl_pos();
      fl_int = max(-MAX_INTEGRAL, min(MAX_INTEGRAL, fl_int + p_error*time_change));
      fl_p_error = p_error - fl_last_error;
      fl_last_error = p_error;
      fl_target_v = Kp_pos*p_error+Ki_pos*fl_int+fl_p_error*Kd_pos;
  
      p_error = fr_target_p - fr_pos();
      fr_int = max(-MAX_INTEGRAL, min(MAX_INTEGRAL, fr_int + p_error*time_change));
      fr_target_v = Kp_pos*p_error+Ki_pos*fr_int;

      p_error = bl_target_p - bl_pos();
      bl_int = max(-MAX_INTEGRAL, min(MAX_INTEGRAL, bl_int + p_error*time_change));
      bl_target_v = Kp_pos*p_error+Ki_pos*bl_int;

      p_error = br_target_p - br_pos();
      br_int = max(-MAX_INTEGRAL, min(MAX_INTEGRAL, br_int + p_error*time_change));
      br_target_v = Kp_pos*p_error+Ki_pos*br_int;
    }
  
  //fl PD loop (no I integrated yet)
  // add derivative and integral term to velocity controller see fl above
  v_error = abs(fl_target_v) - abs(fl_d_vel());
  fl_v_int = max(-MAX_V_INTEGRAL, min(MAX_V_INTEGRAL, fl_v_int+v_error*time_change));
  fl_v_error = v_error - fl_v_last_error;
  fl_v_last_error = v_error;
  fl_pwm = max(0, min(MAX_PWM, fl_pwm+Kp_vel*v_error+Kd_vel*fl_v_error+Kp_vel*fl_v_int));
  fl_dir = fl_target_v > 0;
  fl_motor_PWM(fl_pwm, fl_dir);



  v_error = abs(fr_target_v) - abs(fr_d_vel());
  fr_pwm = max(0, min(MAX_PWM, fr_pwm +Kp_vel*v_error));
  fr_dir = fr_target_v > 0;
  fr_motor_PWM(fr_pwm, fr_dir);

  v_error = abs(bl_target_v) - abs(bl_d_vel());
  bl_pwm = max(0, min(MAX_PWM, bl_pwm +Kp_vel*v_error));
  bl_dir = fl_target_v > 0;
  bl_motor_PWM(bl_pwm, bl_dir);

  v_error = abs(br_target_v) - abs(br_d_vel());
  br_pwm = max(0, min(MAX_PWM, br_pwm +Kp_vel*v_error));
  br_dir = br_target_v > 0;
  br_motor_PWM(br_pwm, br_dir);
  
  }

void control_vel(float v_linear, float v_angular){
  control_vel_fl(v_linear + (robot_width/2.0f)*v_angular);
  control_vel_fr(v_linear + (robot_width/2.0f)*v_angular);
  control_vel_br(v_linear + (robot_width/2.0f)*v_angular);
  control_vel_bl(v_linear + (robot_width/2.0f)*v_angular);
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
