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
extern float startHeading;

// Auto Mode variables
extern float startHeading;
extern float currentHeading;
extern float Direction;

// Functions
void IMU_init();
void updateIMU();
void updateHeading();
void updateAccel();
void setStartHeading(); 
float imuServoRead();   

#endif