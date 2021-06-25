#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <ArduinoWebsockets.h>

const char* ssid = "*****";
const char* password = "*****";

const char* websockets_server_host = "*****";
const uint16_t websockets_server_port = 81;

using namespace websockets;
WebsocketsClient wsclient;

AsyncWebServer server(80);


#define triggerPinEntry 12
#define echoPinEntry 14
#define triggerPinExit 27
#define echoPinExit 26

long unsigned int durationEntry, durationExit, distanceEntry, distanceExit;
long unsigned int tempExit = 0;
long unsigned int tempEntry = 0;
bool highEntry = false;
bool highExit = false;
int count = 0;
int tempCount = -1;

void setup() {

  pinMode(triggerPinEntry, OUTPUT);
  pinMode(echoPinEntry, INPUT);
  pinMode(triggerPinExit, OUTPUT);
  pinMode(echoPinExit, INPUT);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
  }

  httpServer();
  server.begin();
}

void httpServer() {
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){

  StaticJsonDocument<768> doc;

  doc["@context"] = "https://www.w3.org/2019/wot/td/v1";
  doc["title"] = "ThingPeopleCounter";
  doc["description"] = "A Thing to control entry and exit of the room";
  doc["securityDefinitions"]["nosec_sc"]["scheme"] = "nosec";
  doc["security"][0] = "nosec_sc";

  JsonObject properties_numberOfPeople = doc["properties"].createNestedObject("numberOfPeople");
  properties_numberOfPeople["description"] = "Shows the current number of people in the room";
  properties_numberOfPeople["type"] = "string";
  properties_numberOfPeople["readOnly"] = true;

  JsonObject properties_numberOfPeople_forms_0 = properties_numberOfPeople["forms"].createNestedObject();
  properties_numberOfPeople_forms_0["op"] = "readproperty";
  properties_numberOfPeople_forms_0["href"] = "http://192.168.1.4/properties/numberofpeople/";
  properties_numberOfPeople_forms_0["contentType"] = "application/json";

  String tdhttp;
  serializeJson(doc, tdhttp);

  request->send(200, "application/json", tdhttp);
  });

  server.on("/properties/numberofpeople/", HTTP_GET, [](AsyncWebServerRequest *request){
    DynamicJsonDocument doc(1024);
    doc["sensor"][0] = "NumberOfPeople";
    doc["data"][0] = count;
    String wsdata;
    serializeJson(doc, wsdata);
  request->send(200, "application/json", wsdata);
  });

}

int sensorEntry(){
  digitalWrite(triggerPinEntry, LOW);
  delayMicroseconds(5);
  digitalWrite(triggerPinEntry, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPinEntry, LOW);
  durationEntry = pulseIn(echoPinEntry, HIGH);
  return (durationEntry/2) / 29.1;
}

int sensorExit(){
  digitalWrite(triggerPinExit, LOW);
  delayMicroseconds(5);
  digitalWrite(triggerPinExit, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPinExit, LOW);
  durationExit = pulseIn(echoPinExit, HIGH);
  return (durationExit/2) / 29.1;
}


void loop() {
  distanceEntry = sensorEntry();
  distanceExit = sensorExit();

  if(distanceEntry <= 50 && distanceEntry != 0) {
    highEntry = true;
    tempEntry = millis();
  }
  while(highEntry) {
    distanceExit = sensorExit();

    if(distanceExit <= 100) {
      count = count + 1;
      highEntry = false;
      tempEntry = 0;
      delay(1000);
    }
    if((millis() - tempEntry) > 2000) {
      highEntry = false;
      tempEntry = 0;
    }
  }

  if(distanceExit <= 70 && distanceExit != 0) {
    highExit = true;
    tempExit = millis();
  }
  while(highExit) {
    distanceEntry = sensorEntry();
    if(distanceEntry <= 50) {
      count = count - 1;
      if(count < 0) {
        count = 0;
      }
      highExit = false;
      tempExit = 0;
      delay(1000);
    }
    if((millis() - tempExit) > 2000) {
      highExit = false;
      tempExit = 0;
    }
  }
  if(count != tempCount) {
    bool connected = wsclient.connect(websockets_server_host, websockets_server_port, "/");
    if(connected) {
      DynamicJsonDocument doc(100);

      doc["sensor"][0] = "NumberOfPeople";
      doc["data"][0] = count;

      String wsdata;
      serializeJson(doc, wsdata);

      wsclient.send(wsdata);
      tempCount = count;

    } else {
      Serial.println("Not Connected!");
    }
  }
}
