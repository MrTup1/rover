#include <Arduino.h>
#include "return.h"
#include "auto.h"
#include "motors.h"
#include "encoders.h"

bool returnMode = false;
bool recording = false;

#define MAX_MOVES 100
Move moves[MAX_MOVES];
Move currentMove;
int moveCount = 0;

long startEncoder = 0;
float startDirection = 0;
float goalDirection = 0;
long goalTime = 0;
float prevERROR = 0;
long wait180Start = 0;
long waitingStart = 0;
long waitCheck = 0;
bool executingMove = false;
int moveType = 0; // -1 for move tyoe = turn and 1 for drive
long currentEncoder = 0;
long encoderTolerance = 100;
bool turning = false;

long startTime = 0;

bool motionActive = false;

void record(){
  if (!recording) return; // checked in actual motion functions
}

void endMove(MotionState currentMotion) {
  Move m;
  m.type = currentMotion;
  if (currentMotion == LEFTTURN || currentMotion == RIGHTTURN) {
    m.value = Direction - startDirection;
    while (m.value > 180) m.value -= 360;
    while (m.value < -180) m.value += 360;
  } else {
    m.value = millis() - startTime;
  }
  moves[moveCount++] = m;
}


void returnmode() {
  if (mode != RETURN) return;

  if (motionState == STARTRETURN) {

    goalDirection = Direction - 180;
    if (goalDirection < 0) {
      goalDirection += 360;
    }
    motionState = TURNING_180;
    prevERROR = 0;
   wait180Start = millis();
  }

  if (motionState == TURNING_180) {
    if (millis() - wait180Start > 1500 && !(turning)) {
      rightturn(50);
      motionState = TURNING_180;
      turning = true;
    }
    float error = goalDirection - Direction;
    while (error > 180) error -= 360; // range: -180 to +180
    while (error < -180) error += 360; 
    if (abs(error) < turnTolerance || (prevERROR * error < 0)) { // 5 degree error tolerance. or overshoot changes sign of error.
      stop();
      motionState = CHECK_TURN;
      waitCheck = millis();
      // turning = false;
    }
    prevERROR = error;
  }

  if (motionState == CHECK_TURN) {
    if (millis() - waitCheck < 200 ) return; 
    float error = goalDirection - Direction;
    if (executingMove) {
      if (abs(error) > 1) {
        if (error < 0) {
          leftturn(50);
          prevERROR = 0;
          motionState = TURNING_HEADING;
          return;
        } else {
          rightturn(50);
          prevERROR = 0;
          motionState = TURNING_HEADING;
          return;
        }
      } else {
        motionState = WAITING;
        waitingStart = millis();
        executingMove = false;
        moveCount--;
        return;
      }
    } 
    else if (abs(error) > 1) {
      if (error < 0) {
        leftturn(50);
        prevERROR = 0;
        motionState = TURNING_HEADING;
        return;
      } else {
        rightturn(50);
        prevERROR = 0;
        motionState = TURNING_HEADING;
        return;
      }
    } 
    else {
      waitingStart = millis();
      motionState = WAITING;
    }
  }

    if (motionState == TURNING_HEADING) {
    float error = goalDirection - Direction;
    while (error > 180) error -= 360; // range: -180 to +180
    while (error < -180) error += 360; 
    if (abs(error) < turnTolerance || (prevERROR * error < 0)) { // 5 degree error tolerance. or overshoot changes sign of error.
      stop();
      motionState = CHECK_TURN;
    }
    prevERROR = error;
  }

  if (motionState == STOPPED && !executingMove) {
    // finished all moves
    if (moveCount <= 0) {
      stop();
      moveCount = 0;
      return;
    }

    // start next move
    currentMove = moves[moveCount - 1];

    // invert move
    if (currentMove.type == LEFTTURN) {
      currentMove.type = RIGHTTURN;
      goalDirection = Direction - currentMove.value; // finds goal direction
      if (goalDirection >= 360) { // mappes to 0 - 360
        goalDirection -= 360;
      }
      moveType = -1;
    } else if (currentMove.type == RIGHTTURN) {
      currentMove.type = LEFTTURN;
      goalDirection = Direction - currentMove.value; // if delta or current.value is negative then it will move left
      if (goalDirection < 0) {
        goalDirection += 360;
      }
      moveType = -1;
    } else {
      goalTime = millis() + currentMove.value;  
      // goalEncoder = currentEncoder + 1000// currentEncoder + currentMove.value
      moveType = 1;
    }

    if (currentMove.type == FORWARD) forward(SPEED);
    else if (currentMove.type == BACKWARDS) backward(SPEED);
    else if (currentMove.type == RIGHTTURN) rightturn(SPEED);
    else if (currentMove.type == LEFTTURN) leftturn(SPEED);
    else if (currentMove.type == FORWARDRIGHT) forwardrightturn(SPEED);
    else if (currentMove.type == FORWARDLEFT) forwardleftturn(SPEED);
    else if (currentMove.type == BACKWARDSLEFT) backwardsleftturn(SPEED);
    else if (currentMove.type == BACKWARDSRIGHT) backwardsrightturn(SPEED);

    executingMove = true;
    prevERROR = 0;
  }

  if (executingMove) {
    if (moveType == -1) { // turn checks
      float error = goalDirection - Direction;
      while (error > 180) error -= 360; // range: -180 to +180
      while (error < -180) error += 360; 
      if (abs(error) < turnTolerance || (prevERROR * error < 0)) { // 5 degree error tolerance. or overshoot changes sign of error.
        stop();
        motionState = CHECK_TURN;
      }
      prevERROR = error;


    } else if (moveType == 1) {
      if (millis() > goalTime) {
        stop();
        executingMove = false;
        moveCount--;
      }
    }

      // float error = goalEncoder - currentEncoder;
      // if ((prevERROR * error < 0) || abs(error) < encoderTolerance) {
      //   stop();
      //   executingMove = false;
      //   moveCount--;
      // }
      // prevERROR = error;


    if (!executingMove) {
      motionState = WAITING;
      waitingStart = millis();
    }
  }

  if (motionState == WAITING) {
    if (millis() - waitingStart > 1000) {
      motionState = STOPPED;
    }
  }
}