#include <Arduino.h>
#include "motors.h"
#include "encoders.h"
#include "IMU.h"       
#include "pins.h"
#include <WiFi.h>
#include <WebServer.h>
#include "webPageCtrl.h"
#include <iostream>

WebServer server(80);
int SPEED = 100;
unsigned long lastPrintTime = 0;
String receivedMessage = ""; 
int inputState = 0; // 0 = Wait for Distance Input, 1 = Wait for Angle Input, 2 = Automative state
float targetDistance = 0.0;
float targetAngle = 0.0;
float desiredHeading = 0.0;
bool promptPrinted = false; // Ensures we only print the prompt once

void setup() {
  Serial.begin(115200);

  delay(1000);
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
  updateDistances();     // Updates encoder distance
  //updateIMU();           // Updates Pitch/Roll/Heading

  /*
  if (abs(pitch) > 45 || abs(roll) > 45) {
      Serial.println("DANGER: TILT DETECTED!");
      stop();
  }*/
  
  if (!promptPrinted) { //Ensure prompt is printed only once
    if (inputState == 0) {
      Serial.println("Enter target Distance (m): ");
    } else if (inputState == 1) {
      Serial.println("Enter target ANGLE (degrees)");
    }
    promptPrinted = true;
  }


  while (Serial.available()) {
      char incomingChar = Serial.read();  // Read each character from the buffer
      
      if (incomingChar == '\n') {  // Check if the user pressed Enter (new line character)
        if (receivedMessage.length() > 0) { //Ignore input if empty
          if (inputState == 0) {
            targetDistance = receivedMessage.toFloat();
            Serial.print("Distance set to: ");
            Serial.println(targetDistance);

            inputState = 1;
            promptPrinted = false; //Allow the 2nd prompt for angle to be printed

          } else if (inputState == 1) {
            targetAngle = receivedMessage.toFloat();
            Serial.print("Relative Angle set to: ");
            Serial.println(targetAngle);
            desiredHeading = targetAngle + heading; //translate into angle relative to IMU start

            inputState = 2;
            promptPrinted = false; 
          }

          receivedMessage = ""; //Clear buffer for next input
        } 
        
      } else {
        receivedMessage += incomingChar;
      }
    }

  if (inputState == 2) { //target angle and distance given
    if (desiredHeading >= 360.0) desiredHeading -= 360.0;
    if (desiredHeading < 0.0) desiredHeading += 360.0;

    float error = desiredHeading - heading; //constantly checking

    //Check whether right of left turn is closer if overlap has occured
    if (error > 180) {
      error -= 360;
    } else if (error < -180) {
      error += 360;
    }

    //Right turn is positive, left turn is negative
    if (error > 2.0) {
      rightturn(100);
    } else if (error < 2.0) {
      leftturn(100);
    } else {
      stop();
    }
  }
}