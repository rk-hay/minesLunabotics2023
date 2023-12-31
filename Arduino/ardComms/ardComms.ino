float linear_x = 0.0;
float linear_y = 0.0;
float angular_z = 0.0;
bool turn = false;


//TODO: NEXT IMPLEMENT A CONTROLLER, I RECCOMEND USING THE SEED LAB ONE HAYDEN, ITS FAIRLY EASY, BUT YOU WILL NEED TO GET THE ENCODERS WORKING
//SO 
// 1. GET ENCODERS WORKING
//2. MAKE IT SO I CAN SEND ROBOT TO EXACT CORDS RELATIVE TO CURR POS 
void setup() {
    Serial.begin(57600);
    motors_init();
}

void loop() {
    bool f_dir = true;
    comms();
    if (linear_y > 10){
      digitalWrite(13, 0);
      }
    if (linear_y < 0){
      digitalWrite(13, 1);
      }
    linear_x = adjust(linear_x);
    angular_z = adjust(angular_z);
    
    turnDegrees(angular_z);
    
    if (linear_x > 0){
      f_dir = true;
      }
    else{ f_dir = false;}
    //motors_dir(true);
    //motors_PWM(0);
    motors_float(false);
}

void comms() {
    if (Serial.available() >= 3 * sizeof(float)) {  // Ensure enough bytes are available
        Serial.readBytes((char*)&linear_x, sizeof(linear_x));
        Serial.readBytes((char*)&linear_y, sizeof(linear_y));
        Serial.readBytes((char*)&angular_z, sizeof(angular_z));
        Serial.println(linear_x);
        // Your existing delay(10) - consider adjusting as needed
    }
 
}


float adjust(float val){
  if(val < 20 && val > -20){
      val = 0;
    }
    return val;
  }
