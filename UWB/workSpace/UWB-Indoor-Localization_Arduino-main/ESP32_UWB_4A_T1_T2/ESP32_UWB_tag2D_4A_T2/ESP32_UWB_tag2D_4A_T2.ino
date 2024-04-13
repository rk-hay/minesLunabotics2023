// currently tag is module #5
// this version is 2D (X,Y) only, 4 or more anchors (overdetermined linear least square solution)
// The Z coordinates of anchors and tag are all assumed to be zero, so for highest accuracy
// the anchors should be approximately in the same horizontal plane as the tag.
// S. James Remington 1/2022

// This code does not average position measurements!
#define INACTIVITY_TIME 15000
#include <SPI.h>
#include <esp_now.h>
#include "DW1000Ranging.h"
#include "DW1000.h"
#include <WiFi.h>
//#define DEBUG_TRILAT   //prints in trilateration code
#define DEBUG_DIST     //print anchor distances

#define SPI_SCK 18
#define SPI_MISO 19
#define SPI_MOSI 23
#define DW_CS 4

//-------------------------------------------------------------------
String SYNC_MSG = "SYNC";
#define DEVICE_ID 2
#define FRAME_DURATION_MS 150  // 10 milliseconds
#define NUM_TIME_SLOTS 2
#define SLOT_DURATION_MS (FRAME_DURATION_MS / NUM_TIME_SLOTS)
#define MY_SLOT_INDEX DEVICE_ID - 1
#define SYNC_INTERVAL 5000  // 5 seconds

unsigned long lastSyncTime = 0;
long globalTurn =0;
boolean sent = false;
volatile boolean sentAck = false;
volatile boolean received = false;
volatile boolean error = false;
String message;
//-------------------------------------------------------------------

// connection pins
const uint8_t PIN_RST = 27;  // reset pin
const uint8_t PIN_IRQ = 34;  // irq pin
const uint8_t PIN_SS = 4;    // spi select pin
uint8_t T1_using = true;
uint8_t T1_x = 0.0;
uint8_t T1_y = 0.0;

typedef struct struct_message {
  long turn;
  float x;
  float y;
} struct_message;

struct_message Signal;
struct_message incomingSignal;
// TAG antenna delay defaults to 16384
// leftmost two bytes below will become the "short address"
char tag_addr[] = "8D:00:22:EA:82:60:3B:9C";  // "8D:00:22:EA:82:60:3B:9C"
const uint8_t send_to[] = { 0x7D, 0x00, 0x22, 0xEA, 0x82, 0x60 };
String success;
esp_now_peer_info_t peerInfo;

const char *ssid = "Hay";
const char *password = "";
const char *host = "192.168.111.8";
WiFiClient client;
WiFiClient ping;

// variables for position determination
#define N_ANCHORS 4
#define ANCHOR_DISTANCE_EXPIRED 5000  //measurements older than this are ignore (milliseconds)

// global variables, input and output

float anchor_matrix[N_ANCHORS][3] = {
  //list of anchor coordinates, relative to chosen origin.
  { 0.0, 0.0, 0.0 },     //Anchor labeled #1
  { 1.524, 0, 0.0 },     //Anchor labeled #2
  { 1.524, 2.15, 0.0 },  //Anchor labeled #3
  { 0, 2.15, 0.0 }       //Anchor labeled #4
};                       //Z values are ignored in this code, except to compute RMS distance error

uint32_t last_anchor_update[N_ANCHORS] = { 0 };   //millis() value last time anchor was seen
float last_anchor_distance[N_ANCHORS] = { 0.0 };  //most recent distance reports

float current_tag_position[2] = { 0.0, 0.0 };  //global current position (meters with respect to anchor origin)
float current_distance_rmse = 0.0;             //rms error in distance calc => crude measure of position error (meters).  Needs to be better characterized

void setup() {

  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFi.setSleep(false);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected");
  Serial.print("IP Address:");
  Serial.println(WiFi.localIP());

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_send_cb(OnDataSent);
  memcpy(peerInfo.peer_addr, send_to, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }
  esp_now_register_recv_cb(OnDataRecv);
  delay(1000);



  //initialize configuration
  SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);
  DW1000Ranging.initCommunication(PIN_RST, PIN_SS, PIN_IRQ);  //Reset, CS, IRQ pin

  DW1000Ranging.attachNewRange(newRange);
  DW1000Ranging.attachNewDevice(newDevice);
  DW1000Ranging.attachInactiveDevice(inactiveDevice);

  // start as tag, do not assign random short address


  DW1000Ranging.startAsTag(tag_addr, DW1000.MODE_LONGDATA_RANGE_LOWPOWER, false);
  //synchronize();
}

void loop() {
   if (globalTurn == MY_SLOT_INDEX){
    DW1000Ranging.loop();
  }
}

// collect distance data from anchors, presently configured for 4 anchors
// solve for position if all four current

