
#include <SPI.h>
#include "DW1000.h"

#define SPI_SCK 18
#define SPI_MISO 19
#define SPI_MOSI 23
#define DW_CS 4

// connection pins
const uint8_t PIN_RST = 27; // reset pin
const uint8_t PIN_IRQ = 34; // irq pin
const uint8_t PIN_SS = 4;   // spi select pin



#define POLL 0
#define POLL_ACK 1
#define RANGE 2
#define RANGE_REPORT 3
#define RANGE_FAILED 255
// message flow state
volatile byte expectedMsgId;
// message sent/received state
volatile boolean sentAck = false;
volatile boolean receivedAck = false;
// timestamps to remember
DW1000Time timePollSent;
DW1000Time timePollReceived;
DW1000Time timePollAckSent;
DW1000Time timePollAckReceived;
DW1000Time timeRangeSent;
DW1000Time timeRangeReceived;
// last computed range/time
DW1000Time timeComputedRange;
// data buffer
#define LEN_DATA 18 // last two bytes are [to who, from who]
#define N_ANCHORS 4
byte data[LEN_DATA];
#define Adelay 16428 //best so far 16284 expecting 1.18
// watchdog and reset period
uint32_t lastActivity;
uint32_t resetPeriod = 250;

uint8_t maxNumTimeouts = 10; // wait this number of timeouts if switching frequencies before switching back
uint8_t numTimeouts = 0;

// reply times (same on both sides for symm. ranging)
uint16_t replyDelayTimeUS = 7000; //7000
// ranging counter (per second)

uint16_t successRangingCount = 0;
uint32_t rangingCountPeriod = 0;
float samplingRate = 0;

uint8_t msg_sent = 0;
uint8_t my_freq = 5;
uint8_t switchedFreq = 0;

byte MY_NUM = 0;
byte OTHER_NUM = 0; // will need to update
byte FREQUENCY = 5;

typedef enum State_t {
  TAG = 0,
  ANCHOR = 1
} State;

State my_state;
#define arenaLength 8.14
#define arenaHeight 4.57
float anchor_matrix[N_ANCHORS][3] = { //list of anchor coordinates, relative to chosen origin.
  {0.0, 0.0, 0.0},  //Anchor labeled #1
  {0.0, arenaHeight, 0.0},//Anchor labeled #2
  {arenaLength, 0, 0.0}, //Anchor labeled #3
  {arenaLength, arenaHeight, 0.0} //Anchor labeled #4
};  //Z values are ignored in this code, except to compute RMS distance error
float last_anchor_distance[N_ANCHORS] = {0.0}; //most recent distance reports
float current_tag_position[2] = {0.0, 0.0}; //global current position (meters with respect to anchor origin)
float current_distance_rmse = 0.0;  //rms error in distance calc => crude measure of position error (meters).  Needs to be better characterized


void setup()
{
  Serial.begin(115200);
  Serial.flush();
  delay(0.5);

  SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);
 
  //Serial.setTimeout(250);
  initDW1000();
  becomeAnchor();
  //becomeTag();
}
void initDW1000() {
  // initialize the driver
  DW1000.begin(PIN_IRQ, PIN_RST);
  DW1000.select(PIN_SS);
  // general configuration
  if ( setDW(FREQUENCY) ) freezeError(0);
  // attach callback for (successfully) sent and received messages
  DW1000.attachSentHandler(handleSent);
  DW1000.attachReceivedHandler(handleReceived);
}

int8_t setDW(byte freq) {
  if ( !freq || freq > 7 || freq == 6 ) // check for invalid frequency
    return -1;
  my_freq = freq; // only time we modify is when we actually change
  //  Serial.print("\rSetting freq = "); Serial.println(freq);
  DW1000.newConfiguration();
  DW1000.setDefaults(); // we'll change this to dyanimically take a 
  DW1000.setDeviceAddress(MY_NUM);
  DW1000.setNetworkId(10);
  DW1000.enableMode(DW1000.MODE_SHORTDATA_FAST_ACCURACY);
  DW1000.setAntennaDelay(Adelay);
  DW1000.commitConfiguration();
  return 0;
}

