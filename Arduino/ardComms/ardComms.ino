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
    Serial.begin(115200);
    readEncoders();


}

//notes for controls system. the motors need a min PWM speed of 30.
void loop() {
    static float control_loop_timer = 0;
    static float print_timer = 0;
    bool dir = true;
    static float prev_fl_enc = 0;
 
    
    //comms();
    //linear_x = adjust(linear_x);
    //angular_z = adjust(angular_z);
    //dir = linear_x > 0;
    //motors_PWM(linear_x, dir);
    readEncoders();
    
    //motors_PWM(150, LOW);

    //control_vel(.35, 0);
    
    control_pos(.5);
    
    if(millis()- control_loop_timer > 16){
      //Serial.println(millis()- control_loop_timer);
      control_loop();
      control_loop_timer = millis();
    }

    if(millis()-print_timer > 50){
      Serial.print(fl_d_vel());
      Serial.print("  ");
//      Serial.print(fr_d_vel());
//      Serial.print("  ");
//      Serial.print(bl_d_vel());
//      Serial.print("  ");
//      Serial.println(br_d_vel());

      Serial.print(fl_pwm);
      Serial.print("  ");
//      Serial.print(fr_pwm);
//      Serial.print("  ");
//      Serial.print(bl_pwm);
//      Serial.print("  ");
//      Serial.println(br_pwm);

      Serial.print(fl_enc_pos);
      Serial.print("  ");
//      Serial.print(fr_enc_pos);
//      Serial.print("  ");
//      Serial.print(bl_enc_pos);
//      Serial.print("  ");
//      Serial.println(br_enc_pos);

      Serial.print(fl_pos());
      Serial.println("  ");
//      Serial.print(fr_pos());
//      Serial.print("  ");
//      Serial.print(bl_pos());
//      Serial.print("  ");
//      Serial.println(br_pos());
      
      //print_timer = millis();
    }
}

void comms() {
    if (Serial.available() >= 3 * sizeof(float)) {  // Ensure enough bytes are available
        
    char startMarker = Serial.read();
    if (startMarker == START_MARKER) {
        Serial.readBytes((char*)&linear_x, sizeof(linear_x));
        Serial.readBytes((char*)&linear_y, sizeof(linear_y));
        Serial.readBytes((char*)&angular_z, sizeof(angular_z));
        Serial.println(linear_x);
        //Serial.print(" ");
        //Serial.print(angular_z);
        //Serial.println(" ");
        // Process the received data
    }
    }
}


float adjust(float val){
  if(val < 35 && val > -35){
      val = 0;
    }
    return val;
  }
