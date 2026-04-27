#ifndef ENCODERS_H
#define ENCODERS_H

extern float fl, fr, bl, br;
extern volatile long encFL, encFR, encBL, encBR;
extern float distFL, distFR, distBL, distBR;
extern long elapsedTime;

extern float leftSideDistance, rightSideDistance;
extern float globalX, globalY;
extern float startHeading;

void encodersInit();
void updateSpeeds();
void updateDistances();
void resetEncoders();

#endif