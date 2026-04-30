#ifndef RETURN_H
#define RETURN_H

#include "auto.h"

extern volatile long encFL, encFR, encBL, encBR;
extern long cFL, cFR, cBL, cBR;

extern float Direction;
extern int turnTolerance;
extern int turnDirection;

extern long startTime;
extern bool recording;
extern float startDirection;

extern long currentEncoder;


void record();
void endMove(MotionState currentMotion);
void returnmode();

#endif