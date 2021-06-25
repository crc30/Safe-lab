#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include "DHT.h"
#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <ArduinoWebsockets.h>

const char* ssid = "******";
const char* password = "******";

const char* websockets_server_host = "******";
const uint16_t websockets_server_port = 81;

using namespace websockets;
WebsocketsClient wsclient;

Adafruit_BMP085 bmp;

#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

AsyncWebServer server(80);

void setup(){

  dht.begin();
  bmp.begin();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
  }
    
  httpServer();
  server.begin();
}

String sensorData() {
  DynamicJsonDocument doc(1024);
        
  doc["sensor"][0] = "Temperature";
  doc["sensor"][1] = "Humidity";
  doc["sensor"][2] = "Pressure";
  doc["data"][0] = dht.readTemperature();
  doc["data"][1] = dht.readHumidity();
  doc["data"][2] = bmp.readPressure()*0.01;

  String wsdata;
  serializeJson(doc, wsdata);
  return wsdata;
}

void connenctWsServer() {
  bool connected = wsclient.connect(websockets_server_host, websockets_server_port, "/");
  if(connected) {
    String wsdata = sensorData();
    wsclient.send(wsdata);
  } else {
    Serial.println("Not Connected!");
  }
}

void httpServer() {
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){    
  
StaticJsonDocument<2048> doc;

doc["@context"] = "https://www.w3.org/2019/wot/td/v1";
doc["title"] = "ThingSensor";
doc["description"] = "A Thing to control the temperature, humidity and pressure of the room";
doc["securityDefinitions"]["nosec_sc"]["scheme"] = "nosec";
doc["security"][0] = "nosec_sc";

JsonObject events = doc.createNestedObject("events");

JsonObject events_temperatureValue = events.createNestedObject("temperatureValue");

JsonObject events_temperatureValue_data = events_temperatureValue.createNestedObject("data");
events_temperatureValue_data["type"] = "float";
events_temperatureValue_data["minimum"] = 0;
events_temperatureValue_data["maximum"] = 50;
events_temperatureValue_data["accuracy"] = "±2";
events_temperatureValue_data["unit"] = "celsius";

JsonObject events_temperatureValue_forms_0 = events_temperatureValue["forms"].createNestedObject();
events_temperatureValue_forms_0["href"] = "http://192.168.1.8/events/temperatureValue/";
events_temperatureValue_forms_0["op"] = "subscribeevent";
events_temperatureValue_forms_0["contentType"] = "application/json";
events_temperatureValue_forms_0["subprotocol"] = "longpoll";

JsonObject events_humidityValue = events.createNestedObject("humidityValue");

JsonObject events_humidityValue_data = events_humidityValue.createNestedObject("data");
events_humidityValue_data["type"] = "float";
events_humidityValue_data["minimum"] = 20;
events_humidityValue_data["maximum"] = 80;
events_humidityValue_data["accuracy"] = "5%";
events_humidityValue_data["unit"] = "percentage";

JsonObject events_humidityValue_forms_0 = events_humidityValue["forms"].createNestedObject();
events_humidityValue_forms_0["href"] = "http://192.168.1.8/events/humidityValue/";
events_humidityValue_forms_0["op"] = "subscribeevent";
events_humidityValue_forms_0["contentType"] = "application/json";
events_humidityValue_forms_0["subprotocol"] = "longpoll";

JsonObject events_pressureValue = events.createNestedObject("pressureValue");

JsonObject events_pressureValue_data = events_pressureValue.createNestedObject("data");
events_pressureValue_data["type"] = "float";
events_pressureValue_data["minimum"] = 300;
events_pressureValue_data["maximum"] = 1100;
events_pressureValue_data["accuracy"] = "±0.12";
events_pressureValue_data["unit"] = "hectopascals";

JsonObject events_pressureValue_forms_0 = events_pressureValue["forms"].createNestedObject();
events_pressureValue_forms_0["href"] = "http://192.168.1.8/events/pressureValue/";
events_pressureValue_forms_0["op"] = "subscribeevent";
events_pressureValue_forms_0["contentType"] = "application/json";
events_pressureValue_forms_0["subprotocol"] = "longpoll";

JsonObject events_allValueSensor = events.createNestedObject("allValueSensor");
events_allValueSensor["description"] = "Get all sensor temperature, humidity and pressure value";
events_allValueSensor["data"]["type"] = "string";

JsonObject events_allValueSensor_forms_0 = events_allValueSensor["forms"].createNestedObject();
events_allValueSensor_forms_0["href"] = "ws://192.168.1.8:81/";
events_allValueSensor_forms_0["op"] = "subscribeevent";
events_allValueSensor_forms_0["contentType"] = "application/json";
events_allValueSensor_forms_0["subprotocol"] = "websocket";

  
  String tdhttp;
  serializeJson(doc, tdhttp);
   
  request->send(200, "application/json", tdhttp);
  });
  server.on("/events/temperatureValue/", HTTP_GET, [](AsyncWebServerRequest *request){  
      DynamicJsonDocument doc(1024);
      doc["sensor"][0] = "Temperature";
      doc["data"][0] = dht.readTemperature();;
      String wsdata;
      serializeJson(doc, wsdata);
  request->send(200, "application/json", wsdata);
  });
  server.on("/events/humidityValue/", HTTP_GET, [](AsyncWebServerRequest *request){  
      DynamicJsonDocument doc(1024);
      doc["sensor"][0] = "Humidity";
      doc["data"][0] = dht.readHumidity();;
      String wsdata;
      serializeJson(doc, wsdata);
  request->send(200, "application/json", wsdata);
  });
  server.on("/events/pressureValue/", HTTP_GET, [](AsyncWebServerRequest *request){  
      DynamicJsonDocument doc(1024);
      doc["sensor"][0] = "Pressure";
      doc["data"][0] = bmp.readPressure()*0.01;;
      String wsdata;
      serializeJson(doc, wsdata);
  request->send(200, "application/json", wsdata);
  });
}

void loop() {
  connenctWsServer();
  delay(10000);
}