#include "vars.h"
#include "float.h"

#define COMMS_LOOP
#define CONTROL_LOOP
#define PRINT

void setup() {
  motors_init();
  digSetup();
  delay(4000);
  setupEncoders();
  
  Serial.begin(115200);
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
  static float prevStepPos[4] = {0 ,0 ,0, 0};
  //readEncoders();
#ifdef COMMS_LOOP
  newComms();
  if (newData == true) {
    strcpy(tempChars, receivedChars);
    parseData();
    newData = false;
    }
#endif
#ifdef CONTROL_LOOP
  if (millis() - control_loop_timer > 24) {
    diggingTime(digToggle);
    if(digToggle == 0){ //for manual driving use digModeButton
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

    if (stepper_locked == false) {
      step_pos(pos_angs);
      
      //if theortical pos of stepper ! = prev theortical postion{
      if(
             prevStepPos[0] != pos_angs[0] ||
      prevStepPos[1] != pos_angs[1] ||
      prevStepPos[2] != pos_angs[2] ||
      prevStepPos[3] != pos_angs[3]){
      Serial.println("BLERP BLERP BLERP BLERP");
      fixSteppers();
      prevStepPos[0] = pos_angs[0];
      prevStepPos[1] = pos_angs[1];
      prevStepPos[2] = pos_angs[2];
      prevStepPos[3] = pos_angs[3];
      }
  // prevPostion = current postion
     //}
      //global_angle_select(90, 90, 90, 90);
      control_vel_updated(lin_vels);
      
      
      step_timer = millis();
    }
    controller_control_loop();
    control_loop_timer = millis();
  }
  deployAppendageLinActuators(DeployButton); //Screenshot && Select
  slideOutAcutators(ConveyorButton); // XY
  digDepth(DigLinButton);
  bucketConveyor(DigBeltButton); //
  liveTrailer(BucketConveyor);
#endif 


  
#ifdef PRINT
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
          Serial.print("  ");

              Serial.print("Fl_act: ");
            Serial.print(fl_s_pos);
            Serial.print(" Fr: ");
            Serial.print(fr_s_pos);
            Serial.print(" BL: ");
            Serial.print(bl_s_pos);
            Serial.print(" Br: ");
            Serial.print(br_s_pos);
            Serial.println("  ");

//            Serial.print(linear_x);
//            Serial.print("  ");
//            Serial.print(linear_y);
//            Serial.print("  ");
//            Serial.print(angular_z);
//            Serial.print("  ");
//            /

/**
                Serial.print(DeployButton);
                Serial.print("  ");
                Serial.print(ConveyorButton);
                Serial.print("  ");
                Serial.print(DigLinButton);
                Serial.print("  ");
                Serial.print(DigBeltButton);
                Serial.print(" ");
                Serial.println(BucketConveyor);
**/


               Serial.println();
    print_timer = millis();
  }
#endif
}


void diggingTime(bool Toggle){
  static float prevState = 0;
  if (Toggle == true && Toggle !=prevState){
  digModeButton = !digModeButton;
  }
  prevState = Toggle;
}




 void fixSteppers(){
 int fl = 0, fr = 0, bl = 0, br = 0;
 int fl_cor=0, fr_cor=0, bl_cor=0, br_cor=0;
  
  for (int i = 0; i < 50; i++) {
    fl += analogRead(fl_pot_pin) & 0b1111111111111100;
    fr += analogRead(fr_pot_pin) & 0b1111111111111100;
    bl += analogRead(bl_pot_pin) & 0b1111111111111100;
    br += analogRead(br_pot_pin) & 0b1111111111111100;
    //delay(10); // optional delay to stabilize readings
  }
  
  fl /= 50;
  fr /= 50;
  bl /= 50;
  br /= 50;

  int fl_map = map(fl, 246, 275, -90, 90);
  int fr_map = map(fr, 124, 236, -90, 90);
  int bl_map = map(bl, 216, 328, -90, 90);
  int br_map = map(br, 139, 251, -90, 90);
  
  Serial.print("Fl: ");
  Serial.print(fl);
  Serial.print("  FL - map ");
  Serial.print(fl_map);
  Serial.print(" Fr: ");
  Serial.print(fr_map);  
  Serial.print(" Bl: ");
  Serial.print(bl_map);
  Serial.print(" Br: ");
  Serial.println(br_map);
   if (abs(fl_map-fl_s_pos) > 5){
     //fl_cor = fl_s_pos-fl_map;
   }
   if (abs(fr_map-fr_s_pos) > 5){
     fr_cor = fr_s_pos-fr_map;
   }
   if (abs(bl_map-bl_s_pos) > 5){
     bl_cor = bl_s_pos-bl_map;
   }
   if (abs(br_map-br_s_pos) > 5){
     br_cor = br_s_pos-br_map;
   }
   stepper_adjust(fl_cor, fr_cor, bl_cor, br_cor);
 }
