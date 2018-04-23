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
   connectNetwork();
   sendTabData("AT+CGNSINF",5000,DEBUG);    //send demand of gps localization & GPRS connection (no funciona todavía)
   if(state != 0 && internetState != 0){
    /*we just dont want to print empty signs so we wait until 
     * gps module will have connection to satelite.
    
     * when whole table returned:
   for(int i = 0; i < (sizeof(latlongtab)/sizeof(int)); i++){
      Serial.println(latlongtab[i]); // print 
    }*/
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

void connectNetwork() {
  String response = "";    
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