void freezeError(uint8_t num) { // let's just head back to default
  Serial.print('e');
  Serial.print(num);
  OTHER_NUM = 0;
  FREQUENCY = 5;
  becomeAnchor();
}

void handleSent() {
    // status change on sent success
    sentAck = true;
}

void handleReceived() {
    // status change on received success
    receivedAck = true;
}
void transmitPoll() {
    Serial.println("Poll Transmiting");
    msg_sent = 0;  
    DW1000.newTransmit();
    DW1000.setDefaults();
    
    data[0] = POLL;
    getAddress(data + 16);
    
    DW1000.setData(data, LEN_DATA);
    DW1000.startTransmit();
    noteActivity();
}

void transmitPollAck() {
  msg_sent = 0;
  DW1000.newTransmit();
  DW1000.setDefaults();
  
  data[0] = POLL_ACK;
  getAddress(data + 16);
  
  // delay the same amount as ranging tag
  DW1000Time deltaTime = DW1000Time(replyDelayTimeUS, DW1000Time::MICROSECONDS);
  DW1000.setDelay(deltaTime);
  DW1000.setData(data, LEN_DATA);

  DW1000.startTransmit();
  noteActivity();
}

void transmitRange() {
  msg_sent = 0;  
  DW1000.newTransmit();
  DW1000.setDefaults();
  
  data[0] = RANGE;
  getAddress(data + 16);
  
  // delay sending the message and remember expected future sent timestamp
  DW1000Time deltaTime = DW1000Time(replyDelayTimeUS, DW1000Time::MICROSECONDS);
  timeRangeSent = DW1000.setDelay(deltaTime);
  timePollSent.getTimestamp(data + 1);
  timePollAckReceived.getTimestamp(data + 6);
  timeRangeSent.getTimestamp(data + 11);
  DW1000.setData(data, LEN_DATA);
  receivedAck = false;  // in case we just received something, ignore it
  DW1000.startTransmit();
    //Serial.print("Expect RANGE to be sent @ "); Serial.println(timeRangeSent.getAsFloat());
  noteActivity();
}

int8_t checkReceiver() {
  byte forWho = data[16];
  byte fromWhom = data[17];
  Serial.print("For: ");
  Serial.println(forWho);
  Serial.print(" from: ");
  Serial.println(fromWhom);
  if (forWho != MY_NUM) {
        Serial.println('n');
    return 1;
  } else if ( ((OTHER_NUM == 0 ) || (OTHER_NUM == 1)) && ( expectedMsgId == POLL) ) { 
    Serial.println("being polled by 0");
    OTHER_NUM = fromWhom;
    return 0; 
  } else if (fromWhom != OTHER_NUM) {
    Serial.print('N');
    Serial.print(OTHER_NUM);
    Serial.println(fromWhom);
    OTHER_NUM = fromWhom;
    return 2;
  }
  else {
    Serial.println('G');
    return 0;
  }
}

void receiver() {
    DW1000.newReceive();
    DW1000.setDefaults();
    // so we don't need to restart the receiver manually
    DW1000.receivePermanently(true);
    DW1000.startReceive();
    noteActivity();
}

void transmitRangeReport(float curRange) {
  DW1000.newTransmit();
  DW1000.setDefaults();
  
  data[0] = RANGE_REPORT;
  getAddress(data + 16);
  // write final ranging result
  memcpy(data + 1, &curRange, 4);
  DW1000.setData(data, LEN_DATA);
  DW1000.startTransmit();
  noteActivity();
}

void getAddress(byte the_data[]) {
  memset(the_data, 0, 2);
  the_data[0] = OTHER_NUM; // who's it for
  the_data[1] = MY_NUM; // return address
}

void loop() {
   switch( my_state ) {
    case TAG:
      loop_tag();
      break;
    case ANCHOR:
      loop_anchor();
      break;
    }
}

void computeRangeAsymmetric() {
    // asymmetric two-way ranging (more computation intense, less error prone)
    DW1000Time round1 = (timePollAckReceived - timePollSent).wrap();
    DW1000Time reply1 = (timePollAckSent - timePollReceived).wrap();
    DW1000Time round2 = (timeRangeReceived - timePollAckSent).wrap();
    DW1000Time reply2 = (timeRangeSent - timePollAckReceived).wrap();
    DW1000Time tof = (round1 * round2 - reply1 * reply2) / (round1 + round2 + reply1 + reply2);
    // set tof timestamp
    timeComputedRange.setTimestamp(tof);
}

