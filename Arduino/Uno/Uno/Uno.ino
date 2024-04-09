

void setup() {
  pinMode(11, OUTPUT);
  pinMode(A0, OUTPUT);
  pinMode(A1, OUTPUT);
  
}

void loop() {
  analogWrite(11, 242);
  digitalWrite(A0, HIGH);
  digitalWrite(A1, LOW);
}
