// Define pin numbers
#define appendageDeployLinears_PWM A8
#define appendageDeployLinears_IN1 A9
#define appendageDeployLinears_IN2 A10
#define appendageBelt_PWM 13
#define appendageBelt_IN1 A11
#define appendageBelt_IN2 A12

#define appendageDigLin_PWM 8
#define appendageDigLin_IN1 A13
#define appendageDigLin_IN2 A14

#define conveyor_PWM 13
#define conveyor_IN1 A6
#define conveyor_IN2 A7

//TODO 
// FUNCTION FOR EACH OF THE IMPORTANT MOTORS

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

  pinMode(conveyor_PWM, OUTPUT);
  pinMode(conveyor_IN1, OUTPUT);
  pinMode(conveyor_IN2, OUTPUT);

}

void activateDig(){
  //deploy
  deployAppendage(150);
  delay(5000);
  digBelt(50);
  digDepth(50);
  conveyor(50);
  delay(3000);
  digBelt(125);
  digDepth(75);
  conveyor(150);
}

void deployAppendage(int duty){
  //if limit switch != true
  bool dir = duty > 0;
  analogWrite(appendageDeployLinears_PWM, abs(duty));
  digitalWrite(appendageDeployLinears_IN1, !dir);
  digitalWrite(appendageDeployLinears_IN2, dir);
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

void conveyor(int duty){
  //if limit switch != true
  bool dir = duty > 0;
  analogWrite(conveyor_PWM, abs(duty));
  digitalWrite(conveyor_IN1, !dir);
  digitalWrite(conveyor_IN2, dir);
}
