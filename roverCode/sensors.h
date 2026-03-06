#ifndef SENSORS_H
#define SENSORS_H

#include "TFluna.h"

extern float pitch;
extern volatile float frontDistance, leftDistance, rightDistance;
extern TFLuna tfLuna;


void sensorsInit();
void USdistance();
void readLidar();
void updateSensors();

#endif