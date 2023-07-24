#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char* ssid = "NETWORD_ID";
const char* password = "NETWORK_Password";

ESP8266WebServer server(80);

void handleJSONData() {
  if (server.method() == HTTP_POST) {
    String json = server.arg("plain"); // Get the JSON data from the request body
    // Parse and process the received JSON data here
    Serial.println(json);
    server.send(200, "application/json", "{\"status\":\"ok\"}");
  } else {
    server.send(400, "text/plain", "Invalid request");
  }
}

void setup() {
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
  server.handleClient();
}
