#ifndef ENCODERS_H
#define ENCODERS_H

extern float fl, fr, bl, br;

extern float distFL, distFR, distBL, distBR;

extern float leftSideDistance, rightSideDistance;

void encodersInit();
void updateSpeeds();
void updateDistances();
void resetEncoders();

#endif