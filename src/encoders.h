#ifndef ENCODERS_H
#define ENCODERS_H

extern float fl, fr, bl, br;

extern float distFL, distFR, distBL, distBR;

extern float leftSideDistance, rightSideDistance;
extern float globalX, globalY;
extern float startHeading;

void encodersInit();
void updateSpeeds();
void updateDistances();
void resetEncoders();

#endif