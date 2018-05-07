#define BEARER_PROFILE_GPRS "AT+SAPBR=3,1,\"Contype\",\"GPRS\"\r\n"
#define BEARER_PROFILE_APN "AT+SAPBR=3,1,\"APN\",\"%s\"\r\n"
#define QUERY_BEARER "AT+SAPBR=2,1\r\n"
#define OPEN_GPRS_CONTEXT "AT+SAPBR=1,1\r\n"
#define CLOSE_GPRS_CONTEXT "AT+SAPBR=0,1\r\n"
#define HTTP_INIT "AT+HTTPINIT\r\n"
#define HTTP_CID "AT+HTTPPARA=\"CID\",1\r\n"
#define HTTP_PARA "AT+HTTPPARA=\"URL\",\"%s\"\r\n"
#define HTTP_GET "AT+HTTPACTION=0\r\n"
#define HTTP_POST "AT+HTTPACTION=1\n"
#define HTTP_DATA "AT+HTTPDATA=%d,%d\r\n"
#define HTTP_READ "AT+HTTPREAD\r\n"
#define HTTP_CLOSE "AT+HTTPTERM\r\n"
#define debugMode true
#define HTTP_CONTENT "AT+HTTPPARA=\"CONTENT\",\"application/json\"\r\n"
#define HTTPS_ENABLE "AT+HTTPSSL=1\r\n"
#define HTTPS_DISABLE "AT+HTTPSSL=0\r\n"
#define NORMAL_MODE "AT+CFUN=1,1\r\n"
#define REGISTRATION_STATUS "AT+CREG?\r\n"
#define SIGNAL_QUALITY "AT+CSQ\r\n"
#define READ_VOLTAGE "AT+CBC\r\n"
#define SLEEP_MODE "AT+CSCLK=1\r\n"
#define TRUE                    1
#define FALSE                   0
#define DEFAULT_TIMEOUT         5000
#define OK "OK\r\n"
#define DOWNLOAD "DOWNLOAD"
#define HTTP_2XX ",2XX,"
#define HTTPS_PREFIX "https://"
#define CONNECTED "+CREG: 0,1"
#define BEARER_OPEN "+SAPBR: 1,1"

unsigned long lastRunTime = 0;
unsigned long waitForRunTime = 0;

enum Result {
  SUCCESS = 0,
  ERROR_INITIALIZATION = 1,
  ERROR_BEARER_PROFILE_GPRS = 2,
  ERROR_BEARER_PROFILE_APN = 3,
  ERROR_OPEN_GPRS_CONTEXT = 4,
  ERROR_QUERY_GPRS_CONTEXT = 5,
  ERROR_CLOSE_GPRS_CONTEXT = 6,
  ERROR_HTTP_INIT = 7,
  ERROR_HTTP_CID = 8,
  ERROR_HTTP_PARA = 9,
  ERROR_HTTP_GET = 10,
  ERROR_HTTP_READ = 11,
  ERROR_HTTP_CLOSE = 12,
  ERROR_HTTP_POST = 13,
  ERROR_HTTP_DATA = 14,
  ERROR_HTTP_CONTENT = 15,
  ERROR_NORMAL_MODE = 16,
  ERROR_LOW_CONSUMPTION_MODE = 17,
  ERROR_HTTPS_ENABLE = 18,
  ERROR_HTTPS_DISABLE = 19
};
#include <ArduinoJson.h>
#include <SoftwareSerial.h>
SoftwareSerial mySerial(10,11);   //software serial instance for GPS/GSM module
#define DEBUG true 
String state, timegps, latitude, longitude, internetState;

void setup() {
  mySerial.begin(9600);
  Serial.begin(9600);           //open serial port
  delay(50);
  
  /*We can use old sendData function to print our commands*/
  Serial.println("Enviando info");
  sendData("AT+CGNSPWR=1",1000,DEBUG);       //Initialize GPS device
  delay(50);
  Serial.println("Enviando info2");
  sendData("AT+CGNSSEQ=RMC",1000,DEBUG);
  delay(150);
}

