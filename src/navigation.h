#ifndef NAVIGATION_H
#define NAVIGATION_H

// Variables shared with the Web Server
extern int inputState;
extern float targetDistance;
extern float targetAngle;
extern float desiredHeading;
extern bool destinationReached;

// Main navigation loop
void runNavigationMode();

#endif