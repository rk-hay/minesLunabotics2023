#include "vars.h"
#include "float.h"
float linear_x = 0.0;
float linear_y = 0.0;
float angular_z = 0.0;
int ConveyorButton = 0;
int DeployButton = 0;
int DigLinButton = 0;
int DigBeltButton = 0;

char START_MARKER;


void setup() {

  motors_init();
  digSetup();
  setupEncoders();
  Serial.begin(19200);
  readEncoders();
  //Serial.println("setup");
}

//notes for controls system. the motors need a min PWM speed of 30.
void loop() {
   //Serial.println("loop");
  //loop only vars
  static float control_loop_timer = 0;
  static float print_timer = 0;
  static float prev_fl_enc = 0;
  static float comms_timer = 0;
  static float step_timer = 0;
  //first update vars from the system
  if (millis()-comms_timer > 6){
  //Serial.println("comm");
 // comms();

  comms_timer = millis();
  
  }

  readEncoders();
 

  //run the control loop on a timer
  if (millis() - control_loop_timer > 24) {
                                //  Serial.println("cl"); TESTING
                                //control_vel(-0.5, 0.0); OLD
                                //global_angle_select(40,-10,-10,40); OLD
    
    if (abs(linear_x) < .09) {linear_x = 0;}
    if (abs(linear_y) < .09) {linear_y = 0;}
    if (abs(angular_z) < .09) {angular_z = 0;}
    //four_ws_control(linear_x, linear_y, angular_z, 1);
    four_ws_control(0.0, 0.0, 0.0, 1);
    //control_vel_fl(.7); 
    if (stepper_locked == false) {
      step_pos(pos_angs);
      //control_vel_updated(lin_vels);
      //global_angle_select(0, 0, 45, 0);
      step_timer = millis();
    }
    //controller_control_loop();

    deployAppendage(DeployButton);
    digBelt(DigBeltButton);
    digDepth(DigLinButton);
    conveyor(ConveyorButton);
  //------------------------------------------------------------------------------//
  //global_angle_select(-45, 0,  0, 0); //stepperer tester   //fl is cur fr  //fr is br //bl is fl //br is bl
  //motors_PWM(0.0);
  //  fl_motor_PWM(50); 
  //  fr_motor_PWM(0);
  //  bl_motor_PWM(-50);
  //  br_motor_PWM(0);
  //control_vel(0.5, 0.0);
  //four_ws_co
  //TESTS
  //------------------------------------------------------------------------------//
    control_loop_timer = millis();
  }


  //DEBUGGING STATEMENTS (print every 50 millis)
  if (millis() - print_timer > 24) {
         Serial.print(fl_d_vel());
         Serial.print("  ");
//         Serial.print();
         Serial.print("  ");
  //       Serial.print(fl_v_D);
        //  Serial.print("  ");
        //  Serial.print(br_d_vel());
//          Serial.print("  ");
          Serial.print(.7);
            Serial.print("  ");
/**         Serial.print(lin_vels[1]);
         Serial.print("  ");
         Serial.print(lin_vels[2]);
         Serial.print("  ");
         Serial.print(lin_vels[3]);
         Serial.print("  ");
         Serial.print(pos_angs[0]);
         Serial.print("  ");
         Serial.print(pos_angs[1]);
         Serial.print("  ");
         Serial.print(pos_angs[2]);
         Serial.print("  ");
         Serial.print(pos_angs[3]
**/  
           Serial.print(fl_enc_pos);
          // Serial.print("  ");
          // Serial.print(fr_enc_pos);
          // Serial.print("  ");
          // Serial.print(bl_enc_pos);
          // Serial.print("  ");
          // Serial.print(br_enc_pos);

        //  Serial.print(fl_pos());
        //  Serial.print("  ");
        //  Serial.print(fr_pos());
        //  Serial.print("  ");
        //  Serial.print(bl_pos());
        //  Serial.print("  ");
        //  Serial.print(br_pos());
        //  Serial.print("  ");

          //  Serial.print(fl_s_pos);
          //  Serial.print("  ");
          //  Serial.print(fr_s_pos);
          //  Serial.print("  ");
          //  Serial.print(bl_s_pos);
          //  Serial.print("  ");
          //  Serial.print(br_s_pos);
//            Serial.print("  ");
/**
            Serial.print(linear_x);
            Serial.print("  ");
            Serial.print(linear_y);
            Serial.print("  ");
            Serial.print(angular_z);
            Serial.print("  ");
            **/
//
//            Serial.print(maX);
//            Serial.print("  ");
//            Serial.print(maY);
//            Serial.print("  ");
//            Serial.print(prevLinX[0]);
//            Serial.print("  ");
//            Serial.print(prevLinX[1]);
//            Serial.print("  ");
//                        Serial.print("  ");
//            Serial.print(prevLinX[2]);
//            Serial.print("  ");
//            Serial.print(prevLinX[3]);
//            Serial.print("  ");
            
            // Serial.print((3.14/180)*((fl_s_pos+fr_s_pos)/2));
            // Serial.print("  ");
            // Serial.print(3.14/180*(-(bl_s_pos+br_s_pos)/2));
            // Serial.print("  ");
            // Serial.print((fl_d_vel()*cos(fl_s_pos*PI/180)+fr_d_vel()*cos(fr_s_pos*PI/180)+bl_d_vel()*cos(-bl_s_pos*PI/180)+br_d_vel()*cos(-br_s_pos*PI/180))/4);
            // Serial.print("  ");
            // Serial.print(ang_vel());
            // Serial.print("  ");
               Serial.println();
    print_timer = millis();
  }
}

void comms() {
  // Check if there is data available to read
  if (Serial.available() > 0) {
        char cmd2 = Serial.read();
        switch (cmd2){
          case 'X':{Serial.readBytesUntil('\n', (char*)&linear_x, sizeof(float)+sizeof('\n')); break;}
          case 'Y':{Serial.readBytesUntil('\n', (char*)&linear_y, sizeof(float)+sizeof('\n')); break;}
          case 'Z': {Serial.readBytesUntil('\n', (char*)&angular_z, sizeof(float)+sizeof('\n')); break;}
          
          case 'A': {Serial.readBytesUntil('\n', (char*)&ConveyorButton, sizeof(int)+sizeof('\n')); break;}
          case 'B': {Serial.readBytesUntil('\n', (char*)&DeployButton, sizeof(int)+sizeof('\n')); break;}
          case 'C': {Serial.readBytesUntil('\n', (char*)&DigLinButton, sizeof(int)+sizeof('\n')); break;}
          case 'D': {Serial.readBytesUntil('\n', (char*)&DigBeltButton, sizeof(int)+sizeof('\n')); break;}
          
        }//end cmd2
        //TRY NO SWITCH????
        //Serial.readBytesUntil('X', (char*)&linear_x, sizeof(float)+sizeof('X'));
        //Serial.readBytesUntil('Y', (char*)&linear_y, sizeof(float)+sizeof('Y'));
        //Serial.readBytesUntil('Z', (char*)&angular_z, sizeof(float)+sizeof('Z'));
  }//end if
}//end comms



float movingAverage(float in[4], float prevAvg){
  float avg = 0.0;
//  if (in[0] >= FLT_MAX || in[1] >= FLT_MAX || in[2] >= FLT_MAX|| in[3] >= FLT_MAX || in[0] <= FLT_MIN || in[1] <= FLT_MIN || in[2] <= FLT_MIN || in[3] <= FLT_MIN){
//    avg = prevAvg;
//    }
//  else {
    avg = (in[0]+in[1]+in[2]+in[3])/4;
 // }
  return avg;
  }