void loop() {
  getGPS();
  mandarequest();
   //apn();
   //connectNetwork();
   //connectNetwork();
   /*sendTabData("AT+CGNSINF",5000,DEBUG);    //send demand of gps localization & GPRS connection (no funciona todavía)
   if(state != 0){
    /*we just dont want to print empty signs so we wait until 
     * gps module will have connection to satelite.
    
     * when whole table returned:
   for(int i = 0; i < (sizeof(latlongtab)/sizeof(int)); i++){
      Serial.println(latlongtab[i]); // print 
    }*//*
    Serial.println("Time: ");
    Serial.println(timegps);
    Serial.println("Latitude:");
    Serial.println(latitude);
    Serial.println(" Longitude ");
    Serial.println(longitude);
   }else{
    Serial.println("GPS & Internet initializing");
   }*/
}
void getGPS()
{
  sendTabData("AT+CGNSINF",5000,DEBUG);    //send demand of gps localization & GPRS connection (no funciona todavía)
   if(state != 0&&latitude!=""){
    /*we just dont want to print empty signs so we wait until 
     * gps module will have connection to satelite.
    
     * when whole table returned:
     * */
     /*
   for(int i = 0; i < (sizeof(latlongtab)/sizeof(int)); i++){
      Serial.println(latlongtab[i]); // print 
    }*//**/
    Serial.println("Time: ");
    Serial.println(timegps);
    Serial.println("Latitude:");
    Serial.println(latitude);
    Serial.println(" Longitude ");
    Serial.println(longitude);
   }else{
    Serial.println("GPS & Internet initializing");
   }
  }
void print(const __FlashStringHelper *message, int code = -1){
  if (code != -1){
    Serial.print(message);
    Serial.println(code);
  }
  else {
    Serial.println(message);
  }
}

void mandarequest(){
  
  char response[90];
  char body[90];
  char otro[10];
  char otro1[10];
  char SMIDIVID[10] = "ABD10";
  Result result;
  print(F("Cofigure bearer: "), configureBearer("web.iusacellgsm.mx"));
   print(F("intentando el connect"));
  result = connect();
  print(F("HTTP connect: "), result);
  Serial.print("A latitud"+latitude);
  Serial.print("A longuitud"+longitude);
  latitude.toCharArray(otro,10);
  longitude.toCharArray(otro1,10);
  Serial.print("cosa");
  Serial.print(otro);
  Serial.print(otro1);
  
  sprintf(body, "{\r\n  \"smidivID\": \"%s\",\r\n  \"ubicacion\": {\r\n    \"lat\": %s,\r\n    \"lon\": %s\r\n  }}",SMIDIVID,otro,otro1);
  result = post("ee997bd2.ngrok.io/ubicacion", body, response);
  print(F("HTTP POST: "), result);
  if (result == SUCCESS) {
    Serial.println(response);
    StaticJsonBuffer<32> jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(response);
    lastRunTime = millis();
    waitForRunTime = root["waitForRunTime"];
    
    print(F("Last run time: "), lastRunTime);
    print(F("Next post in: "), waitForRunTime);
  }
  /*
  result = http.get("393ab7c0.ngrok.io/health", response);
  print(F("HTTP GET: "), result);
  if (result == SUCCESS) {
    Serial.println(response);
    StaticJsonBuffer<32> jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(response);
    lastRunTime = millis();
    waitForRunTime = root["waitForRunTime"];
    
    print(F("Last run time: "), lastRunTime);
    print(F("Next post in: "), waitForRunTime);
  }
  */
  memset(otro, 0, sizeof(otro));
  memset(otro1, 0, sizeof(otro1));
  latitude ="";
  longitude="";
  print(F("HTTP disconnect: "),disconnect());
}



