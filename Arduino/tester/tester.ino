
//           motor Defines              //
//--------------------------------------//
//drivers pos pwm
#define fl_d_pwm  3

//drivers pos dir
#define fl_d_for 52

//drivers neg dir
#define fl_d_rev 50




void setup() {
  pinMode(fl_d_for, OUTPUT);
  pinMode(fl_d_rev, OUTPUT);
  pinMode(fl_d_pwm, OUTPUT);

}

void loop() {
  Motor1_Forward(100);
}

void Motor1_Forward(int Speed)
{
digitalWrite(fl_d_for,HIGH);
digitalWrite(fl_d_rev,LOW);
analogWrite(fl_d_pwm, Speed);
}
