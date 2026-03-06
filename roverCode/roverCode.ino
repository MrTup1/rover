#include <WiFi.h>
#include <WebServer.h>

#include "Motors.h"
#include "Encoders.h"
#include "webPage.h"
#include "WebPageCtrl.h"
#include "PID.h"
#include "sensors.h"
#include "TFluna.h"


WebServer server(80);
int SPEED = 100;
bool moving = false;
float targetSpeed = 0;
bool autoMode = false;

#define lidarSCL 26
#define lidarSDA 27

TFLuna tfLuna(lidarSDA, lidarSCL);

void setup() {
  Serial.begin(115200);

  motorsInit();
  encodersInit();
  sensorsInit();
  tfLuna.begin();

  WiFi.softAP("ESP32-Group B rover", "Williscool");

  setupWebServer(server);
  server.begin();
}

void loop() {
  server.handleClient();
  updateSpeeds();
  updateSensors();
}
