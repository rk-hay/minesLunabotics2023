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
    //four_ws_control(0.2, 0.0, -0.5, 1);
    if (stepper_locked == false) {
      step_pos(pos_angs);
      control_vel_updated(lin_vels);
      //global_angle_select(0, 0, 0, -45);
      step_timer = millis();
    }
    controller_control_loop();
    control_loop_timer = millis();
  }
  if(digToggle == 0){
  deployAppendageLinActuators(DeployButton); //DeployButton fold out 
  slideOutAcutators(ConveyorButton); // 
  }
  else{
    //if(digitalRead(!pinConnectedToStop)){
    //slideOutAcutators(254);
  //}
    //else if(digitalRead(!pinConnectedToFoldOut)){
      //deployAppendageLinActuators(254);
    //}
    //
  }
  digDepth(7);
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


//            Serial.print(fl_s_pos);
//            Serial.print("  ");
//            Serial.print(fr_s_pos);
//            Serial.print("  ");
//            Serial.print(bl_s_pos);
//            Serial.print("  ");
//            Serial.print(br_s_pos);
//            Serial.print("  ");
//
            Serial.print(linear_x);
            Serial.print("  ");
            Serial.print(linear_y);
            Serial.print("  ");
            Serial.print(angular_z);
            Serial.print("  ");
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



// #define fl_pot_pin
// #define fr_pot_pin  
// #define bl_pot_pin
// #define br_pot_pin
// void fixSteppers(){
//   int fl_cor = 0;
//   int fr_cor = 0;
//   int bl_cor = 0;
//   int br_cor = 0;

//   int fl_POT = map(analogRead(fl_pot_pin), leftMostPotVal, rightMostPotVal, -90, 90) //TODO double check that -90 is left and 90 is right
//   int fr_POT = map(analogRead(fr_pot_pin), leftMostPotVal, rightMostPotVal, -90, 90)
//   int bl_POT = map(analogRead(bl_pot_pin), leftMostPotVal, rightMostPotVal, -90, 90)
//   int fr_POT = map(analogRead(fr_pot_pin), leftMostPotVal, rightMostPotVal, -90, 90)
  
//   if (abs(fl_POT-fl_s_pos) > 5){
//     fl_cor = fl_s_pos-fl_POT;
//   }
//   if (abs(fr_POT-fr_s_pos) > 5){
//     fr_cor = fr_s_pos-fr_POT;
//   }
//   if (abs(bl_POT-bl_s_pos) > 5){
//     bl_cor = bl_s_pos-bl_POT;
//   }
//   if (abs(br_POT-br_s_pos) > 5){
//     br_cor = br_s_pos-br_POT;
//   }
//   stepper_adjust(fl_cor, fr_cor, bl_cor, br_cor);
// }