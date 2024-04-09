// Define pin numbers
#define appendageDeployLinears_PWM A8 //the pwm pin 
#define appendageDeployLinears_IN1 A9
#define appendageDeployLinears_IN2 A10

#define slideOut_PWM A8 //was 13
#define slideOut_IN1 A6
#define slideOut_IN2 A7

#define appendageDigLin_PWM 8
#define appendageDigLin_IN1 A13
#define appendageDigLin_IN2 A5

#define liveTrailer_PWM A12 // new is 11


#define bucketConveyor_PWM A11 //set to 13?


#define appendageBelt_PWM 7
//TODO 
// FUNCTION FOR EACH OF THE IMPORTANT MOTORS




void digSetup() {
  // Initialize motor pins as outputs
  pinMode(appendageDeployLinears_PWM, OUTPUT);
  pinMode(appendageDeployLinears_IN1, OUTPUT);
  pinMode(appendageDeployLinears_IN2, OUTPUT);

  pinMode(bucketConveyor_PWM, OUTPUT);
  
  pinMode(appendageDigLin_PWM, OUTPUT);
  pinMode(appendageDigLin_IN1, OUTPUT);
  pinMode(appendageDigLin_IN2, OUTPUT);

  pinMode(slideOut_PWM, OUTPUT);
  pinMode(slideOut_IN1, OUTPUT);
  pinMode(slideOut_IN2, OUTPUT);

  pinMode(liveTrailer_PWM, OUTPUT);
  digitalWrite(liveTrailer_PWM, LOW);

  //analogWrite(appendageDeployLinears_PWM, 255);
  digitalWrite(A8, HIGH);
}


void deployAppendageLinActuators(int duty){
  //if limit switch != true
  bool dir = duty > 0;
  if (duty != 0){
  digitalWrite(appendageDeployLinears_IN1, !dir); 
  digitalWrite(appendageDeployLinears_IN2, dir); // high when +
  }
  else{
    digitalWrite(appendageDeployLinears_IN1, 0); 
    digitalWrite(appendageDeployLinears_IN2, 0); // high when +
    }
  
}

void liveTrailer(int duty){
  if (duty > 0){
    digitalWrite(liveTrailer_PWM, HIGH);
    }
  else{
    digitalWrite(liveTrailer_PWM, LOW);
    }
  
}

void bucketConveyor(int duty){
  if (duty > 0){
  digitalWrite(bucketConveyor_PWM, HIGH);
  }
  else{
  digitalWrite(bucketConveyor_PWM, LOW);
    }
}

void digDepth(int duty){
  //if limit switch != true
  bool dir = duty > 0;
  if (abs(duty) > 242){duty = 242;}
  analogWrite(appendageDigLin_PWM, abs(duty)); //abs(duty)
  digitalWrite(appendageDigLin_IN1, !dir);
  digitalWrite(appendageDigLin_IN2, dir);
}

void slideOutAcutators(int duty){  
  //if limit switch != true
  bool dir = duty > 0;
  if (duty != 0){
  digitalWrite(slideOut_IN1, !dir);
  digitalWrite(slideOut_IN2, dir);
  }
  else{
  digitalWrite(slideOut_IN1, 0);
  digitalWrite(slideOut_IN2, 0);
    }
}
