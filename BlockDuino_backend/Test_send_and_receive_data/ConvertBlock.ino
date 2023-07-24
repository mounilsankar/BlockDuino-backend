#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char* ssid = "newveena";
const char* password = "98246192";

ESP8266WebServer server(80);

void parseJson(JsonObject o) {
    int id = o["id"];
    Serial.print(id);
    switch (id) {
        // id 1 corresponds to the on Start block
        case 1:
            {
                for (JsonObject child : o["contains"].as<JsonArray>()) {
                    parseJson(child);
                }
                break;
            }
        // id 2 corresponds to a forever block
        case 2:
            {
                for (JsonObject child : o["contains"].as<JsonArray>()) {
                    parseJson(child);
                }
                break;
            }
        // id 5 corresponds to a set pwm pin block
        case 5:
            {
                JsonObject arguments = o["args"][0];
                JsonObject pinBlock = arguments["pin_block"];
                JsonObject valueBlock = arguments["value_block"];
                int pin, value;
                if (pinBlock.isNull()) {
                    pin = arguments["pin"];
                } else {
                    pin = evaluateBlock(pinBlock);
                }
                if (valueBlock.isNull()) {
                    value = arguments["value"];
                } else {
                    value = evaluateBlock(valueBlock);
                }
                analogWrite(pin, value);
                pinMode(pin, OUTPUT);
                break;
            }
        // id 6 corresponds to set pin block
        case 6:
            {
                JsonObject arguments = o["args"][0];
                JsonObject pinBlock = arguments["pin_block"];
                JsonObject valueBlock = arguments["value_block"];
                int pin, value;
                if (pinBlock.isNull()) {
                    pin = arguments["pin"];
                } else {
                    pin = evaluateBlock(pinBlock);
                }
                if (valueBlock.isNull()) {
                    value = arguments["value"];
                } else {
                    value = evaluateBlock(valueBlock);
                }
                digitalWrite(pin, value);
                pinMode(pin, OUTPUT);
                break;
            }
    }
}

int evaluateBlock(JsonObject pinBlock) {
    const int id = pinBlock["id"];
    switch (id) {
        // id 3 corresponds to a state of pin block
        case 3:
            {
                JsonObject childBlock = pinBlock["child_block"];
                int pin;
                if (childBlock.isNull()) {
                    pin = pinBlock["pin"];
                } else {
                    pin = evaluateBlock(childBlock);
                }
                return digitalRead(pin);
            }
        // id 4 corresponds to a read analogue pin block
        case 4:
            {
                JsonObject childBlock = pinBlock["child_block"];
                int pin;
                if (childBlock.isNull()) {
                    pin = pinBlock["pin"];
                } else {
                    pin = evaluateBlock(childBlock);
                }
                return analogRead(pin);
            }
        default:
            return 0;
    }
}

void handleJSONData() {
  if (server.method() == HTTP_POST) {
    String json = server.arg("plain"); // Get the JSON data from the request body
    // Parse and process the received JSON data here
    Serial.println(json);
    StaticJsonDocument<3840> doc;
    DeserializationError error = deserializeJson(doc, json);
    if (error) { 
      Serial.print(F("deserializeJson() failed: "));
      server.send(400, "text/plain", "deserializeJson() failed: ");
      Serial.println(error.f_str());
    return;
    }
    server.send(200, "application/json", "{\"status\":\"ok\"}");
    parseJson(doc.as<JsonObject>());
  } else {
    server.send(400, "text/plain", "Invalid request");
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  server.on("/jsondata", handleJSONData);
  server.begin();
  Serial.println("HTTP server started");

}
void loop() {
  // put your main code here, to run repeatedly:
  // input is an array of dictionary where each dictionary represents a block
  // Example input [JsonObject a , JsonObject b, JsonObject c]
  /*
  Sample schema
{
  “id”: 1
  “contains”: [1,2,3,](array of json objects representing blocks)
}
char json[] = "{\"sensor\":\"gps\",\"time\":1351824120,\"data\":[48.756080,2.302038]}";
"{\"pin\": 3, \"value\": 1}"
{
“id” : 6
“args” : [
    { 
       “pin”: 3
        “Value”: “High”
    }
  ]
}

  */
 server.handleClient();
}