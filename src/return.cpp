#include <Arduino.h>
#include "return.h"
#include "auto.h"
#include "motors.h"

bool returnMode = false;
bool recording = false;

#define MAX_MOVES 100
Move moves[MAX_MOVES];
Move currentMove;
int moveCount = 0;

long startEncoder = 0;
float startDirection = 0;
float goalDirection = 0;
long goalEncoder = 0;
float prevERROR = 0;
long wait180Start = 0;
long waitingStart = 0;
bool executingMove = false;
int moveType = 0; // -1 for move tyoe = turn and 1 for drive
long currentEncoder = 0;
long encoderTolerance = 100;
bool turning;


MotionState prevMotionState = STOPPED;
MotionState currentMotion = STOPPED;

bool motionActive = false;

void record(){
  if (!recording) return;

  if (motionState != STOPPED && prevMotionState == STOPPED) { // if started a move record it
    currentMotion = motionState;
    motionActive = true;

    noInterrupts();
    long cFL = encFL, cFR = encFR, cBL = encBL, cBR = encBR;
    interrupts();

    if (motionState == FORWARD || motionState == BACKWARDS) {
      startEncoder = (cFL + cFR + cBL + cBR) / 4;   // average of encorder counts
    } else if (motionState == FORWARDLEFT || motionState == BACKWARDSLEFT) {
      startEncoder = (cFR + cBR) / 2;
    } else if (motionState == FORWARDRIGHT|| motionState == BACKWARDSRIGHT) {
      startEncoder = (cFL + cBL) / 2;
    }
    startDirection = Direction;               
  }

  if (motionState == STOPPED && motionActive && prevMotionState) {
    endMove();
  }
  prevMotionState = motionState;
}

void endMove() {
  Move m;

  noInterrupts();
  long cFL = encFL, cFR = encFR, cBL = encBL, cBR = encBR;
  interrupts();

  if (currentMotion == FORWARD) {
    long delta = ((cFL + cFR + cBL + cBR) / 4) - startEncoder;
    m.type = FORWARD;
    m.value = delta;   // encoder ticks 
  }

  if (currentMotion == BACKWARDS) {
    long delta = ((cFL + cFR + cBL + cBR) / 4) - startEncoder;
    m.type = BACKWARDS;
    m.value = delta;   // encoder ticks 
  }

  if (currentMotion == FORWARDLEFT) {
    long delta = ((cFR + cBR) / 2) - startEncoder;
    m.type = FORWARDLEFT;
    m.value = delta;   // encoder ticks 
  }

  if (currentMotion == FORWARDRIGHT) {
    long delta = ((cFL + cBL) / 2) - startEncoder;
    m.type = FORWARDRIGHT;
    m.value = delta;   // encoder ticks 
  }

  if (currentMotion == BACKWARDSLEFT) {
    long delta = ((cFR + cBR) / 2) - startEncoder;
    m.type = BACKWARDSLEFT;
    m.value = delta;   // encoder ticks 
  }

  if (currentMotion == BACKWARDSRIGHT) {
    long delta = ((cFL + cBL) / 2) - startEncoder;
    m.type = BACKWARDSRIGHT;
    m.value = delta;   // encoder ticks 
  }

  if (currentMotion == RIGHTTURN) {
    float delta = Direction - startDirection;

    // normalize angle
    while (delta > 180) delta -= 360;
    while (delta < -180) delta += 360;

    m.type = RIGHTTURN;
    m.value = delta;
  }

  if (currentMotion == LEFTTURN) {
    float delta = Direction - startDirection;

    while (delta > 180) delta -= 360;
    while (delta < -180) delta += 360;

    m.type = LEFTTURN;
    m.value = delta;
  }

  if (currentMotion == TURNING_90) {
    float delta = Direction - startDirection;

    while (delta > 180) delta -= 360;
    while (delta < -180) delta += 360;
    if (turnDirection == -1) m.type = LEFTTURN;
    else if (turnDirection == +1) m.type = RIGHTTURN;
    m.value = delta;
  }

  if (moveCount < MAX_MOVES) {
    moves[moveCount++] = m;
    moveCount++;
  }

  motionActive = false;
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
      leftturn(SPEED);
      motionState = TURNING_180;
      turning = true;
    }
    float error = goalDirection - Direction;
    while (error > 180) error -= 360; // range: -180 to +180
    while (error < -180) error += 360; 
    if (abs(error) < turnTolerance || (prevERROR * error < 0)) { // 5 degree error tolerance. or overshoot changes sign of error.
      stop();
      waitingStart = millis();
      // turning = false;
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
      goalDirection = Direction + currentMove.value; // finds goal direction
      if (goalDirection >= 360) { // mappes to 0 - 360
        goalDirection -= 360;
      }
      moveType = -1;
    } else if (currentMove.type == RIGHTTURN) {
      currentMove.type = LEFTTURN;
      goalDirection = Direction + currentMove.value; // if delta or current.value is negative then it will move left
      if (goalDirection < 0) {
        goalDirection += 360;
      }
      moveType = -1;
    }

    noInterrupts();
    long cFL = encFL, cFR = encFR, cBL = encBL, cBR = encBR;
    interrupts();

    if (currentMove.type == FORWARD || currentMove.type == BACKWARDS) {
      goalEncoder = ((cFL + cFR + cBL + cBR) / 4) + currentMove.value;   // average of encorder counts
      moveType = 1;
    } else if (currentMove.type == FORWARDLEFT || currentMove.type == BACKWARDSLEFT) {
      goalEncoder = ((cFR + cBR) / 2) + currentMove.value;
      moveType = 1;
    } else if (currentMove.type == FORWARDRIGHT|| currentMove.type == BACKWARDSRIGHT) {
      goalEncoder = ((cFL + cBL) / 2) + currentMove.value;
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
        executingMove = false;
        moveCount--;
      }
      prevERROR = error;


    } else if (moveType == 1) {
      noInterrupts();
      long cFL = encFL, cFR = encFR, cBL = encBL, cBR = encBR;
      interrupts();

      if (currentMove.type == FORWARD || currentMove.type == BACKWARDS) {
        currentEncoder = (cFL + cFR + cBL + cBR) / 4;   // average of encorder counts
      } else if (currentMove.type == FORWARDLEFT || currentMove.type == BACKWARDSLEFT) {
        currentEncoder = (cFR + cBR) / 2;
      } else if (currentMove.type == FORWARDRIGHT|| currentMove.type == BACKWARDSRIGHT) {
        currentEncoder = (cFL + cBL) / 2;
      }

      float error = goalEncoder - currentEncoder;
      if (abs(error) < encoderTolerance || (abs(prevERROR) > 10 && prevERROR * error < 0)) { 
        stop();
        executingMove = false;
        moveCount--;
      }
      prevERROR = error;
    }
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