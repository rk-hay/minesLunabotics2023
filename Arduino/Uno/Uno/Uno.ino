bool newData = false;
const byte numChars = 12;
char receivedChars[numChars];
char tempChars[numChars]; 

float BucketConveyor = 0;
float DigBeltButton = 0;

#define liveTrailer_PWM 5
#define bucketConveyor_PWM 6

void setup() {
  Serial.begin(19200);
  Serial.print(". ");
  
  pinMode(liveTrailer_PWM, OUTPUT);
  pinMode(bucketConveyor_PWM, OUTPUT);
}

void loop() {
  newComms();
  if (newData == true) {

    strcpy(tempChars, receivedChars);
    parseData();
    newData = false;
    }

  bucketConveyor(DigBeltButton);
  liveTrailer(BucketConveyor);
  
  Serial.print(DigBeltButton);
  Serial.print(" ");
  Serial.println(BucketConveyor);
}




void newComms(){
    static boolean recvInProgress = false;
    static byte ndx = 0;
    char startMarker = '<';
    char endMarker = '>';
    char rc;

    while (Serial.available() > 0 && newData == false) {
        rc = Serial.read();
        //Serial.println("recieving");
        if (recvInProgress == true) {
            if (rc != endMarker) {

                receivedChars[ndx] = rc;
                ndx++;
                if (ndx >= numChars) {
                    ndx = numChars - 1;
                }
            }
            else {
                receivedChars[ndx] = '\0'; // terminate the string
                recvInProgress = false;
                ndx = 0;
                newData = true;
            }
        }

        else if (rc == startMarker) {
            
            recvInProgress = true;
        }
    }
}
void parseData() {
    char * strtokIndx;

    strtokIndx = strtok(NULL, ",");
    DigBeltButton = atof(strtokIndx); 
    
    strtokIndx = strtok(NULL, ",");
    BucketConveyor = atof(strtokIndx);


}

void liveTrailer(int duty){
  digitalWrite(liveTrailer_PWM, abs(duty));
}

void bucketConveyor(int duty){
  analogWrite(bucketConveyor_PWM, abs(duty));
}
