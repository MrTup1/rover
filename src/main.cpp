#include <Arduino.h>
#include "motors.h"
#include "encoders.h"
#include "IMU.h"       
#include "pins.h"
#include <WiFi.h>
#include <WebServer.h>
#include "webPageCtrl.h"
#include <iostream>
#include "sensors.h"
#include "PID.h"
#include "auto.h"
#include "navigation.h"

WebServer server(80);
// CRASH CHECK
static unsigned long last_IMU_check = 0;
const float CRASH_THRESHOLD = 15.0; // TUNE THIS
unsigned long crashStart = 0; 


#define lidarSCL 26
#define lidarSDA 27
TFLuna tfLuna(lidarSDA, lidarSCL);

void setup() {
  Serial.begin(115200);

  delay(1000);
  motorsInit();
  encodersInit();
  IMU_init(); //This blocks anything until calibration is complete
  sensorsInit();
  tfLuna.begin();
    
  WiFi.softAP("ESP32-Group B rover", "Williscool");

  setupWebServer(server);
  server.begin();
}


void loop() {
  server.handleClient(); // Listens for Web Page buttons
  updateDistances();     // Updates encoder distance
  updateSensors();

  // --- SLOPE DETECTION --- //
  if (abs(pitch) > 45 || abs(roll) > 45) {
      Serial.println("DANGER: TILT DETECTED!");
      stop();
  }

  static uint32_t lastPID = 0;
  if (micros() - lastPID > 10000) {
    updateSpeeds(); 
    lastPID = micros();
  }

  static uint32_t lastSensors = 0;
  if (millis() - lastSensors > 100) {
    updateSensors();
    lastSensors = millis();
  }
  

  // --- CRASH DETECTION --- //

  if (millis() - last_IMU_check >= 10) { 
      last_IMU_check = millis();
      updateIMU();           // Updates Pitch/Roll/Heading

      float shockMagnitude = sqrt(accX * accX + 
                                  accY * accY + 
                                  accZ * accZ);

      if (shockMagnitude > CRASH_THRESHOLD) {
          if (crashStart == 0) {
            crashStart = millis();
          } else if (millis() - crashStart >= 1000) { //NEEDS TESTING
            stop();
            inputState = 0;   // Put it in manual mode
            mode = FREEDRIVE;
            crashStart = 0;   // Reset timer
          } else {
            crashStart = 0;   // Reset timer, above threshold not continous 
          }
      }
  }

  runAutoMode();
  runNavigationMode();

}  