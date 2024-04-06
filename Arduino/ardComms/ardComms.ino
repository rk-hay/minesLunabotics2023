#include "vars.h"
#include "float.h"



void setup() {
  
  motors_init();
  digSetup();
  setupEncoders();
  Serial.begin(19200);
  Serial.flush();
  Serial.print(". ");
  readEncoders();

}




//notes for controls system. the motors need a min PWM speed of 30.
void loop() {
  static int driveMode = 1;
  static float control_loop_timer = 0;
  static float print_timer = 0;
  static float prev_fl_enc = 0;
  static float comms_timer = 0;
  static float step_timer = 0;
  newComms();
  if (newData == true) {

    strcpy(tempChars, receivedChars);
    parseData();
    newData = false;
    }
  readEncoders();
  //run the control loop on a timer
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
    //four_ws_control(.5, 0.0, .25, 3);
    //control_vel_fl(.7); 
    if (stepper_locked == false) {
      control_vel_updated(lin_vels);
      step_pos(pos_angs);
      //global_angle_select(0, 0, 0, -45);
      step_timer = millis();
    }
    controller_control_loop();


 
    control_loop_timer = millis();
  }
   deployAppendageLinActuators(DeployButton); //
   digDepth(DigLinButton); //
   slideOutAcutators(ConveyorButton); // 
   bucketConveyor(DigBeltButton);
   liveTrailer(BucketConveyor);
   
  //DEBUGGING STATEMENTS (print every 50 millis)
  if (millis() - print_timer > 500) {
//         Serial.print(fl_d_vel());
//         Serial.print("  ");
//         Serial.print();
   //      Serial.print("  ");
  //       Serial.print(fl_v_D);
        //  Serial.print("  ");
        //  Serial.print(br_d_vel());
//          Serial.print("  ");
//          Serial.print(.7);
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
           Serial.print(fl_enc_pos);
           Serial.print("  ");
           Serial.print(fr_enc_pos);
           Serial.print("  ");
           Serial.print(bl_enc_pos);
           Serial.print("  ");
           Serial.print(br_enc_pos);
           Serial.print("  ");


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


              Serial.print(DeployButton);
              Serial.print("  ");
              Serial.print(DigLinButton);
              Serial.print("  ");
              Serial.print(ConveyorButton);
              Serial.print("  ");
              Serial.print(DigBeltButton);
              Serial.print(" ");
              Serial.println(BucketConveyor);
            
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

//
//void comms() {
//  // Check if there is data available to read
//  if (Serial.available() > 0) {
//        char cmd2 = Serial.read();
//        switch (cmd2){
//          case 'X':{Serial.readBytesUntil('\n', (char*)&linear_x, sizeof(float)+sizeof('\n')); break;}
//          case 'Y':{Serial.readBytesUntil('\n', (char*)&linear_y, sizeof(float)+sizeof('\n')); break;}
//          case 'Z': {Serial.readBytesUntil('\n', (char*)&angular_z, sizeof(float)+sizeof('\n')); break;}
//          
//          case 'A': {Serial.readBytesUntil('\n', (char*)&ConveyorButton, sizeof(float)+sizeof('\n')); break;}
//          case 'B': {Serial.readBytesUntil('\n', (char*)&DeployButton, sizeof(float)+sizeof('\n')); break;}
//          case 'C': {Serial.readBytesUntil('\n', (char*)&DigLinButton, sizeof(float)+sizeof('\n')); break;}
//          case 'D': {Serial.readBytesUntil('\n', (char*)&DigBeltButton, sizeof(float)+sizeof('\n')); break;}
//          case 'E': {Serial.readBytesUntil('\n', (char*)&BucketConveyor, sizeof(float)+sizeof('\n')); break;}
//        }//end cmd2
//  }//end if
//}//end comms
//
//  
