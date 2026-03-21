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
#include "obstacleAvoidance.h"

WebServer server(80);
int SPEED = 100;
int ROVER_LENGTH = 300;
unsigned long lastPrintTime = 0;
String receivedMessage = ""; 
int inputState = 0; // 0 = Wait for Distance Input, 1 = Wait for Angle Input, 2 = Automative state
float targetDistance = 0.0;
float targetAngle = 0.0;
float targetSpeed = 0;
float desiredHeading = 0.0;
float error = 9999;
float distance_error = 9999;
float turnAngle = 0;
float avoidTargetHeading = 0;
float previousSideDistance = 0;


bool moving = false;
bool autoMode = false;
bool destinationReached = false;
bool obstacleLocationLogged = false;

bool wallClose;
bool wallFar;
bool edgeDetected;    //Whether a corner of a box has been reached
float edgeDetectedAt; //Snapshot of distance when edge has been detected

//P controller heading variables
static float Kp = 0.6;
int baseSpeed = 50;

bool promptPrinted = false; // Ensures we only print the prompt once

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
  updateSpeeds();        // Updates encoder math
  updateDistances();     // Updates encoder distance
  updateIMU();           // Updates Pitch/Roll/Heading
  updateSensors();

  
  if (abs(pitch) > 45 || abs(roll) > 45) {
      Serial.println("DANGER: TILT DETECTED!");
      stop();
  }

  if (inputState == 2) { //target angle and distance given
    destinationReached = false;

    if (turnDegrees(desiredHeading)) {
      resetEncoders();
      inputState = 3;
    }
  }
  /*
  if (inputState == 3) {
      //Rotation finished, time to move forward to the goal
      if (check_Obstacle()) { //Temporary, prevent rover crashing into objects
        stop();
        inputState = 0; // Go all the way back to waiting for a new Serial input
      } else {
        distance_error = targetDistance - (leftSideDistance + rightSideDistance) / 2;
        
        if (distance_error > 100) {
          float headingError = desiredHeading - heading;

          //Check whether right of left turn is closer if overlap has occured
          if (headingError > 180) {
            headingError -= 360;
          } else if (headingError < -180) {
            headingError += 360;
          }
          
          //Implement P controller
          float P_term = Kp * headingError; //This is the adjustment if rover is slightly heading to the left or right
          int leftPWM = baseSpeed + P_term;
          int rightPWM = baseSpeed - P_term;
          forwardPID(leftPWM, rightPWM);

          //forward(100);
        } else if (distance_error < -100) {
          backward(100);
        } else {
          stop();
          destinationReached = true;
          inputState = 0; // Go all the way back to waiting for a new Serial input
        }
      }
  }*/

  if (inputState == 3) {
      //Rotation finished, time to move forward to the goal
      distance_error = sqrt((pow((targetDistance - globalX) , 2) + pow((globalY) , 2)));
      
      if (distance_error < 50) {
        stop();
        destinationReached = true;
        inputState = 0; // Go all the way back to waiting for a new Serial input
      }

      else if (frontDistance > 0 && frontDistance < 30) {
        stop();
        //Object detected directly in front of rover
        if (leftDistance > rightDistance) {
          turnAngle = -90.0f;
        } else if (rightDistance > leftDistance) {
          turnAngle = 90.0f;
        }

        avoidTargetHeading = heading + turnAngle;
        inputState = 4;
      }

      else if (distance_error > 100) {
        float headingError = desiredHeading - heading;

        //Check whether right of left turn is closer if overlap has occured
        if (headingError > 180) {
          headingError -= 360;
        } else if (headingError < -180) {
          headingError += 360;
        }
        
        //Implement P controller
        float P_term = Kp * headingError; //This is the adjustment if rover is slightly heading to the left or right
        int leftPWM = baseSpeed + P_term;
        int rightPWM = baseSpeed - P_term;
        forwardPID(leftPWM, rightPWM);
      }
  }
  
  //Turn 90 degrees away from obstacle

  if (inputState == 4) { 
    if (turnDegrees(avoidTargetHeading)) {
      edgeDetected = false;             //reset flag variables
      obstacleLocationLogged = false;
      stop();
      if (turnAngle == -90.0f) { //Ensure state 5 doesnt terminate immediately
          previousSideDistance = rightDistance;
      } else {
          previousSideDistance = leftDistance;
      } 
      inputState = 5;
    } 
  }

  // Go forward until path is clear

  if (inputState == 5) { 

    if (turnAngle == -90.0f) { //This means the right ultrasonic is closer to the obstacle
      if ((rightDistance > 0 && rightDistance < 50))
      
      if ((rightDistance - previousSideDistance) > 40) {
        edgeDetected = true;
      } else {
        forward(baseSpeed);
      }
      previousSideDistance = rightDistance;

    } else if (turnAngle == 90.0f) {
      if ((leftDistance - previousSideDistance) > 40) {
        edgeDetected = true;
      } else {
        forward(baseSpeed);
      }
      previousSideDistance = leftDistance;
    }

    if (edgeDetected == true && obstacleLocationLogged == false) {
      edgeDetectedAt = (leftSideDistance + rightSideDistance) / 2.0f;
      obstacleLocationLogged = true;
    }

    if (edgeDetected == true) {
      float distance_covered = (leftSideDistance + rightSideDistance) / 2.0f - edgeDetectedAt;
      if (distance_covered < ROVER_LENGTH) {
        forward(baseSpeed);
      } else {
        stop();
        inputState = 6;
      }
    }
  }

  // Reangle itself towards target

  if (inputState == 6) { 
    float rad = atan2(-globalY, (targetDistance - globalX)); //Get angle from odometry virtual plane
    desiredHeading = (rad * (180 / PI)) + startHeading;      //Convert odometry into real IMU data
    if (turnDegrees(desiredHeading)) {
      inputState = 3;
    };
  }
  

} 