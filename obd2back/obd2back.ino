/*************************************************************************
* Testing sketch for Freematics OBD-II UART Adapter V1/V2/V2.1
* Performs AT command-set test
* Reads and prints several OBD-II PIDs value
* Reads and prints motion sensor data if available
* Distributed under BSD
* Visit https://freematics.com/products for more product information
* Written by Stanley Huang <stanley@freematics.com.au>
*************************************************************************/

#include <OBD2UART.h>

// On Arduino Leonardo, Micro, MEGA or DUE, hardware serial can be used for output as the adapter occupies Serial1
// On Arduino UNO and those have no Serial1, we use software serial for output as the adapter uses Serial
#ifdef ARDUINO_AVR_UNO

#else
#define mySerial1 Serial
#endif

#if defined(ESP32) && !defined(Serial1)
HardwareSerial Serial1(1);
#endif
#include <SoftwareSerial.h>
SoftwareSerial mySerial(11, 10);
COBD obd;
bool hasMEMS;

void testATcommands()
{
    static const char cmds[][6] = {"ATZ\r", "ATI\r", "ATH0\r", "ATRV\r", "0100\r", "010C\r", "0902\r"};
    char buf[128];

    for (byte i = 0; i < sizeof(cmds) / sizeof(cmds[0]); i++) {
        const char *cmd = cmds[i];
        mySerial1.print("Sending ");
        mySerial1.println(cmd);
        if (obd.sendCommand(cmd, buf, sizeof(buf))) {
            char *p = strstr(buf, cmd);
            if (p)
                p += strlen(cmd);
            else
                p = buf;
            while (*p == '\r') p++;
            while (*p) {
                mySerial1.write(*p);
                if (*p == '\r' && *(p + 1) != '\r')
                    mySerial1.write('\n');
                p++;
            }
            mySerial1.println();
        } else {
            mySerial1.println("Timeout");
        }
        delay(1000);
    }
    mySerial1.println();
}

void readPIDSingle()
{
    int value;
    mySerial1.print('[');
    mySerial1.print(millis());
    mySerial1.print(']');
    mySerial1.print("RPM=");
    if (obd.readPID(PID_RPM, value)) {
      mySerial1.print(value);
    }
    mySerial1.println();
}

void readPIDMultiple()
{   
    char nombre [10][50] = {"DISTANCIA","GASOLINA","RPM","VELOCIDAD","CARGA DE MOTOR","TROOTTLE","TEMPERATURA ANTICONGELANTE"};
    static const byte pids[] = {PID_DISTANCE,PID_FUEL_LEVEL ,PID_RPM,PID_SPEED, PID_ENGINE_LOAD, PID_THROTTLE, PID_COOLANT_TEMP};
    int values[sizeof(pids)];
   
    //if (obd.readPID(pids, sizeof(pids), values) == sizeof(pids)) {
    if (obd.readPID(pids, sizeof(pids), values)) {
      mySerial1.print('[');
      mySerial1.print(millis());
      mySerial1.println(']');
      for (byte i = 0; i < sizeof(pids) ; i++) {
        mySerial1.print(nombre[i]);
        mySerial1.print((int)pids[i] | 0x100, HEX);
        mySerial1.print('=');
        mySerial1.print(values[i]);
        mySerial1.println(' ');
       }
       mySerial1.println();
    }
}

void readBatteryVoltage()
{
  mySerial1.print('[');
  mySerial1.print(millis());
  mySerial1.print(']');
  mySerial1.print("Battery:");
  mySerial1.print(obd.getVoltage(), 1);
  mySerial1.println('V');
}

void readMEMS()
{
  int16_t acc[3] = {0};
  int16_t gyro[3] = {0};
  int16_t mag[3] = {0};
  int16_t temp = 0;

  if (!obd.memsRead(acc, gyro, mag, &temp)) return;

  mySerial1.print('[');
  mySerial1.print(millis());
  mySerial1.print(']');

  mySerial1.print("ACC:");
  mySerial1.print(acc[0]);
  mySerial1.print('/');
  mySerial1.print(acc[1]);
  mySerial1.print('/');
  mySerial1.print(acc[2]);

  mySerial1.print(" GYRO:");
  mySerial1.print(gyro[0]);
  mySerial1.print('/');
  mySerial1.print(gyro[1]);
  mySerial1.print('/');
  mySerial1.print(gyro[2]);

  mySerial1.print(" MAG:");
  mySerial1.print(mag[0]);
  mySerial1.print('/');
  mySerial1.print(mag[1]);
  mySerial1.print('/');
  mySerial1.print(mag[2]);

  mySerial1.print(" TEMP:");
  mySerial1.print((float)temp / 10, 1);
  mySerial1.println("C");
}

void setup()
{
  mySerial1.begin(115200);
  while (!mySerial1);
  
  for (;;) {
    delay(1000);
    byte version = obd.begin();
    mySerial1.print("Freematics OBD-II Adapter ");
    if (version > 0) {
      mySerial1.println("detected");
      mySerial1.print("OBD firmware version ");
      mySerial1.print(version / 10);
      mySerial1.print('.');
      mySerial1.println(version % 10);
      break;
    } else {
      mySerial1.println("not detected");
    }
  }

  // send some commands for testing and show response for debugging purpose
  testATcommands();

  hasMEMS = obd.memsInit();
  mySerial1.print("MEMS:");
  mySerial1.println(hasMEMS ? "Yes" : "No");
  obd.init();
  /*
  // initialize OBD-II adapter
  do {
    mySerial1.println("Connecting...");
  } while (!obd.init());
  mySerial1.println("OBD connected!");
*/
  char buf[64];
  if (obd.getVIN(buf, sizeof(buf))) {
      mySerial1.print("VIN:");
      mySerial1.println(buf);
  }
  
  uint16_t codes[6];
  byte dtcCount = obd.readDTC(codes, 6);
  if (dtcCount == 0) {
    mySerial1.println("No DTC"); 
  } else {
    mySerial1.print(dtcCount); 
    mySerial1.print(" DTC:");
    for (byte n = 0; n < dtcCount; n++) {
      mySerial1.print(' ');
      mySerial1.print(codes[n], HEX);
    }
    mySerial1.println();
  }
  delay(5000);
}

void loop()
{
  
  //readPIDSingle();
  readPIDMultiple();
  readBatteryVoltage();
  if (hasMEMS) {
    readMEMS();
  }
}

