#include <Arduino.h>
#include "motors.h"
#include "encoders.h"
#include "IMU.h"       
#include "pins.h"
#include <WiFi.h>
#include <WebServer.h>
#include "webPageCtrl.h"

WebServer server(80);
int SPEED = 100;
unsigned long lastPrintTime = 0;

void setup() {
  Serial.begin(115200);
  
  motorsInit();
  encodersInit();
  //IMU_init(); //This blocks anything until calibration is complete
    
  WiFi.softAP("ESP32-Group B rover", "Williscool");

  setupWebServer(server);
  server.begin();
}


void loop() {
  server.handleClient(); // Listens for Web Page buttons
  updateSpeeds();        // Updates encoder math
  //updateIMU();           // Updates Pitch/Roll/Heading

  if (abs(pitch) > 45 || abs(roll) > 45) {
      Serial.println("DANGER: TILT DETECTED!");
      stop();
  }
  
  if (millis() - lastPrintTime >= 500) {
    Serial.print("Pitch: "); 
    Serial.println(pitch);
    
    lastPrintTime = millis(); // Reset the stopwatch
  }
}