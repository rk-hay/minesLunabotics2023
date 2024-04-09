#include "vars.h"
#include "float.h"



void setup() { 
  motors_init();
  digSetup();
  delay(4000);
  setupEncoders();
  Serial.begin(19200);
  Serial.flush();
  //Serial.print(". ");
  //readEncoders();

}




//notes for controls system. the motors need a min PWM speed of 30.
void loop() {
  static int driveMode = 1;
  static float control_loop_timer = 0;
  static float print_timer = 0;
  static float prev_fl_enc = 0;
  static float comms_timer = 0;
  static float step_timer = 0;
  readEncoders();
  newComms();
  if (newData == true) {
    strcpy(tempChars, receivedChars);
    parseData();
    newData = false;
    }

  if (millis() - control_loop_timer > 24) {
    diggingTime(digToggle);
    if(digModeButton == 0){
      if (linear_x == 0 && linear_y == 0 && angular_z != 0){
        driveMode = 3; //pivot mode
        }
      else if(linear_x != 0 && linear_y != 0 && angular_z == 0){
        driveMode = 2; //in-phase mode
        }
      else if(linear_x != 0 && linear_y == 0 && angular_z != 0){
        driveMode = 1; //opposite phase mode
        }
      else{
        driveMode = 1;
        }
    }
    else{ driveMode = 4; }// digging time bby}

    
    four_ws_control(linear_x, linear_y, angular_z, driveMode);
    //four_ws_control(0.0, 0.0, 0.0, 1);
    if (stepper_locked == false) {
      step_pos(pos_angs);
      control_vel_updated(lin_vels);
      //global_angle_select(0, 0, 0, -45);
      step_timer = millis();
    }
    controller_control_loop();
    control_loop_timer = millis();
  }
  deployAppendageLinActuators(DeployButton); //DeployButton fold out
  digDepth(DigLinButton); // linear actuator
  slideOutAcutators(ConveyorButton); // 
  bucketConveyor(DigBeltButton);
  liveTrailer(BucketConveyor);
   
  //DEBUGGING STATEMENTS (print every 50 millis)
  if (millis() - print_timer > 100) {
        // Serial.print(fl_d_vel());
        //  Serial.print("  ");
        //  Serial.print(lin_vels[2]);
        //  Serial.print("  ");
        //  Serial.print(lin_vels[3]);
        //  Serial.print("  ");
        //  Serial.print(pos_angs[0]);
        //  Serial.print("  ");
        //  Serial.print(pos_angs[1]);
        //  Serial.print("  ");
        //  Serial.print(pos_angs[2]);
        //  Serial.print("  ");
        //  Serial.print(pos_angs[3]
 
           Serial.print(fl_enc_pos);//11
           Serial.print("  ");
           Serial.print(fr_enc_pos);
           Serial.print("  ");
           Serial.print(bl_enc_pos);
           Serial.print("  ");
           Serial.print(br_enc_pos); //7
//           Serial.print("  ");


//            Serial.print(fl_s_pos);
//            Serial.print("  ");
//            Serial.print(fr_s_pos);
//            Serial.print("  ");
//            Serial.print(bl_s_pos);
//            Serial.print("  ");
//            Serial.print(br_s_pos);
//            Serial.print("  ");
//
//            Serial.print(linear_x);
//            Serial.print("  ");
//            Serial.print(linear_y);
//            Serial.print("  ");
//            Serial.print(angular_z);
//            Serial.print("  ");
//            /


              // Serial.print(DeployButton);
              // Serial.print("  ");
              // Serial.print(DigLinButton);
              // Serial.print("  ");
              // Serial.print(ConveyorButton);
              // Serial.print("  ");
              // Serial.print(DigBeltButton);
              // Serial.print(" ");
              // Serial.println(BucketConveyor);

               Serial.println();
    print_timer = millis();
  }
}


void diggingTime(bool Toggle){
  static float prevState = 0;
  if (Toggle == true && Toggle !=prevState){
  digModeButton = !digModeButton;
  }
  prevState = Toggle;
}