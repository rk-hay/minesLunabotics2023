#include "vars.h"

float linear_x = 0.0;
float linear_y = 0.0;
float angular_z = 0.0;
bool turn = false;
long fl_newPos = -999;
long fr_newPos = -999;
long bl_newPos = -999;
long br_newPos = -999;
char START_MARKER = 'S';

//TODO: NEXT IMPLEMENT A CONTROLLER, I RECCOMEND USING THE SEED LAB ONE HAYDEN, ITS FAIRLY EASY, BUT YOU WILL NEED TO GET THE ENCODERS WORKING
//SO 
// 1. GET ENCODERS WORKING
//2. MAKE IT SO I CAN SEND ROBOT TO EXACT CORDS RELATIVE TO CURR POS 
void setup() {
    motors_init();
    
    Serial.begin(115200);
    //readEncoders();
}

void loop() {
    bool f_dir = true;
    comms();
    linear_x = adjust(linear_x);
    angular_z = adjust(angular_z);
    oneStep();
    //turnDegrees(angular_z);
    //f_dir = linear_x > 0;
    //motors_dir(f_dir);
    //motors_PWM(linear_x);
    //motors_float(false); //change to false
    
    //readEncoders();
    


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
  if(val < 20 && val > -20){
      val = 0;
    }
    return val;
  }
