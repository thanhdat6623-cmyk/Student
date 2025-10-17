#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Arduino_JSON.h>
#include "SPIFFS.h"
// Replace with your network credentials
const char* ssid = "y"; 
const char* password = "111111115";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// Create an Event Source on /events
AsyncEventSource events("/events");

// Json Variable to Hold Sensor Readings
JSONVar readings;

// Timer variables
unsigned long lastTime = 0;  
unsigned long lastTime1 = 0;
unsigned long lastTime2 = 0;
unsigned long phDelay = 100;
unsigned long ddDelay = 100;
unsigned long wtDelay = 100;
//chan ket noi 
int pHPin = 32;
int ddpin = 34;
int watterpin = 35; 
void initSPIFFS() {
  if (!SPIFFS.begin()) {
    Serial.println("An error has occurred while mounting SPIFFS");
  }
  Serial.println("SPIFFS mounted successfully");
}
// Initialize WiFi
void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");
  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("");
  Serial.println(WiFi.localIP());
}

String getphsensor(){
  int buf[10];                //buffer for read analog
  for(int i=0;i<10;i++)       //Get 10 sample value from the sensor for smooth the value
  { 
    buf[i]=analogRead(35);
    delay(10);
  }
  for(int i=0;i<9;i++)        //sort the analog from small to large
  {
    for(int j=i+1;j<10;j++)
    {
      if(buf[i]>buf[j])
      {
        int temp=buf[i];
        buf[i]=buf[j];
        buf[j]=temp;
      }
    }
  }
  int avgValue=0;
  for(int i=2;i<8;i++) {                     //take the average value of 6 center sample
    avgValue+=buf[i];}
  // ánh xạ
  int pHValue = map(avgValue/6 , 0, 4095, 0, 14); // Giả sử cảm biến pH có dải đo từ 0 đến 14
  readings["pHValue"] = String(pHValue);
  String jsonString = JSON.stringify(readings);
  return jsonString;
}

String getdd() {
   int buf0[10];                //buffer for read analog
  for(int i=0;i<10;i++)       //Get 10 sample value from the sensor for smooth the value
  { 
     buf0[i] = analogRead(ddpin);
    delay(10);
  }
  for(int i=0;i<9;i++)        //sort the analog from small to large
  {
    for(int j=i+1;j<10;j++)
    {
      if(buf0[i]>buf0[j])
      {
        int temp0=buf0[i];
        buf0[i]=buf0[j];
        buf0[j]=temp0;
      }
    }
  }
  int avgValue0=0;
  for(int i=2;i<8;i++) {                     //take the average value of 6 center sample
    avgValue0+=buf0[i];}
  // ánh xạ
  int dd = map(avgValue0/6 , 0 , 4095, 0 ,1000)/100;
  readings["dd"] = String(dd);
  String ddString = JSON.stringify (readings);
  return ddString;
}

String getwt(){
  int b = analogRead(35);
  int a = map(b , 0 , 2050, 0 ,100);
  return String(a);
}
void setup() {
  Serial.begin(115200);
  initWiFi();
  initSPIFFS();
  SPIFFS.begin();

  // Handle Web Server
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", "text/html");
  });

  server.serveStatic("/", SPIFFS, "/");


  // Handle Web Server Events
  events.onConnect([](AsyncEventSourceClient *client){
    if(client->lastId()){
      Serial.printf("Client reconnected! Last message ID that it got is: %u\n", client->lastId());
    }
    // send event with message "hello!", id current millis
    // and set reconnect delay to 1 second
    client->send("hello!", NULL, millis(), 10000);
  });
  server.addHandler(&events);

  server.begin();
}


void loop() {
  if ((millis() - lastTime) > phDelay) {
    // Send Events to the Web Server with the Sensor Readings
    events.send(getphsensor().c_str(),"1_readings",millis());
    lastTime = millis();
  }
  if ((millis() - lastTime1) > ddDelay) {
    // Send Events to the Web Server with the Sensor Readings
    events.send(getdd().c_str(),"2_readings",millis());
    lastTime1 = millis();
  }
  if ((millis() - lastTime2) > wtDelay) {
    // Send Events to the Web Server with the Sensor Readings
    events.send(getwt().c_str(),"3_reading",millis());
    lastTime2 = millis();
  }
}
