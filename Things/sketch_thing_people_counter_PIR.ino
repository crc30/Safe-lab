#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <ArduinoWebsockets.h>

const char* ssid = "******";
const char* password = "******";

const char* websockets_server_host = "******";
const uint16_t websockets_server_port = 81;

using namespace websockets;

AsyncWebServer server(80);
WebsocketsClient wsclient;

int LEDOUT = 26;
int LEDIN = 16;
int PIROUT = 27;
int PIRIN = 4;

int count = 0;
int tempCount = -1;
int tempCount2 = -1;
long unsigned int tempOut = 0;
long unsigned int tempIn = 0;

void IRAM_ATTR detectsMovementIN() {
  digitalWrite(LEDIN, HIGH);
  tempIn = millis();
}
void IRAM_ATTR detectsMovementOUT() {
  digitalWrite(LEDOUT, HIGH);
  tempOut = millis();
}

void setup(){
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
  }
  Serial.println(WiFi.localIP());
  
  httpServer();
  server.begin();

  pinMode(LEDOUT, OUTPUT);  
  pinMode(LEDIN, OUTPUT);
  
  pinMode(PIROUT, INPUT_PULLUP);
  pinMode(PIRIN, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(PIROUT), detectsMovementOUT, RISING);
  attachInterrupt(digitalPinToInterrupt(PIRIN), detectsMovementIN, RISING);
  
}

void httpServer() {
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){    

  StaticJsonDocument<512> doc;

  doc["@context"] = "https://www.w3.org/2019/wot/td/v1";
  doc["title"] = "ThingPeopleCounter";
  doc["description"] = "A Thing to control entry and exit of the room";
  doc["securityDefinitions"]["nosec_sc"]["scheme"] = "nosec";
  doc["security"][0] = "nosec_sc";
  
  JsonObject events = doc.createNestedObject("events");
  
  JsonObject events_entryexit_data = events["entryexit"].createNestedObject("data");
  events_entryexit_data["type"] = "int";
  events_entryexit_data["readOnly"] = true;
  
  JsonObject events_forms_0 = events["forms"].createNestedObject();
  events_forms_0["href"] = "ws://192.168.1.2:81/";
  events_forms_0["contentType"] = "application/json";
  
  String tdhttp;
  serializeJson(doc, tdhttp);
   
  request->send(200, "application/json", tdhttp);
  });
}


void loop() {
  
  if(digitalRead(PIROUT) == LOW) { 
    digitalWrite(LEDOUT, LOW);      
  }
  if(digitalRead(PIRIN) == LOW) { 
    digitalWrite(LEDIN, LOW);  
  }
  
  if(tempOut > tempIn &&  tempOut != 0 && tempIn != 0) {
    count --;
    if(count < 0) {
      count = 0;
    }
    tempOut = tempIn = 0;
  }
  if(tempOut < tempIn && tempOut != 0 && tempIn != 0) {
    count ++;
    tempOut = tempIn = 0;
  }
  
  if(tempIn != 0 && millis() - tempIn >= 3000 && tempOut == 0) {
    tempIn = 0;  
  }
  if(tempOut != 0 && millis() - tempOut >= 3000 && tempIn == 0) {
    tempOut = 0;  
  }

  if(count != tempCount) {
    bool connected = wsclient.connect(websockets_server_host, websockets_server_port, "/");
    if(connected) {
      DynamicJsonDocument doc(100);
      
      doc["sensor"][0] = "PeopleCounter";
      doc["data"][0] = count;
      
      String wsdata;
      serializeJson(doc, wsdata);
      
      wsclient.send(wsdata);
      tempCount = count;

    } else {
      Serial.println("Not Connected!");
    }
    
  }
  delay(1000);
}
