
// Define variables to store received data
char identifier;
float linearX, linearY, angularZ;

void setup() {
  // Start serial communication with a baud rate of 9600
  Serial.begin(1000000);
}

void loop() {
  // Check if there is data available to read
  if (Serial.available() > 0) {
    // Read the first byte to identify the message type
    identifier = Serial.read();

    // Check the identifier and read corresponding data
    if (identifier == 'S') {
      // Read the next 12 bytes as three 32-bit floating-point values
      Serial.readBytes((char*)&linearX, sizeof(float));
      Serial.readBytes((char*)&linearY, sizeof(float));
      Serial.readBytes((char*)&angularZ, sizeof(float));

      // Process the received data as needed
      // For example, print the values to the Serial Monitor
      Serial.print("Linear X: ");
      Serial.println(linearX);
      Serial.print("Linear Y: ");
      Serial.println(linearY);
      Serial.print("Angular Z: ");
      Serial.println(angularZ);
    }
  }
}
