#include <OBD2UART.h>

COBD obd;

void setup()
{
  Serial.begin(9600);
  // we'll use the debug LED as output
  pinMode(LED_BUILTIN, OUTPUT);
  // start serial communication
  obd.begin();
  Serial.print("beginnn");
  // initiate OBD-II connection until success
  while (!obd.init(PROTO_KWP2000_FAST));
  Serial.print("end");
}

void loop()
{
  Serial.print("while");
  int value;
  // save engine RPM in variable 'value', return true on success
  if (obd.readPID(PID_RPM, value)) {
    Serial.print(value);
    // light on LED on Arduino board when the RPM exceeds 3000
    digitalWrite(LED_BUILTIN, value > 1500 ? HIGH : LOW);
    Serial.print("inside if");
  }else{
    Serial.print("nothing");
    }
    Serial.print("... ");
}
