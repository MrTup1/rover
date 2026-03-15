#ifndef IMU_H
#define IMU_H

#include <Arduino.h>

// Global variables (extern means they are defined in the .cpp file)
extern float heading;
extern float pitch;
extern float roll;
extern float accX;
extern float accY;
extern float accZ;

// Functions
void IMU_init();
void updateIMU();

#endif