void sendTabData(String command, const int timeout, boolean debug) {
  String latlongtab[5];  // for the purpose of example let array be global
 
    mySerial.println(command); 
    long int time = millis();
    int i = 0;   
    
    while((time+timeout) > millis()){
      while(mySerial.available()){       
        char c = mySerial.read();
        if(c != ','){ //read characters until you find comma, if found increment
          latlongtab[i]+=c;
          delay(100);
        }else{
          i++;        
        }
        if(i == 5){
          delay(100);
          goto exitL;
        }       
      }    
    }exitL:    
    if(debug){ 
      /*or you just can return whole table,in case if its not global*/ 
      state = latlongtab[1];     //state = recieving data - 1, not recieving - 0
      timegps = latlongtab[2];
      latitude = latlongtab[3]; //latitude
      longitude = latlongtab[4]; //longitude
    }    
}
Result connect() {

  Result result = SUCCESS;
  unsigned int attempts = 0;
  unsigned int MAX_ATTEMPTS = 10;

  while (sendCmdAndWaitForResp(QUERY_BEARER, BEARER_OPEN, 2000) == FALSE && attempts < MAX_ATTEMPTS){
    attempts ++;
    if (sendCmdAndWaitForResp(OPEN_GPRS_CONTEXT, OK, 2000) == FALSE){
      result = ERROR_OPEN_GPRS_CONTEXT;
    }
    else {
      result = SUCCESS;
    }
  }

  if (sendCmdAndWaitForResp(HTTP_INIT, OK, 2000) == FALSE)
    result = ERROR_HTTP_INIT;

  return result;
}

Result configureBearer(const char *apn){

  Result result = SUCCESS;

  unsigned int attempts = 0;
  unsigned int MAX_ATTEMPTS = 10;


  while (sendCmdAndWaitForResp(REGISTRATION_STATUS, CONNECTED, 2000) != TRUE && attempts < MAX_ATTEMPTS){
    sendCmdAndWaitForResp(READ_VOLTAGE, OK, 1000);
    sendCmdAndWaitForResp(SIGNAL_QUALITY, OK, 1000);
    attempts ++;
    delay(1000 * attempts);
    if (attempts == MAX_ATTEMPTS) {
      attempts = 0;
    }
  }

  if (sendCmdAndWaitForResp(BEARER_PROFILE_GPRS, OK, 2000) == FALSE)
    result = ERROR_BEARER_PROFILE_GPRS;

  char httpApn[64];
  sprintf(httpApn, BEARER_PROFILE_APN, apn);
  if (sendCmdAndWaitForResp(httpApn, OK, 2000) == FALSE)
    result = ERROR_BEARER_PROFILE_APN;

  return result;
}

String sendData(String command, const int timeout, boolean debug) {
  
    String response = "";    
    mySerial.println(command); 
    long int time = millis();
    int i = 0;  
     
    while( (time+timeout) > millis()){
      while(mySerial.available()){       
        char c = mySerial.read();
        response+=c;
      }  
    }    
    if(debug){
      Serial.print(response);
    }    
    return response;
}

int sendCmdAndWaitForResp(const char* cmd, const char *resp, unsigned timeout)
{
    delay(1000);
    mySerial.println(cmd);
    return waitForResp(resp,timeout);
}


