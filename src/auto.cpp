#include <Arduino.h>
#include "auto.h"
#include "motors.h"
#include "sensors.h"
#include "IMU.h"

int SPEED = 90;  

int minDistance = 40;
int minSideDistance = 50;
int stopSideDistance = 10;
int obsticleGoByeBye = 70;

int turnAngle = 25;
int turnTolerance = 5;


bool navigationMode = false; // dan use later

bool autoMode = false;
volatile bool obstruction = false;
volatile bool bumper = false;
float endDirection = 0;
int turnDirection = 0;
int heading = 0;
float prevError = 0;
long obstructionStopTime = 0;
long waitStart = 0;
long waitBack = 0;
bool turningFromObstruction = false;

bool waitRight = false;
bool waitLeft = false;


MotionState motionState = STOPPED;

Mode mode = FREEDRIVE;

//Bumper interrupt
void IRAM_ATTR bumperISR() {
  obstruction = true;
  bumper = true;
}

void bumperInit() {
  pinMode(3, INPUT);
  attachInterrupt(digitalPinToInterrupt(3), bumperISR, CHANGE);
}

//Automode loop
void runAutoMode() {
  if (mode != AUTO) return; 

  if (motionState == START_AUTO) {
    heading = Direction;
    forward(SPEED); //makes motionstate = FORWARD
  }

  if (motionState == STOPPED) {
    forward(SPEED); //makes motionstate = FORWARD
  }

  if (motionState == FORWARD || motionState == FORWARDRIGHT || motionState == FORWARDLEFT) {
    float errors = heading - Direction;
    while (errors > 180) errors -= 360;
    while (errors < -180) errors += 360;

    if (waitLeft && leftDistance > obsticleGoByeBye && (abs(errors) > 35)) {
      obstructionStopTime = millis();
      motionState = WAIT_TURN;
      return;
    }
    if (waitRight && rightDistance > obsticleGoByeBye && (abs(errors) > 35)) {
      obstructionStopTime = millis();
      motionState = WAIT_TURN;
      return;
    }

    if (leftDistance < minSideDistance) forwardrightturn(SPEED);
    else if (motionState == FORWARDRIGHT && leftDistance > minSideDistance) forward(SPEED);

    if (rightDistance < minSideDistance) forwardleftturn(SPEED);
    else if (motionState == FORWARDLEFT && rightDistance > minSideDistance) forward(SPEED);

    if (frontDistance < minDistance || leftDistance < stopSideDistance || rightDistance < stopSideDistance) obstruction = true; // test min check on all sides
    
    if (obstruction) { // if obsticle detected then stops
      stop();
      obstructionStopTime = millis();
      motionState = OBSTRUCTION;
    }
  }

  if (motionState == WAIT_TURN) {
    if (frontDistance < minDistance || leftDistance < stopSideDistance || rightDistance < stopSideDistance) obstruction = true; // test min check on all sides
    if (obstruction) { // if obsticle detected then stops
      stop();
      obstructionStopTime = millis();
      motionState = OBSTRUCTION;
      return;
    }

    if (millis() - obstructionStopTime > 600) { //waits 0.4 sec to clear obsticle
      stop();
      obstructionStopTime = millis();
      motionState = OBSTRUCTION;
      waitLeft = false;
      waitRight = false;
    }
  }

  if (motionState == WAIT_BACK) {
    if (millis() - waitBack > 200)
      stop();
      waitStart = millis();
      motionState == WAITING;
      bumper = false;
  }

  if (motionState == OBSTRUCTION) {
    if (millis() - obstructionStopTime > 1000) { //waits 1 sec before starting turning

      waitLeft = false;
      waitRight = false;

      float error = heading - Direction;
      while (error > 180) error -= 360;
      while (error < -180) error += 360;

      if (bumper) {
        backward(SPEED);
        waitBack = millis();
        motionState = WAIT_BACK;
        return;
      }

      if (abs(error) < 35) {
        if (rightDistance < leftDistance) {
          leftturn(SPEED);
          waitRight = true;
          motionState = TURNING_FREE; // turn till no obsticle
        }
        else {
          rightturn(SPEED);
          waitLeft = true;
          motionState = TURNING_FREE; // turn till no obsticle
        }
      }
      else if (abs(error) > 35) {
        if (error > 0) {
          endDirection = heading;
          if (endDirection >= 360) endDirection -= 360;
          rightturn(SPEED);
          motionState = TURNING_90;
        } else {
          endDirection = heading;
          if (endDirection < 0) endDirection += 360;
          leftturn(SPEED);
          motionState = TURNING_90; // turns until the pointing back at the heading
        }
      }
      else {
        forward(SPEED);
      }
      obstruction = false;
    }
  }

  if (motionState == TURNING_FREE) {
    if (frontDistance > minDistance + 15) {
      stop(); // stops if no longer obsticle in front
      waitStart = millis();
      motionState = WAITING;
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
    if (millis() - waitStart > 1000) { // waits one second then goes forward if no obstruction
      if (frontDistance < minDistance || leftDistance < stopSideDistance || rightDistance < stopSideDistance) {
        obstructionStopTime = millis() + 500; 
        motionState = OBSTRUCTION; 
      } else forward(SPEED);
    }
  }
}