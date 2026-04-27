#ifndef RETURN_H
#define RETURN_H

extern volatile long encFL, encFR, encBL, encBR;
extern float Direction;
extern int turnTolerance;
extern int turnDirection;

void record();
void endMove();
void returnmode();

#endif