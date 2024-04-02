#include "vars.h"



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
    
    strtokIndx = strtok(tempChars,",");
    linear_x = atof(strtokIndx); 
    strtokIndx = strtok(NULL, ",");
    linear_y = atof(strtokIndx);     
    strtokIndx = strtok(NULL, ",");
    angular_z = atof(strtokIndx); 
    strtokIndx = strtok(NULL, ",");
    ConveyorButton = atof(strtokIndx);   
    strtokIndx = strtok(NULL, ",");
    DeployButton = atof(strtokIndx); 
    strtokIndx = strtok(NULL, ",");
    DigLinButton = atof(strtokIndx);     
    strtokIndx = strtok(NULL, ",");
    DigBeltButton = atof(strtokIndx); 
    strtokIndx = strtok(NULL, ",");
    BucketConveyor = atof(strtokIndx); 

}




float movingAverage(float in[4], float prevAvg){
  float avg = 0.0;
//  if (in[0] >= FLT_MAX || in[1] >= FLT_MAX || in[2] >= FLT_MAX|| in[3] >= FLT_MAX || in[0] <= FLT_MIN || in[1] <= FLT_MIN || in[2] <= FLT_MIN || in[3] <= FLT_MIN){
//    avg = prevAvg;
//    }
//  else {
    avg = (in[0]+in[1]+in[2]+in[3])/4;
 // }
  return avg;
  }
