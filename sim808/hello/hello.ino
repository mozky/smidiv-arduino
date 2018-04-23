#include <SoftwareSerial.h>
SoftwareSerial mySerial(7, 8);
void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  mySerial.begin(9600);   
  Serial.begin(9600);   
  delay(100);
}

void loop() {
 if (Serial.available()>0) {
   digitalWrite(LED_BUILTIN, HIGH);
   mySerial.write(Serial.read());
 }

 if (mySerial.available()>0) {
   digitalWrite(LED_BUILTIN, LOW);
   Serial.write(mySerial.read());
 }
}
