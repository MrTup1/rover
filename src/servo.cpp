#include "servo.h"
#include "IMU.h"

Servo frontservo;  

int servoPin = 21; 
int pos = 137;  //approximately horizontal    

void servoInit() {
  ESP32PWM::allocateTimer(1);
  
  frontservo.setPeriodHertz(50);
  frontservo.attach(servoPin, 500, 2400);

  frontservo.write(pos); 
}

void setServo(){
  float Z = imuServoRead();
  int newpos = 135 - (int)Z;

  if(newpos > 180) newpos = 180;
  if(newpos < 100) newpos = 100;

  frontservo.write(newpos); 
}