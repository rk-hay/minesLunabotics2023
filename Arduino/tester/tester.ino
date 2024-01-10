
#ifndef VARS_H
#define VARS_H


//stepper pos enable -PULL HIGH
#define fl_s_nenable 52

//stepper pos enable - PULL HIGH
#define fl_s_penable 50

//steppers neg dir - DIR pick
#define fl_s_ndir 48

//steppers pos dir - PULL HIGH
#define fl_s_pdir 46

//steppers neg pwm - PULSE TO MOVE
#define fl_s_npwm 44

//steppers pos pwm - PULL HIGH
#define fl_s_ppwm 42





float stepper = 0;

void setup() {
  Serial.begin(115200);
  pinMode(fl_s_nenable, OUTPUT);
  pinMode(fl_s_penable, OUTPUT);
  pinMode(fl_s_ndir, OUTPUT);
  pinMode(fl_s_pdir, OUTPUT);
  pinMode(fl_s_npwm, OUTPUT);
  pinMode(fl_s_ppwm, OUTPUT);


  

}
//High for CC for Low for CW
void moveSteps(int steps, int dir) {
  digitalWrite(fl_s_ndir, !dir);
  digitalWrite(fl_s_pdir, dir);
  digitalWrite(fl_s_npwm, LOW);
  
  
  for (int i = 0; i < abs(steps); i++) {
    digitalWrite(fl_s_ppwm, HIGH);
    delayMicroseconds(50000);
    digitalWrite(fl_s_ppwm, LOW);
    
    // You can add additional delay if needed
    //delayMicroseconds(50000); // Adjust the delay based on your requirements
  }
}

void loop() {

//  // Move 200 steps clockwise (assuming 200 steps per revolution)
  Serial.println("cc");
  moveSteps(2500, HIGH);
  
  // Pause for a moment
  delay(1000);
  
  // Move 100 steps counterclockwise
  Serial.println("cw ");
  moveSteps(2500, LOW);
  
  
   ;
}
#endif
