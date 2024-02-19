#include "vars.h"

float linear_x = 0.0;
float linear_y = 0.0;
float angular_z = 0.0;
char START_MARKER;


void setup() {

  motors_init();
  setupEncoders();
  Serial.begin(115200);
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
  if (millis()-comms_timer > 15){
  //Serial.println("comm");
  //comms();
  updated_comms();
  comms_timer = millis();
  } 

  //linear_x = adjust(linear_x, .09);
  //linear_y = adjust(linear_y, .09);
  //angular_z = adjust(angular_z, 2.0);
  readEncoders();
 
  //motors_PWM(50, LOW);
  //br_motor_PWM(50, HIGH);
  
  //run the control loop on a 16 ms timer
  if (millis() - control_loop_timer > 16) {
    static bool autonomous = true;
    //  Serial.println("cl");
    //control_vel(linear_x, linear_y);
    //global_angle_select(40,-10,-10,40);
    control_vel_updated(lin_vels);
    if (stepper_locked == false && millis()-step_timer > 32) {
      step_pos(pos_angs);
      step_timer = millis();
    }
    if (toggleAuto){ autonomous = !autonomous;}
    if (autonomous) {auto_control_loop();}
    else {controller_control_loop();}
    control_loop_timer = millis();
  }


  //DEBUGGING STATEMENTS (print every 50 millis)
  if (millis() - print_timer > 50) {
        //  Serial.print(fl_d_vel());
        //  Serial.print("  ");
        //  Serial.print(fr_d_vel());
        //  Serial.print("  ");
        //  Serial.print(bl_d_vel());
        //  Serial.print("  ");
        //  Serial.print(br_d_vel());
        //  Serial.print("  ");

          Serial.print(lin_vels[0]);
          Serial.print("  ");
          Serial.print(lin_vels[1]);
          Serial.print("  ");
          Serial.print(lin_vels[2]);
          Serial.print("  ");
          Serial.print(lin_vels[3]);

    //      Serial.print(fl_enc_pos);
    //      Serial.print("  ");
    //      Serial.print(fr_enc_pos);
    //      Serial.print("  ");
    //      Serial.print(bl_enc_pos);
    //      Serial.print("  ");
    //      Serial.print(br_enc_pos);

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
          //  Serial.print("  ");

            //Serial.print(linear_x);
            //Serial.print("  ");
            //Serial.print(linear_y);
            //Serial.print("  ");
            //Serial.print(angular_z);
            //Serial.print("  ");
            
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
      char command = Serial.read();
    switch (command) {
      case 'L':{
        char cmd2 = Serial.read();
        switch (cmd2){
          case 'X':{Serial.readBytes((char*)&linear_x, sizeof(float)); break;}
          case 'Y':{Serial.readBytes((char*)&linear_y, sizeof(float)); break;}
         // case 'Z': {break;}
        }//end cmd2 L
        break;
      }//end case L
      case 'A':{
        char cmd2 = Serial.read();
        switch (cmd2){
         // case 'X':{Serial.readBytes((char*)&angular_x, sizeof(float)); break;}
         // case 'Y':{Serial.readBytes((char*)&angular_y, sizeof(float)); break;}
          case 'Z': {Serial.readBytes((char*)&angular_z, sizeof(float)); break;}
        }//end cmd2 A
        break;
      }
      case 'B':{
        char cmd2 = Serial.read();
        switch(cmd2){
          case 'Y': {Serial.readBytes((char*)&toggleAuto, sizeof(bool)); break;}        
          }
        break;
      }  
    }//end switch(command)
      //Serial.print("Linear X: ");
      //Serial.println(linear_x);
      //Serial.print("Linear Y: ");
      //Serial.println(linear_y);
      //Serial.print("Angular Z: ");
      //Serial.println(angular_z); 
  }//end if
}//end comms

void updated_comms(){
  if (Serial.available() > 1){
    char command = Serial.read();
    switch(command){
      case 'V':{
          Serial.readBytes((char*)&lin_vels, sizeof(lin_vels));
        break;
      }//end L
      case 'P':{
          Serial.readBytes((char*)&pos_angs, sizeof(pos_angs));
        break;
      }//end P
      case 'B':{
          Serial.readBytes((char*)&toggleAuto, sizeof(bool));      
        break;
      }  
    }//end switch(Comma..
  }//end if (Serial...
}//end updated_comms

float adjust(float val, int limit) {
  if ((val < limit && val > -limit)) {
    val = 0;
  }
  return val;
}
