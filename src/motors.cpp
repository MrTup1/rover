#include <Arduino.h>
#include "Motors.h"
#include "Pins.h"

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
  digitalWrite(DIR_FR, LOW);
  analogWrite(PWM_FR, speed);
  digitalWrite(DIR_BL, LOW);   
  analogWrite(PWM_BL, speed); 
  digitalWrite(DIR_BR, LOW);
  analogWrite(PWM_BR, speed);
  digitalWrite(DIR_FL, LOW);   
  analogWrite(PWM_FL, speed); //speed betwen 0min - 255max
}
void backward(int speed) { //left and right side backwards
  digitalWrite(DIR_FL, HIGH);   
  analogWrite(PWM_FL, speed); //speed betwen 0min - 255max
  digitalWrite(DIR_FR, HIGH);
  analogWrite(PWM_FR, speed);
  digitalWrite(DIR_BL, HIGH);   
  analogWrite(PWM_BL, speed); 
  digitalWrite(DIR_BR, HIGH);
  analogWrite(PWM_BR, speed);
}
void stop() { // both sides stopped
  analogWrite(PWM_FL, 0);
  analogWrite(PWM_FR, 0);
  analogWrite(PWM_BL, 0);
  analogWrite(PWM_BR, 0);
}
void rightturn(int speed) { //left forward and right backwards
  digitalWrite(DIR_FL, LOW);   
  analogWrite(PWM_FL, speed); //speed betwen 0min - 255max
  digitalWrite(DIR_FR, HIGH);
  analogWrite(PWM_FR, speed);
  digitalWrite(DIR_BL, LOW);   
  analogWrite(PWM_BL, speed); 
  digitalWrite(DIR_BR, HIGH);
  analogWrite(PWM_BR, speed);
}
void leftturn(int speed) { // left backwards and right forward
  digitalWrite(DIR_FL, HIGH);   
  analogWrite(PWM_FL, speed); //speed betwen 0min - 255max
  digitalWrite(DIR_FR, LOW);
  analogWrite(PWM_FR, speed);
  digitalWrite(DIR_BL, HIGH);   
  analogWrite(PWM_BL, speed); 
  digitalWrite(DIR_BR, LOW);
  analogWrite(PWM_BR, speed);
}

void updateleft(int speed) {
  digitalWrite(DIR_FL, HIGH);   
  analogWrite(PWM_FL, speed);
  digitalWrite(DIR_BL, HIGH);   
  analogWrite(PWM_BL, speed); 
}

void updateright(int speed) {
  digitalWrite(DIR_FR, HIGH);   
  analogWrite(PWM_FR, speed);
  digitalWrite(DIR_BR, HIGH);   
  analogWrite(PWM_BR, speed); 
}