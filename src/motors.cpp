#include <Arduino.h>
#include "motors.h"
#include "pins.h"
#include "PID.h"
#include "auto.h"

bool moving = false;
float targetSpeed = 0;
float lefttargetSpeed = 0;
float righttargetspeed = 0;

extern float heading;


void motorsInit() {
  pinMode(DIR_FL, OUTPUT);
  pinMode(PWM_FL, OUTPUT);
  pinMode(DIR_FR, OUTPUT);
  pinMode(PWM_FR, OUTPUT);
  pinMode(DIR_BL, OUTPUT);
  pinMode(PWM_BL, OUTPUT);
  pinMode(DIR_BR, OUTPUT);
  pinMode(PWM_BR, OUTPUT);
}


void forward(int speed) { //left and right side forward
  motionState = FORWARD;
  moving = true;
  targetSpeed = speed;
  lefttargetSpeed = speed;
  righttargetspeed = speed;
}

void backward(int speed) { //left and right side backwards
  motionState = BACKWARDS;
  moving = true;
  targetSpeed = -speed;
  lefttargetSpeed = -speed;
  righttargetspeed = -speed;
}

void stop() { // both sides stopped
  motionState = STOPPED;
  moving = false;
  targetSpeed = 0;
  lefttargetSpeed = 0;
  righttargetspeed = 0;
}

void rightturn(int speed) { //left forward and right backwards
  motionState = RIGHTTURN;
  moving = false;
  lefttargetSpeed = speed;
  righttargetspeed = -speed;
}

void leftturn(int speed) { // left backwards and right forward
  motionState = LEFTTURN;
  moving = false;
  lefttargetSpeed = -speed;
  righttargetspeed = speed;
}

void forwardleftturn(int speed) { // left backwards and right forward
  motionState = FORWARDLEFT;
  moving = true;
  targetSpeed = speed;
  lefttargetSpeed = speed / 2.0;
  righttargetspeed = (speed);
}
void forwardrightturn(int speed) { //left forward and right backwards
  motionState = FORWARDRIGHT;
  moving = true;
  targetSpeed = speed;
  lefttargetSpeed = speed;
  righttargetspeed = (speed) / 2.0;
}
void backwardsleftturn(int speed) { // left backwards and right forward
  motionState = BACKWARDSLEFT;
  moving = true;
  targetSpeed = -speed;
  lefttargetSpeed = -speed / 2.0;
  righttargetspeed = -speed;
}
void backwardsrightturn(int speed) { //left forward and right backwards
  motionState = BACKWARDSRIGHT;
  moving = true;
  targetSpeed = -speed;
  lefttargetSpeed = -speed;
  righttargetspeed = (-speed) / 2.0;
}

void steer(float leftSpeed, float rightSpeed) {
  moving = true;
  lefttargetSpeed = leftSpeed;
  righttargetspeed = rightSpeed;
}

//INPUT IS A TARGET HEADNG
bool turnDegrees(float targetHeading) { //RIGHT TURN IS POSITIVE, LEFT IS NEGATIVE, returns true when finish turning
    if (targetHeading >= 360.0) targetHeading -= 360.0;
    if (targetHeading < 0.0) targetHeading += 360.0;

    float error = targetHeading - heading; //constantly checking

    //Check whether right of left turn is closer if overlap has occured
    if (error > 180) {
      error -= 360;
    } else if (error < -180) {
      error += 360;
    }

    //Right turn is positive, left turn is negative
    if (error > 2.0) {
      rightturn(90);
      return false;
    } else if (error < -2.0) {
      leftturn(90);
      return false;
    } else {
      stop();
      return true;
    }
}