void noteActivity() {
    // update activity timestamp, so that we do not reach "resetPeriod"
    lastActivity = millis();
}

void resetInactive() {
    // anchor listens for POLL
  if ( my_state == ANCHOR ) {

    // we didn't fully send a message
    if ( msg_sent == 0) {
      if ( expectedMsgId == RANGE ) {
	transmitPollAck();
      }
      else {
	freezeError(1);	
      }
    } else { // we'll restart
      OTHER_NUM = 0;
      receiver();
    }
  } // end ANCHOR

  /* ######################## TAG #################################### */

  else {
    // check for didn't send a message
    if ( msg_sent == 0 ) {
      if ( expectedMsgId == POLL_ACK ) {
	transmitPoll();	
      }
      else if (expectedMsgId == RANGE_REPORT) {
	transmitRange();
      }
      else
	freezeError(3);
    }

    // didn't receive a msg
    else {
      numTimeouts++;
      if (numTimeouts > maxNumTimeouts) {
	  becomeAnchor(); //TODO FIX
      } else {
      Serial.println('t');
      expectedMsgId = POLL_ACK;
      transmitPoll();
      }
    }
  }
  noteActivity();
}

void becomeTag() {
  resetPeriod = 250;
  my_state = TAG;
  msg_sent = 1;
  //  Serial.println("------------TAG-----------");
  //  delay(transition_delay);
  
  // transmit a poll msg
  expectedMsgId = POLL_ACK;
  receiver();
  transmitPoll();
  noteActivity();
  numTimeouts = 0;
}

void becomeAnchor() { // can I move the bulk of this to just the initializing state ?
  resetPeriod = 500;   // wait longer as a tag for a msg as an anchor
  my_state = ANCHOR;
  OTHER_NUM = 0;
  msg_sent = 1;
  //  Serial.println("----------ANCHOR----------");
  // delay(transition_delay);
  // wait for a poll msg
  Serial.println(OTHER_NUM);
  expectedMsgId = POLL;
  receiver();
  noteActivity();
  // for first time ranging frequency computation
  rangingCountPeriod = millis();
}


void loop_tag() {
//  Serial.print('l');3
  //Serial.println("Tag");
  handleSerial();

  delay(0.01);
  if (!sentAck && !receivedAck) {
    // check if inactive
    if (millis() - lastActivity > resetPeriod) {
      resetInactive();
    }
    return;
  }
  // continue on any success confirmation  
  if (sentAck) {
    sentAck = false;
    byte msgId = data[0];
    msg_sent = 1;
    if (msgId == POLL) {
      DW1000.getTransmitTimestamp(timePollSent);
      Serial.println('p');
      Serial.print("Sent POLL @ "); Serial.println(timePollSent.getAsFloat());
    } else if (msgId == RANGE) {
      DW1000.getTransmitTimestamp(timeRangeSent);
      Serial.println('r');
    }
    noteActivity();
  }
  if (receivedAck) {
    //    Serial.print('R');
    receivedAck = false;
    // get message and parse
    Serial.println("Rec_Ack");
    DW1000.getData(data, LEN_DATA);
    byte id = data[0];
    // check address of the message to make sure it's for us
    if (checkReceiver()) {
      Serial.println("checking Rec");
      data[0] = id;
      return;
    }
    byte msgId = data[0];
    if (msgId != expectedMsgId) { // start over again
	  expectedMsgId = POLL_ACK;
	  transmitPoll();
	  noteActivity();
      return;
    }
    if (msgId == POLL_ACK) {
      Serial.println('o');
      DW1000.getReceiveTimestamp(timePollAckReceived);
      expectedMsgId = RANGE_REPORT;
      transmitRange();      
      noteActivity();
      //      Serial.println("POLL_ACK");
    } else if (msgId == RANGE_REPORT) {
      //      Serial.print("RANGE REPORT: ");
      expectedMsgId = POLL_ACK;
      float curRange;
      memcpy(&curRange, data + 1, 4);
      static int ranged = 0;
      ranged++;
      last_anchor_distance[OTHER_NUM-2] = curRange;
      if (ranged >= 4){
        trilat2D_4A();
        ranged = 0;
        Serial.print("P= ");
        Serial.print(current_tag_position[0]);
        Serial.write(',');
        Serial.println(current_tag_position[1]);
      }
      Serial.println("");
      Serial.print("c="); // tell localino node we completed
      Serial.println(curRange);

      //print2msi(curRange); // send the range to the msi
      becomeAnchor(); // we got the distance !!
      noteActivity();
      }
    else
      freezeError(9);
    }
  }


