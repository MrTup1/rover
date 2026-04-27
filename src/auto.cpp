#include <Arduino.h>
#include "auto.h"
#include "motors.h"
#include "sensors.h"
#include "IMU.h"

int SPEED = 90;
int minDistance = 60;
int minSideDistance = 60;
int turnAngle = 115;
int turnTolerance = 10;


bool navigationMode = false; // dan use later

bool autoMode = false;
volatile bool obstruction = false;
float endDirection = 0;
int turnDirection = 0;
float prevError = 0;
long obstructionStopTime = 0;
long waitStart = 0;

MotionState motionState = STOPPED;

Mode mode = FREEDRIVE;

//Bumper interrupt
void IRAM_ATTR bumperISR() {
  obstruction = true;
}

void bumperInit() {
  pinMode(3, INPUT);
  attachInterrupt(digitalPinToInterrupt(3), bumperISR, CHANGE);
}

//Automode loop
void runAutoMode() {
  SPEED = 90;
  if (mode != AUTO) return; 

  if (motionState == STOPPED) {
    forward(SPEED); //makes motionstate = FORWARD
  }

  if (motionState == FORWARD || motionState == FORWARDRIGHT || motionState == FORWARDLEFT) {
    if (leftDistance < minSideDistance) forwardrightturn(SPEED);
    if (rightDistance < minSideDistance) forwardleftturn(SPEED);
    // if (frontDistance < minDistance) obstruction = true; //checks obsticle in front
    if (frontDistance < minDistance || leftDistance < (minDistance / 4) || rightDistance < (minDistance / 4)) obstruction = true; // test min check on all sides
    if (obstruction) { // if obsticle detected then stops
      stop();
      obstructionStopTime = millis();
      motionState = OBSTRUCTION;
    }

  }

  if (motionState == OBSTRUCTION) {
    if (millis() - obstructionStopTime > 1500) { //waits 1 sec before starting turning
      int startDirection = Direction; //gets the current direction (for turn later)
      if (leftDistance < rightDistance) { 
        turnDirection = 1;//turn right 
        endDirection = Direction + turnAngle; // finds goal direction
        if (endDirection >= 360) { // mappes to 0 - 360
          endDirection -= 360;
        }
        rightturn(SPEED);
      }
      else {
        turnDirection = -1; //left turn
        endDirection = Direction - turnAngle; // finds goal direction
        if (endDirection < 0) {
          endDirection += 360;
        }
        leftturn(SPEED);
      }
      obstruction = false;
      motionState = TURNING_90;
      prevError = 0;
    }
  }

  if (motionState == TURNING_90) {
    float error = endDirection - Direction;
    while (error > 180) error -= 360; // range: -180 to +180
    while (error < -180) error += 360; 
    if (abs(error) < turnTolerance || (prevError * error < 0)) { // 5 degree error tolerance. or overshoot changes sign of error.
      stop();
      waitStart = millis();
      motionState = WAITING;
    }
    prevError = error;
  }

  if (motionState == WAITING) {
    if (millis() - waitStart > 1500) { // waits one second then goes forward if no obstruction
      if (frontDistance < minDistance) {
        obstructionStopTime = millis() + 500;
        motionState = OBSTRUCTION;
      }
      else motionState = STOPPED;
      
    }
  }
}
