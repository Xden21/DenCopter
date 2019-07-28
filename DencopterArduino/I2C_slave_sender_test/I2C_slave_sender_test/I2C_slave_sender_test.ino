#include <Wire.h>

void setup() {
  Wire.begin(4);            //address 0x04
  Wire.onRequest(sendData); //sendData is the eventfunction handled here
}

//eventfunction that sends the data
void sendData() {
  byte buffer[] = {1, 2,3,4,5,6,7,8,9,10};
  Wire.write(buffer,10);
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(100);
}