void loop_anchor() {
  //Serial.println("Anchor");
  handleSerial();

  int32_t curMillis = millis(); 
  if (!sentAck && !receivedAck) {
    // check if inactive
    if (curMillis - lastActivity > resetPeriod) {
      resetInactive();
    }
    return;
  }

  // continue on any success confirmation
  if (sentAck) {
    sentAck = false;
    byte msgId = data[0];
    msg_sent = 1;
    if (msgId == POLL_ACK) {
      DW1000.getTransmitTimestamp(timePollAckSent);
    }
    else if (msgId == RANGE_REPORT) {
      OTHER_NUM = 0; // open it up to listen to anyone
    }
    else
      freezeError(8);
    noteActivity();
  }
  if (receivedAck) {
    receivedAck = false;
    // get message and parse
    DW1000.getData(data, LEN_DATA);
    byte id = data[0];
    // check address of the message to make sure it's for us
    if (checkReceiver()) {
      data[0] = id;
      return;
      }
    
    byte msgId = data[0];
    if (msgId != expectedMsgId) {
      expectedMsgId = POLL;
      OTHER_NUM = 0;
      noteActivity();
      return;
    }
    if (msgId == POLL) {
      // on POLL we (re-)start, so no protocol failure
      DW1000.getReceiveTimestamp(timePollReceived);
      expectedMsgId = RANGE;
      transmitPollAck();
      noteActivity();
      //      Serial.println("\rPOLL");
    }
    else if (msgId == RANGE) {
      DW1000.getReceiveTimestamp(timeRangeReceived);
      expectedMsgId = POLL;
      timePollSent.setTimestamp(data + 1);
      timePollAckReceived.setTimestamp(data + 6);
      timeRangeSent.setTimestamp(data + 11);
      // (re-)compute range as two-way ranging is done
      computeRangeAsymmetric(); // CHOSEN RANGING ALGORITHM
      float distance = timeComputedRange.getAsMeters();
      transmitRangeReport(distance);
      noteActivity();	// update sampling rate (each second)
      Serial.print("ranged");
      Serial.println(distance);
      OTHER_NUM = 0;
    }
  }
}

void handleSerial(){
  char in = 'z';
  while(Serial.available() > 0){
    in = Serial.read();
    if (in == 'a'){becomeAnchor();}
    if (in == 't'){
      while (Serial.available() == 0) {}
      in = Serial.read();
      Serial.print("IN: ");
      Serial.println(in);
      in = in - '0';
      OTHER_NUM = (byte)in;
      Serial.print("OTHERNUM: ");
      Serial.println(OTHER_NUM);
      becomeTag();
      }
  }
}

void reply_ack(byte num) {
  Serial.print('a');
  Serial.print(num);
}