void newRange() {
    int i;                                                                   //index of this anchor, expecting values 1 to 7
    int index = DW1000Ranging.getDistantDevice()->getShortAddress() & 0x07;  //the 2nd number in the array of the addr i.e 3 in  "83:00:5B:D5:A9:9A:E2:9C"
    if (index > 0) {
      last_anchor_update[index - 1] = millis();                            //if we just recieved 3, then we note down when we got it
      float range = DW1000Ranging.getDistantDevice()->getRange();          //get distance to 3
      last_anchor_distance[index - 1] = range;                             //put it in an array
      if (range < 0.0 || range > 30.0) last_anchor_update[index - 1] = 0;  //error or out of bounds, ignore this measurement
    }

    int detected = 0;

    //reject old measurements
    for (i = 0; i < N_ANCHORS; i++) {
      if (millis() - last_anchor_update[i] > ANCHOR_DISTANCE_EXPIRED) last_anchor_update[i] = 0;  //not from this one
      if (last_anchor_update[i] > 0) detected++;
    }

#ifdef DEBUG_DIST
    // print distance and age of measurement
    uint32_t current_time = millis();
    for (i = 0; i < N_ANCHORS; i++) {
      Serial.print(i + 1);  //ID
      Serial.print("> ");
      Serial.print(last_anchor_distance[i]);
      Serial.print("\t");
      Serial.println(current_time - last_anchor_update[i]);  //age in millis
    }
#endif

    if (detected == 4) {  //four measurements minimum
      trilat2D_4A();
      //output the values (X, Y and error estimate)
      Serial.print("P= ");
      Serial.print(current_tag_position[0]);
      Serial.write(',');
      Serial.println(current_tag_position[1]);
      // Serial.write(',');
      // Serial.println(current_distance_rmse);
      Signal.x = current_tag_position[0];
      Signal.y = current_tag_position[1];
      static long hold = 0;
      if (millis() - hold > 100) {
        esp_now_send(send_to, (uint8_t *)&Signal, sizeof(Signal));
        hold = millis();
      }
    }
}  //end newRange

void newDevice(DW1000Device *device) {
  Serial.print("Device added: ");
  Serial.println(device->getShortAddress(), HEX);
}

void inactiveDevice(DW1000Device *device) {
  Serial.print("delete inactive device: ");
  Serial.println(device->getShortAddress(), HEX);
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
  float d[N_ANCHORS];        //temp vector, distances from anchors

  static float A[N_ANCHORS - 1][2], Ainv[2][2], b[N_ANCHORS - 1], kv[N_ANCHORS];  //calculated in first call, used in later calls

  int i, j, k;
  // copy distances to local storage
  for (i = 0; i < N_ANCHORS; i++) d[i] = last_anchor_distance[i];

#ifdef DEBUG_TRILAT
  char line[60];
  snprintf(line, sizeof line, "d: %6.2f %6.2f %6.2f", d[0], d[1], d[2]);
  Serial.println(line);
#endif

  if (first) {  //intermediate fixed vectors
    first = false;

    float x[N_ANCHORS], y[N_ANCHORS];  //intermediate vectors

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
      Serial.println(line);
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
    Serial.println(line);
#endif

    //invert ATA
    float det = ATA[0][0] * ATA[1][1] - ATA[1][0] * ATA[0][1];
    if (fabs(det) < 1.0E-4) {
      Serial.println("***Singular matrix, check anchor coordinates***");
      while (1) delay(1);  //hang
    }
    det = 1.0 / det;
    //scale adjoint
    Ainv[0][0] = det * ATA[1][1];
    Ainv[0][1] = -det * ATA[0][1];
    Ainv[1][0] = -det * ATA[1][0];
    Ainv[1][1] = det * ATA[0][0];
#ifdef DEBUG_TRILAT
    snprintf(line, sizeof line, "Ainv %7.4f %7.4f\n     %7.4f %7.4f", Ainv[0][0], Ainv[0][1], Ainv[1][0], Ainv[1][1]);
    Serial.println(line);
    snprintf(line, sizeof line, "det Ainv %8.3e", det);
    Serial.println(line);
#endif

  }  //end if (first);

  //least squares solution for position
  //solve:  (x,y) = 0.5*(Ainv AT b)

  for (i = 1; i < N_ANCHORS; i++) {
    b[i - 1] = d[0] * d[0] - d[i] * d[i] + kv[i] - kv[0];
  }

  float ATb[2] = { 0.0 };  //A transpose b
  for (i = 0; i < N_ANCHORS - 1; i++) {
    ATb[0] += A[i][0] * b[i];
    ATb[1] += A[i][1] * b[i];
  }

  current_tag_position[0] = 0.5 * (Ainv[0][0] * ATb[0] + Ainv[0][1] * ATb[1]);  //x
  current_tag_position[1] = 0.5 * (Ainv[1][0] * ATb[0] + Ainv[1][1] * ATb[1]);  //y

  // calculate rms error for distances
  float rmse = 0.0, dc0 = 0.0, dc1 = 0.0, dc2 = 0.0;
  for (i = 0; i < N_ANCHORS; i++) {
    dc0 = current_tag_position[0] - anchor_matrix[i][0];
    dc1 = current_tag_position[1] - anchor_matrix[i][1];
    dc2 = anchor_matrix[i][2];  //include known Z coordinate of anchor
    dc0 = d[i] - sqrt(dc0 * dc0 + dc1 * dc1 + dc2 * dc2);
    rmse += dc0 * dc0;
  }
  current_distance_rmse = sqrt(rmse / ((float)N_ANCHORS));  //error

  return 1;
}  //end trilat2D_3A


void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  //Serial.print("\r\nLast Packet Send Status:\t");
  //Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  if (status == 0) {
    success = "Delivery Success :)";
  } else {
    success = "Delivery Fail :(";
  }
}
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&incomingSignal, incomingData, sizeof(incomingSignal));
  //Serial.print("Time: ");
  globalTurn = incomingSignal.turn;
  //Serial.println(globalTime);
  T1_x = incomingSignal.x;
  T1_y = incomingSignal.y;
}


//-------------------------------------------------------------------
//-------------------------------------------------------------------