#include <ArduinoWebsockets.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <InfluxDbClient.h>
#include <ESPAsyncWebServer.h>

const char* ssid = "******";
const char* password = "******";

#define INFLUXDB_URL "******"
#define INFLUXDB_TOKEN "******"
#define INFLUXDB_ORG "******"
#define INFLUXDB_BUCKET "******"
#define TZ_INFO "CET-1CEST,M3.5.0,M10.5.0/3"

InfluxDBClient client(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN);

AsyncWebServer server(80);

using namespace websockets;
WebsocketsServer wsserver;

void setup() {
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
  }

  wsserver.listen(81);

  httpServer();
  server.begin();
}

void checkMsg() {
  auto wsclient = wsserver.accept();
  if(wsclient.available()) {
    auto msg = wsclient.readBlocking();

    DynamicJsonDocument doc(1024);
    deserializeJson(doc, msg.data());

    for(int i = 0; i<doc["sensor"].size(); i++){
      Point sensor(doc["sensor"][i].as<String>());
      sensor.addField("value", doc["data"][i].as<float>());
      if (!client.writePoint(sensor)) {
        client.getLastErrorMessage();
      }
      sensor.clearFields();
    }
  }
}

void httpServer() {
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){   
  
  StaticJsonDocument<1024> doc;

  doc["@context"] = "https://www.w3.org/2019/wot/td/v1";
  doc["title"] = "StoreThingSensorData";
  doc["description"] = "A thing to write Time Series data to InfluxDB";
  doc["securityDefinitions"]["nosec_sc"]["scheme"] = "nosec";
  doc["security"][0] = "nosec_sc";
  
  JsonObject actions_storedata = doc["actions"].createNestedObject("storedata");
  actions_storedata["description"] = "Write Time Series data to InfluxDB";
  
  JsonObject actions_storedata_input = actions_storedata.createNestedObject("input");
  actions_storedata_input["type"] = "json object";
  actions_storedata_input["writeOnly"] = "True";
  
  JsonArray actions_storedata_input_format = actions_storedata_input.createNestedArray("format");
  actions_storedata_input_format.add("sensor");
  actions_storedata_input_format.add("data");
  
  JsonObject actions_storedata_input_sensor = actions_storedata_input.createNestedObject("sensor");
  actions_storedata_input_sensor["description"] = "Name sensor";
  actions_storedata_input_sensor["items"]["type"] = "string";
  
  JsonObject actions_storedata_input_data = actions_storedata_input.createNestedObject("data");
  actions_storedata_input_data["description"] = "Data of sensor";
  actions_storedata_input_data["items"]["type"] = "float";
  
  JsonObject actions_storedata_forms_0 = actions_storedata["forms"].createNestedObject();
  actions_storedata_forms_0["op"] = "invokeaction";
  actions_storedata_forms_0["href"] = "ws://192.168.1.11:81/";
  actions_storedata_forms_0["contentType"] = "application/json";
  
  String tdhttp;
  serializeJson(doc, tdhttp);
  
  request->send(200, "application/json", tdhttp);
  });

}

void loop() {
  checkMsg();
  delay(1000);
}
