#include "vars.h"
#include "float.h"
float linear_x = 0.0;
float linear_y = 0.0;
float angular_z = 0.0;
char START_MARKER;


void setup() {

  motors_init();
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
  comms();
  //updated_comms();
  comms_timer = millis();
  
    if (maInc < 3){
      prevLinX[maInc] = linear_x;
      prevLinY[maInc] = linear_y;
      prevAngZ[maInc] = angular_z;
      maInc += 1;
      }
    else{
      prevLinX[maInc] = linear_x;
      prevLinY[maInc] = linear_y;
      prevAngZ[maInc] = angular_z;
      maInc = 0;
      }
    maX = movingAverage(prevLinX, maX);
    maY = movingAverage(prevLinY, maY);
    maZ = movingAverage(prevAngZ, maX);
    
  } 

  //linear_x = adjust(linear_x, .09);
  //linear_y = adjust(linear_y, .09);
  //angular_z = adjust(angular_z, 2.0);
  readEncoders();
 
  //motors_PWM(50);
  //br_motor_PWM(50);  
  //run the control loop on a 16 ms timer
  if (millis() - control_loop_timer > 24) {
    //  Serial.println("cl"); TESTING
    //control_vel(-0.5, 0.0); OLD
    //global_angle_select(40,-10,-10,40); OLD
    
    four_ws_control(maX, maY, maZ, 1); 
    if (stepper_locked == false) {
      step_pos(pos_angs);
      control_vel_updated(lin_vels);
      //global_angle_select(0, 0, 45, 0);
      step_timer = millis();
    }
    controller_control_loop();
  //------------------------------------------------------------------------------//
  //global_angle_select(45, 45,  45, 45); //stepperer tester
  //motors_PWM(50.0);
  //fl_motor_PWM(50); //does not turn with neg value
  //fr_motor_PWM(-50);
  //bl_motor_PWM(-50);
  //br_motor_PWM(255);
  //control_vel(0.5, 0.0);
  //four_ws_co
  //TESTS
  //------------------------------------------------------------------------------//
    control_loop_timer = millis();
  }


  //DEBUGGING STATEMENTS (print every 50 millis)
  if (millis() - print_timer > 24) {
        //  Serial.print(fl_d_vel());
        //  Serial.print("  ");
        //  Serial.print(fr_d_vel());
        //  Serial.print("  ");
        //  Serial.print(bl_d_vel());
        //  Serial.print("  ");
        //  Serial.print(br_d_vel());
        //  Serial.print("  ");
//            Serial.print(lin_vels[0]);
//            Serial.print("  ");
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
//          Serial.print(fl_enc_pos);
//          Serial.print("  ");
//          Serial.print(fr_enc_pos);
//          Serial.print("  ");
//          Serial.print(bl_enc_pos);
//          Serial.print("  ");
//          Serial.print(br_enc_pos);

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
            Serial.print("  ");
//
            Serial.print(linear_x);
            Serial.print("  ");
            Serial.print(linear_y);
            Serial.print("  ");
            Serial.print(angular_z);
            Serial.print("  ");
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
        }//end cmd2
        //TRY NO SWITCH????
        //Serial.readBytesUntil('X', (char*)&linear_x, sizeof(float)+sizeof('X'));
        //Serial.readBytesUntil('Y', (char*)&linear_y, sizeof(float)+sizeof('Y'));
        //Serial.readBytesUntil('Z', (char*)&angular_z, sizeof(float)+sizeof('Z'));
  }//end if
}//end comms

void comms2() {
  // Check if there is data available to read
  if (Serial.available() > 0) {
        char cmd2 = Serial.read();
        switch (cmd2){
         // case 'X':{Serial.readStringUntil('\n', (char*)&linear_x, sizeof(float)+sizeof('\n')); linear_x =linear_x/1000;  break;}
         // case 'Y':{Serial.readStringUntil('\n', (char*)&linear_y, sizeof(float)+sizeof('\n')); linear_y =linear_y/1000; break;}
         // case 'Z': {Serial.readStringUntil('\n', (char*)&angular_z, sizeof(float)+sizeof('\n')); angular_z =angular_z/1000; break;}
        }//end cmd2
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
