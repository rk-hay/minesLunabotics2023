// Define pin numbers
#define appendageDeployLinears_PWM A8 //the pwm pin 
#define appendageDeployLinears_IN1 A9
#define appendageDeployLinears_IN2 A10

#define appendageBelt_PWM 7
#define appendageBelt_IN1 A11
#define appendageBelt_IN2 A12

#define appendageDigLin_PWM 8
#define appendageDigLin_IN1 A13
#define appendageDigLin_IN2 A5

#define slideOut_PWM A8 //was 13
#define slideOut_IN1 A6
#define slideOut_IN2 A7

#define liveTrailer_PWM 13
#define liveTrailer_IN1 A2
#define liveTrailer_IN2 19

#define m1_PWM 7 //BUCKETS
#define m1_SLP A11  // enable if pwm != 0
#define m1_DIR A12  // 0 for forward 1 for backward

#define m2_PWM 13
#define m2_SLP A2
#define m2_DIR 19

#define IBT_PWM 7
#define IBT_IN1 A2
#define IBT_IN2 19

//TODO 
// FUNCTION FOR EACH OF THE IMPORTANT MOTORS

void trailer(int duty){
    //if limit switch != true
  bool dir = duty > 0;
  analogWrite(IBT_PWM, abs(duty));
  digitalWrite(IBT_IN1, dir);  //high when -
  digitalWrite(IBT_IN2, !dir); // high when +
  }


void bucketConveyor(int duty){
    //if limit switch != true
  bool dir = duty > 0;
  if (dir){
  digitalWrite(liveTrailer_IN1, !dir);
  digitalWrite(liveTrailer_IN2, dir);
  }
  else{
  digitalWrite(liveTrailer_IN1, HIGH);
  digitalWrite(liveTrailer_IN2, HIGH);
  }
  }


void digSetup() {
  // Initialize motor pins as outputs
  pinMode(appendageDeployLinears_PWM, OUTPUT);
  pinMode(appendageDeployLinears_IN1, OUTPUT);
  pinMode(appendageDeployLinears_IN2, OUTPUT);

  pinMode(appendageBelt_PWM, OUTPUT);
  pinMode(appendageBelt_IN1, OUTPUT);
  pinMode(appendageBelt_IN2, OUTPUT);
  
  pinMode(appendageDigLin_PWM, OUTPUT);
  pinMode(appendageDigLin_IN1, OUTPUT);
  pinMode(appendageDigLin_IN2, OUTPUT);

  pinMode(slideOut_PWM, OUTPUT);
  pinMode(slideOut_IN1, OUTPUT);
  pinMode(slideOut_IN2, OUTPUT);

  pinMode(slideOut_IN1, OUTPUT);
  pinMode(slideOut_IN2, OUTPUT);

  pinMode(m1_PWM, OUTPUT);
  pinMode(m1_SLP, OUTPUT);
  pinMode(m1_DIR, OUTPUT);

  pinMode(m2_PWM, OUTPUT);
  pinMode(m2_SLP, OUTPUT);
  pinMode(m2_DIR, OUTPUT);
  
  pinMode(IBT_PWM, OUTPUT);
  pinMode(IBT_IN1, OUTPUT);
  pinMode(IBT_IN2, OUTPUT);
}


void deployAppendageLinActuators(int duty){
  //if limit switch != true
  bool dir = duty > 0;
  analogWrite(appendageDeployLinears_PWM, abs(duty));
  digitalWrite(appendageDeployLinears_IN1, !dir);  //high when -
  digitalWrite(appendageDeployLinears_IN2, dir); // high when +
}

void liveTrailer(int duty){
  //if limit switch != true
  bool dir = duty > 0;
  if (dir){
  digitalWrite(liveTrailer_IN1, !dir);
  digitalWrite(liveTrailer_IN2, dir);
  }
  else{
  digitalWrite(liveTrailer_IN1, HIGH);
  digitalWrite(liveTrailer_IN2, HIGH);
  }
}


void digBelt(int duty){
  //if limit switch != true
  bool dir = duty > 0;
  analogWrite(appendageBelt_PWM, abs(duty));
  digitalWrite(appendageBelt_IN1, !dir);
  digitalWrite(appendageBelt_IN2, dir);
}

void digDepth(int duty){
  //if limit switch != true
  bool dir = duty > 0;
  analogWrite(appendageDigLin_PWM, abs(duty));
  digitalWrite(appendageDigLin_IN1, !dir);
  digitalWrite(appendageDigLin_IN2, dir);
}

void slideOutAcutators(int duty){  
  //if limit switch != true
  bool dir = duty > 0;
  analogWrite(slideOut_PWM, abs(duty));
  digitalWrite(slideOut_IN1, !dir);
  digitalWrite(slideOut_IN2, dir);
}
