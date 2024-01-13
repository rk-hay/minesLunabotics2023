#include "vars.h"

float linear_x = 0.0;
float linear_y = 0.0;
float angular_z = 0.0;
char START_MARKER = 'S';

//TODO: NEXT IMPLEMENT A CONTROLLER, I RECCOMEND USING THE SEED LAB ONE HAYDEN, ITS FAIRLY EASY, BUT YOU WILL NEED TO GET THE ENCODERS WORKING
//SO
// 1. GET ENCODERS WORKING
//2. MAKE IT SO I CAN SEND ROBOT TO EXACT CORDS RELATIVE TO CURR POS
void setup() {

  motors_init();
  setupEncoders();
  Serial.begin(1000000);
  readEncoders();


}

//notes for controls system. the motors need a min PWM speed of 30.
void loop() {
  //loop only vars
  static float control_loop_timer = 0;
  static float print_timer = 0;
  static float prev_fl_enc = 0;
  static float comms_timer = 0;
  static float step_timer = 0;
  //first update vars from the system
  if (millis()-comms_timer > 15){
  //  Serial.println("comm");
  comms();
  comms_timer = millis();
  } 





  linear_x = adjust(linear_x, .09);
  linear_y = adjust(linear_y, .09);
  angular_z = adjust(angular_z, 2.0);
  readEncoders();

  //motors_PWM(linear_x*100, LOW);

  
  
  if (stepper_locked == false && millis()-step_timer > 30) {
    //Serial.println("step");
    opp_s_step_fast(angular_z);
    step_timer = millis();
  }

  //control_pos(.5); //alternative movement. Maybe use for fine grain postioning.

  //run the control loop on a 16 ms timer
  if (millis() - control_loop_timer > 16) {
  //  Serial.println("cl");
    control_vel(linear_x, linear_y);
    control_loop();
    control_loop_timer = millis();
  }


  //DEBUGGING STATEMENTS (print every 50 millis)
  if (millis() - print_timer > 50) {
//          Serial.print(fl_d_vel());
//          Serial.print("  ");
//          Serial.print(fr_d_vel());
//          Serial.print("  ");
//          Serial.print(bl_d_vel());
//          Serial.print("  ");
//          Serial.print(br_d_vel());
//          Serial.print("  ");

    //      Serial.print(fl_pwm);
    //      Serial.print("  ");
    //      Serial.print(fr_pwm);
    //      Serial.print("  ");
    //      Serial.print(bl_pwm);
    //      Serial.print("  ");
    //      Serial.print(br_pwm);

    //      Serial.print(fl_enc_pos);
    //      Serial.print("  ");
    //      Serial.print(fr_enc_pos);
    //      Serial.print("  ");
    //      Serial.print(bl_enc_pos);
    //      Serial.print("  ");
    //      Serial.print(br_enc_pos);

    //      Serial.print(fl_pos());
    //      Serial.println("  ");
    //      Serial.print(fr_pos());
    //      Serial.print("  ");
    //      Serial.print(bl_pos());
    //      Serial.print("  ");
    //      Serial.print(br_pos());
   
//            Serial.print(fl_s_pos);
//            Serial.println("  ");
//            Serial.print(fr_s_pos);
//            Serial.print("  ");
//            Serial.print(bl_s_pos);
//            Serial.print("  ");
//            Serial.print(br_s_pos);

//
          Serial.print(linear_x);
//          Serial.print("  ");
//          Serial.print(linear_y);
//          Serial.print("  ");
//          Serial.print(angular_z);
//          Serial.print("  ");
           Serial.println();
    print_timer = millis();
  }
}

void comms() {
  // Check if there is data available to read
  if (Serial.available() > 0) {
    // Read the first byte to identify the message type
    START_MARKER = Serial.read();

    // Check the identifier and read corresponding data
    if (START_MARKER == 'S') {
      // Read the next 12 bytes as three 32-bit floating-point values
      Serial.readBytes((char*)&linear_x, sizeof(float));
      Serial.readBytes((char*)&linear_y, sizeof(float));
      Serial.readBytes((char*)&angular_z, sizeof(float));
    
      // Process the received data as needed
      // For example, print the values to the Serial Monitor
      Serial.print("Linear X: ");
      Serial.println(linear_x);
//      Serial.print("Linear Y: ");
//      Serial.println(linear_y);
//      Serial.print("Angular Z: ");
//      Serial.println(angular_z);
    }
  }
}



float adjust(float val, int limit) {
  if ((val < limit && val > -limit)) {
    val = 0;
  }
  return val;
}
