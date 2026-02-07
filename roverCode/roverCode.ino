#include <WiFi.h>
#include <WebServer.h>

#include "Motors.h"
#include "Encoders.h"
#include "WebPageCtrl.h"

WebServer server(80);
int SPEED = 100;

void setup() {
  Serial.begin(115200);

  motorsInit();
  encodersInit();

  WiFi.softAP("ESP32-Group B rover", "Williscool");

  setupWebServer(server);
  server.begin();
}

void loop() {
  server.handleClient();
  updateSpeeds();
}