int trilat2D_4A(void) {

  // for method see technical paper at
  // https://www.th-luebeck.de/fileadmin/media_cosa/Dateien/Veroeffentlichungen/Sammlung/TR-2-2015-least-sqaures-with-ToA.pdf
  // S. James Remington 1/2022
  //
  // A nice feature of this method is that the normal matrix depends only on the anchor arrangement
  // and needs to be inverted only once. Hence, the position calculation should be robust.
  //
  static bool first = true;  //first time through, some preliminary work
  float d[N_ANCHORS]; //temp vector, distances from anchors

  static float A[N_ANCHORS - 1][2], Ainv[2][2], b[N_ANCHORS - 1], kv[N_ANCHORS]; //calculated in first call, used in later calls

  int i, j, k;
  // copy distances to local storage
  for (i = 0; i < N_ANCHORS; i++) d[i] = last_anchor_distance[i];

#ifdef DEBUG_TRILAT
  char line[60];
  snprintf(line, sizeof line, "d: %6.2f %6.2f %6.2f", d[0], d[1], d[2]);
 //Serial.println(line);
#endif

  if (first) {  //intermediate fixed vectors
    first = false;

    float x[N_ANCHORS], y[N_ANCHORS]; //intermediate vectors

    for (i = 0; i < N_ANCHORS; i++) {
      x[i] = anchor_matrix[i][0];
      y[i] = anchor_matrix[i][1];
      kv[i] = x[i] * x[i] + y[i] * y[i];
    }

    // set up least squares equation

    for (i = 1; i < N_ANCHORS; i++) {
      A[i - 1][0] = x[i] - x[0];
      A[i - 1][1] = y[i] - y[0];
#ifdef DEBUG_TRILAT
      snprintf(line, sizeof line, "A  %5.2f %5.2f", A[i - 1][0], A[i - 1][1]);
     //Serial.println(line);
#endif
    }
    float ATA[2][2];  //calculate A transpose A
    // Cij = sum(k) (Aki*Akj)
    for (i = 0; i < 2; i++) {
      for (j = 0; j < 2; j++) {
        ATA[i][j] = 0.0;
        for (k = 0; k < N_ANCHORS - 1; k++) ATA[i][j] += A[k][i] * A[k][j];
      }
    }
#ifdef DEBUG_TRILAT
    snprintf(line, sizeof line, "ATA %5.2f %5.2f\n    %5.2f %5.2f", ATA[0][0], ATA[0][1], ATA[1][0], ATA[1][1]);
   //Serial.println(line);
#endif

    //invert ATA
    float det = ATA[0][0] * ATA[1][1] - ATA[1][0] * ATA[0][1];
    if (fabs(det) < 1.0E-4) {
     //Serial.println("***Singular matrix, check anchor coordinates***");
      while (1) delay(1); //hang
    }
    det = 1.0 / det;
    //scale adjoint
    Ainv[0][0] =  det * ATA[1][1];
    Ainv[0][1] = -det * ATA[0][1];
    Ainv[1][0] = -det * ATA[1][0];
    Ainv[1][1] =  det * ATA[0][0];
#ifdef DEBUG_TRILAT
    snprintf(line, sizeof line, "Ainv %7.4f %7.4f\n     %7.4f %7.4f", Ainv[0][0], Ainv[0][1], Ainv[1][0], Ainv[1][1]);
   //Serial.println(line);
    snprintf(line, sizeof line, "det Ainv %8.3e", det);
   //Serial.println(line);
#endif

  } //end if (first);

  //least squares solution for position
  //solve:  (x,y) = 0.5*(Ainv AT b)

  for (i = 1; i < N_ANCHORS; i++) {
    b[i - 1] = d[0] * d[0] - d[i] * d[i] + kv[i] - kv[0];
  }

  float ATb[2] = {0.0}; //A transpose b
  for (i = 0; i < N_ANCHORS - 1; i++) {
    ATb[0] += A[i][0] * b[i];
    ATb[1] += A[i][1] * b[i];
  }

  current_tag_position[0] = 0.5 * (Ainv[0][0] * ATb[0] + Ainv[0][1] * ATb[1]); //x
  current_tag_position[1] = 0.5 * (Ainv[1][0] * ATb[0] + Ainv[1][1] * ATb[1]); //y
  // calculate rms error for distances
  float rmse = 0.0, dc0 = 0.0, dc1 = 0.0, dc2 = 0.0;
  for (i = 0; i < N_ANCHORS; i++) {
    dc0 = current_tag_position[0] - anchor_matrix[i][0];
    dc1 = current_tag_position[1] - anchor_matrix[i][1];
    dc2 = anchor_matrix[i][2]; //include known Z coordinate of anchor
    dc0 = d[i] - sqrt(dc0 * dc0 + dc1 * dc1 + dc2 * dc2);
    rmse += dc0 * dc0;
  }
  current_distance_rmse = sqrt(rmse / ((float)N_ANCHORS)); //error
  return 1;
}  //end trilat2D_3A
