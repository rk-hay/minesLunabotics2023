#include <esp_now.h>
#include <SPI.h>
#include "DW1000Ranging.h"
#include "DW1000.h"
#include <WiFi.h>

#define SPI_SCK 18
#define SPI_MISO 19
#define SPI_MOSI 23
#define DW_CS 4
uint8_t T1_using = true;
uint8_t T1_x = 0.0;
uint8_t T1_y = 0.0;

typedef struct struct_message {
    uint8_t isUsing;
    uint8_t x;
    uint8_t y;
} struct_message;

struct_message Signal;
struct_message incomingSignal;
// connection pins
const uint8_t PIN_RST = 27; // reset pin
const uint8_t PIN_IRQ = 34; // irq pin
const uint8_t PIN_SS = 4;   // spi select pin

// TAG antenna delay defaults to 16384
// leftmost two bytes below will become the "short address"
char tag_addr[] = "8D:00:22:EA:82:60:3B:9C"; // "8D:00:22:EA:82:60:3B:9C"
const uint8_t send_to[] = {0x7D, 0x00, 0x22, 0xEA, 0x82, 0x60};
String success;
esp_now_peer_info_t peerInfo;

const char *ssid = "Hay";
const char *password = "";
const char *host = "192.168.107.8";
WiFiClient client;
WiFiClient ping;
// variables for position determination
#define N_ANCHORS 2
#define ANCHOR_DISTANCE_EXPIRED 5000   //measurements older than this are ignore (milliseconds)

// global variables, input and output

float anchor_matrix[N_ANCHORS][3] = { //list of anchor coordinates, relative to chosen origin.
  {0.0, 0.0, 0.0},  //Anchor labeled #1
  {2.2606, 0.0, 0.1},//Anchor labeled #2
};  //Z values are ignored in this code

uint32_t last_anchor_update[N_ANCHORS] = {0}; //millis() value last time anchor was seen
float last_anchor_distance[N_ANCHORS] = {0.0}; //most recent distance reports

float current_tag_position[2] = {0.0, 0.0}; //global current position (meters with respect to anchor origin)
float current_distance_rmse = 0.0;  //rms error in distance calc => crude measure of position error (meters).  Needs to be better characterized
long runtime = 0;

void setup()
{

  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFi.setSleep(false);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
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
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
  esp_now_register_recv_cb(OnDataRecv);
  delay(1000);



  //initialize configuration
  SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);
  DW1000Ranging.initCommunication(PIN_RST, PIN_SS, PIN_IRQ); //Reset, CS, IRQ pin

  DW1000Ranging.attachNewRange(newRange);
  DW1000Ranging.attachNewDevice(newDevice);
  DW1000Ranging.attachInactiveDevice(inactiveDevice);

  // start as tag, do not assign random short address

  DW1000Ranging.startAsTag(tag_addr, DW1000.MODE_LONGDATA_RANGE_LOWPOWER, false);
}

void loop()
{
if (T1_using == false){
  Signal.isUsing = true;
  esp_err_t result = esp_now_send(send_to, (uint8_t *)&Signal, sizeof(Signal));
  if (result == ESP_OK) {
    Serial.println("Pinged T1: Wait");
  }
  else {
    Serial.println("No Ping");
  }
  Serial.println("ranging");
   DW1000Ranging.loop();
  Signal.x = current_tag_position[0];
  Signal.y =  current_tag_position[1];
  Signal.isUsing = false;
    result = esp_now_send(send_to, (uint8_t *)&Signal, sizeof(Signal));
  if (result == ESP_OK) {
    Serial.println("Pinged T1: Clear");
  }
  else {
    Serial.println("No Ping");
  }
//   delay(10);
}
}

// collect distance data from anchors, presently configured for 4 anchors
// solve for position if all four current

void newRange()
{
  int i; //indices, expecting values 1 to 2
  //index of this anchor, expecting values 1,2
  int index = DW1000Ranging.getDistantDevice()->getShortAddress() & 0x03; 
  
  if (index > 0) {
    last_anchor_update[index - 1] = millis();  //decrement for array index
    float range = DW1000Ranging.getDistantDevice()->getRange();
    last_anchor_distance[index-1] = range;
    if (range < 0.0 || range > 30.0)     last_anchor_update[index - 1] = 0;  //sanity check, ignore this measurement
  }
  
  int detected = 0;

  //reject old measurements
  for (i = 0; i < N_ANCHORS; i++) {
    if (millis() - last_anchor_update[i] > ANCHOR_DISTANCE_EXPIRED) last_anchor_update[i] = 0; //not from this one
    if (last_anchor_update[i] > 0) detected++;
  }

  if ( detected == 2) { 
    trilat2D_2A();
    //output the values (X, Y and error estimate)
    Serial.print("P= ");
    Serial.print(current_tag_position[0]);
    Serial.write(',');
    Serial.print(current_tag_position[1]);
    Serial.write(',');
    Serial.println(current_distance_rmse);
  }
}  //end newRange

void newDevice(DW1000Device *device)
{
  Serial.print("Device added: ");
  Serial.println(device->getShortAddress(), HEX);
}

void inactiveDevice(DW1000Device *device)
{
  Serial.print("delete inactive device: ");
  Serial.println(device->getShortAddress(), HEX);
}

int trilat2D_2A(void) {
  float d[N_ANCHORS]; //temp vector, distances from anchors
  int i;
  for (i = 0; i < N_ANCHORS; i++) d[i] = last_anchor_distance[i];
                            //Anchor      2  x cord
  current_tag_position[0] = (d[0]*d[0] - d[1]*d[1] + anchor_matrix[1][0]*anchor_matrix[1][0]) / (2*anchor_matrix[1][0]); 
  current_tag_position[1] = abs(sqrt(d[0] * d[0] - current_tag_position[0] * current_tag_position[0]));

  // calculate rms error for distances
  float rmse = 0.0, dc0 = 0.0, dc1 = 0.0;
  for (i = 0; i < N_ANCHORS; i++) {
    dc0 = current_tag_position[0] - anchor_matrix[i][0];
    dc1 = current_tag_position[1] - anchor_matrix[i][1];
    dc0 = d[i] - sqrt(dc0 * dc0 + dc1 * dc1);
    rmse += dc0 * dc0;
  }
  current_distance_rmse = sqrt(rmse / ((float)N_ANCHORS));
  Serial.print("D= ");
  Serial.print(d[0]);
  Serial.print(" ");
  Serial.print(d[1]);
  Serial.println(" ");
  return 1;
} //end trilat2D_2A

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  if (status ==0){
    success = "Delivery Success :)";
  }
  else{
    success = "Delivery Fail :(";
  }
}
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&incomingSignal, incomingData, sizeof(incomingSignal));
  Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.println(incomingSignal.isUsing);
  T1_using = incomingSignal.isUsing;
  T1_x = incomingSignal.x;
  T1_y = incomingSignal.y;
}