#ifndef PID_H
#define PID_H

extern bool moving;
extern float targetSpeed;
extern float lefttargetSpeed, righttargetspeed;
extern float trim;

void resetPID();
void updatePID();

void rampCalc();   //Calculates new ramped targets ahead of PID PWM calc
void stallCheck();

#endif