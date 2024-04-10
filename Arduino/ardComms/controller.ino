
//tunable variables
const float cKp_vel = 1; // kP shouldn’t be greater than 1/(max error) max errors is 2m/s so 500 PWM? (target is 1/-1 m/s robot is going -1/1 m/s difference is 500 (255 to -255))
const float cKi_vel = 0.0; ////change to feed forward????
const float cKd_vel = 0.000;

//random globals I need
uint32_t last_millis = 0;
float postional_mode = false;

#define NUM_WHEELS 4

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
  
  //fl PD loop (no I integrated yet)
  // add derivative and integral term to velocity controller see fl above
  v_error = fl_target_v - fl_d_vel(); //CONSIDER CHANGING SUCH AS IF TARGET CHANGES RESET INTEGRAL OR SMTHN OPTIONS: EVERY TIME ERROR = 0 INT = 0, OR ONLY USE INTEGRAL WITHIN A SMALL REGION, OR RESET THE INTEGRAL UPON RECIEVING A NEW GOAL
  fl_v_I = max(-MAX_V_INTEGRAL, min(MAX_V_INTEGRAL, fl_v_I+v_error*time_change));
  fl_v_D = (v_error - fl_v_last_error)/time_change;
  fl_v_last_error = v_error;
  float fl_updated_vel = max(-MAX_VEL, min(MAX_VEL, cKp_vel*v_error+cKd_vel*fl_v_D+cKi_vel*fl_v_I));
  

  v_error = fr_target_v - fr_d_vel();
  fr_v_I = max(-MAX_V_INTEGRAL, min(MAX_V_INTEGRAL, fr_v_I+v_error*time_change));
  fr_v_D = (v_error - fr_v_last_error)/time_change;
  fr_v_last_error = v_error;
  float fr_updated_vel = max(-MAX_VEL, min(MAX_VEL, cKp_vel*v_error+cKd_vel*fr_v_D+cKi_vel*fr_v_I));
  


  v_error = bl_target_v - bl_d_vel();
  bl_v_I = max(-MAX_V_INTEGRAL, min(MAX_V_INTEGRAL, bl_v_I+v_error*time_change));
  bl_v_D = (v_error - fl_v_last_error)/time_change;
  bl_v_last_error = v_error;
  float bl_updated_vel = max(-MAX_VEL, min(MAX_VEL, cKp_vel*v_error+cKd_vel*bl_v_D+cKi_vel*fl_v_I));
  


  v_error = br_target_v - br_d_vel();
  br_v_I = max(-MAX_V_INTEGRAL, min(MAX_V_INTEGRAL, br_v_I+v_error*time_change));
  br_v_D = (v_error - br_v_last_error)/time_change;
  br_v_last_error = v_error;
  float br_updated_vel = max(-MAX_VEL, min(MAX_VEL, cKp_vel*v_error+cKd_vel*br_v_D+cKi_vel*br_v_I));
  
  float updated_vels[4] ={fl_updated_vel, fr_updated_vel, bl_updated_vel, br_updated_vel};
  float vels[4] = {fl_d_vel(), fr_d_vel(), bl_d_vel(), br_d_vel()};

    for (int i = 0; i < NUM_WHEELS; i++) {
      if (vels[i] < (calculateAverageVelocity(vels) - 0.1) || vels[i] > (calculateAverageVelocity(vels) + 0.1)) {
            // Adjust speed to reduce slip (for simplicity, reducing speed by 10%)
            updated_vels[i] *= 0.5;
      }
    }
  fl_motor_PWM(vel_to_pwm(updated_vels[0]));
  fr_motor_PWM(vel_to_pwm(updated_vels[1]));
  bl_motor_PWM(vel_to_pwm(updated_vels[2]));
  br_motor_PWM(vel_to_pwm(updated_vels[3])); 
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


//UPDATE FROM .4
float vel_to_pwm(float vel){
  float pwm = (255/1)*vel;
  return pwm;
}


float calculateAverageVelocity(float velocities[]) {
    float sum = 0;
    for (int i = 0; i < NUM_WHEELS; i++) {
        sum += velocities[i];
    }
    return sum / NUM_WHEELS;
}