int waitForResp(const char *resp, unsigned int timeout)
{
    int len = strlen(resp);
    int sum=0;
    unsigned long timerStart,timerEnd;
    timerStart = millis();

    while(1) {
        if(mySerial.available()) {
            char c = mySerial.read();
            if (debugMode) Serial.print(c);
            sum = (c == resp[sum] || resp[sum] == 'X') ? sum+1 : 0;
            if(sum == len)break;
        }
        timerEnd = millis();
        if(timerEnd - timerStart > timeout) {
            return FALSE;
        }
    }

    while(mySerial.available()) {
        mySerial.read();
    }

    return TRUE;
}
Result disconnect() {

  Result result = SUCCESS;

  if (sendCmdAndWaitForResp(CLOSE_GPRS_CONTEXT, OK, 2000) == FALSE)
    result = ERROR_CLOSE_GPRS_CONTEXT;
  if (sendCmdAndWaitForResp(HTTP_CLOSE, OK, 2000) == FALSE)
    result = ERROR_HTTP_CLOSE;

  return result;
}
void connectNetwork() {
  String response = "";    
  mySerial.println("AT+SAPBR=2,1\r\n");
  delay(1000);
  mySerial.println("AT+SAPBR=1,1\r\n");
  delay(1000);
  mySerial.println("AT+HTTPINIT"); 
  long int time = millis();
  int i = 0;  
   
  while( (time+1000) > millis()){
    while(mySerial.available()){       
      char c = mySerial.read();
      response+=c;
    }  
  }
  Serial.print("Internet response: ");  
  Serial.print(response);
  internetState = response;
}
Result post(const char *uri, const char *body, char *response) {

  Result result = setHTTPSession(uri);

  char httpData[32];
  unsigned int delayToDownload = 10000;
  sprintf(httpData, HTTP_DATA, strlen(body), 10000);
  if (sendCmdAndWaitForResp(httpData, DOWNLOAD, 2000) == FALSE){
    result = ERROR_HTTP_DATA;
  }

  purgeSerial();
  delay(500);
  mySerial.println(body);

  if (sendCmdAndWaitForResp(HTTP_POST, HTTP_2XX, delayToDownload) == TRUE) {
    mySerial.println(HTTP_READ);
    readResponse(response);
    result = SUCCESS;
  }
  else {
    result = ERROR_HTTP_POST;
  }

  return result;
}
Result setHTTPSession(const char *uri){

  Result result;
  if (sendCmdAndWaitForResp(HTTP_CID, OK, 2000) == FALSE)
    result = ERROR_HTTP_CID;

  char httpPara[128];
  sprintf(httpPara, HTTP_PARA, uri);

  if (sendCmdAndWaitForResp(httpPara, OK, 2000) == FALSE)
    result = ERROR_HTTP_PARA;

  bool https = strncmp(HTTPS_PREFIX, uri, strlen(HTTPS_PREFIX)) == 0;
  if (sendCmdAndWaitForResp(https ? HTTPS_ENABLE : HTTPS_DISABLE, OK, 2000) == FALSE) {
    result = https ? ERROR_HTTPS_ENABLE : ERROR_HTTPS_DISABLE;
  }

  if (sendCmdAndWaitForResp(HTTP_CONTENT, OK, 2000) == FALSE)
    result = ERROR_HTTP_CONTENT;

  return result;
}

void readResponse(char *response){

  char buffer[128];
  cleanBuffer(buffer, sizeof(buffer));
  cleanBuffer(response, sizeof(response));

  if (readBuffer(buffer, sizeof(buffer),5000) == TRUE){
    parseJSONResponse(buffer, sizeof(buffer), response);
  }
}

int readBuffer(char *buffer, int count, unsigned int timeOut)
{
    int i = 0;
    unsigned long timerStart,timerEnd;
    timerStart = millis();
    while(1) {
        while (mySerial.available()) {
            char c = mySerial.read();
            buffer[i] = c;
            buffer[i + 1] = '\0';
            ++i;
            if(i > count-1)break;
        }
        if(i > count-1)break;
        timerEnd = millis();
        if(timerEnd - timerStart > timeOut) {
            break;
        }
    }
    delay(500);
    while(mySerial.available()) {
        mySerial.read();
    }
    return TRUE;
}

void cleanBuffer(char *buffer, int count)
{
    for(int i=0; i < count; i++) {
        buffer[i] = '\0';
    }
}

void purgeSerial()
{
  while (mySerial.available()) mySerial.read();
}
void parseJSONResponse(const char *buffer, unsigned int bufferSize, char *response){

  int start_index = 0;
  int i = 0;
  while (i < bufferSize - 1 && start_index == 0) {
    char c = buffer[i];
    if ('{' == c){
      start_index = i;
    }
    ++i;
  }

  int end_index = 0;
  int j = bufferSize - 1;
  while (j >= 0 && end_index == 0) {
    char c = buffer[j];
    if ('}' == c) {
      end_index = j;
    }
    --j;
  }

  for(int k = 0; k < (end_index - start_index) + 2; ++k){
    response[k] = buffer[start_index + k];
    response[k + 1] = '\0';
  }
}
