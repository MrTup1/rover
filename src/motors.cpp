#include <Arduino.h>
#include "motors.h"
#include "pins.h"
#include "PID.h"
#include "auto.h"

bool moving = false;
float targetSpeed = 0;
float lefttargetSpeed = 0;
float righttargetspeed = 0;
float trim = 0.0;

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
  righttargetspeed = speed + trim;
}

void backward(int speed) { //left and right side backwards
  motionState = BACKWARDS;
  moving = true;
  targetSpeed = -speed;
  lefttargetSpeed = -speed;
  righttargetspeed = -speed - trim;
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
  righttargetspeed = (speed + trim);
}
void forwardrightturn(int speed) { //left forward and right backwards
  motionState = FORWARDRIGHT;
  moving = true;
  targetSpeed = speed;
  lefttargetSpeed = speed;
  righttargetspeed = (speed + trim) / 2.0;
}
void backwardsleftturn(int speed) { // left backwards and right forward
  motionState = BACKWARDSLEFT;
  moving = true;
  targetSpeed = -speed;
  lefttargetSpeed = -speed / 2.0;
  righttargetspeed = -speed - trim;
}
void backwardsrightturn(int speed) { //left forward and right backwards
  motionState = BACKWARDSRIGHT;
  moving = true;
  targetSpeed = -speed;
  lefttargetSpeed = -speed;
  righttargetspeed = (-speed - trim) / 2.0;
}

void steer(float leftSpeed, float rightSpeed) {
  moving = true;
  lefttargetSpeed = leftSpeed;
  righttargetspeed = rightSpeed;
}

//INPUT IS A TARGET HEADNG
bool turnDegrees(float targetHeading) { //RIGHT TURN IS POSITIVE, LEFT IS NEGATIVE, returns true when finish turning
    static float turnIntegral = 0;
    static unsigned long lastTurnTime = 0;
    
    if (targetHeading >= 360.0) targetHeading -= 360.0;
    if (targetHeading < 0.0) targetHeading += 360.0;

    
    //Check whether right of left turn is closer if overlap has occured
    if (error > 180) {
      error -= 360;
    } else if (error < -180) {
      error += 360;
    }
    
    float turnKp = 1.8;       
    float turnKi = 0.3;       
    int maxTurnSpeed = 90;    
    int minTurnSpeed = 55;
    
    float error = targetHeading - heading; 

    unsigned long now = millis();
    float dt = (now - lastTurnTime) / 1000.0; // convert to seconds
    if (dt > 0.1) dt = 0.05; 
        lastTurnTime = now;

    //Right turn is positive, left turn is negative
    if (abs(error) > 2.0) {
      turnIntegral += error * dt;
      turnIntegral = constrain(turnIntegral, -40.0, 40.0); 

      int calculatedSpeed = (error * turnKp) + (turnIntegral * turnKi);
      
      // Ensure we don't exceed max speed
      int finalTurnSpeed = constrain(abs(calculatedSpeed), minTurnSpeed, maxTurnSpeed);

      if (error > 0) {
        rightturn(finalTurnSpeed); 
      } else {
        leftturn(finalTurnSpeed);
      }
      return false;

    } else {
      stop();
      turnIntegral = 0; //reset integral memory for next turn
      return true;
    }
}
