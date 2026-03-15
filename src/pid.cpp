#include <Arduino.h>
#include "pid.h"
#include "pins.h"
#include "encoders.h"
#include "motors.h"

extern bool moving;
extern float targetSpeed;

// PID variables
float Kp = 0.8;
float Ki = 0.9;
float Kd = 0.02;

float errorFL = 0, errorFR = 0, errorBL = 0, errorBR = 0;
float prevErrorFL = 0, prevErrorFR = 0, prevErrorBL = 0, prevErrorBR = 0;
float integralFL = 0, integralFR = 0, integralBL = 0, integralBR = 0;
float dt = 0.1;

float dpwmFL = 0, dpwmFR = 0, dpwmBL = 0, dpwmBR = 0; // outputs after PID

void updatePID() {
  if (!moving) return; //only when moving forward or backwards

  //calculate PWM change for each
  errorFL = targetSpeed - fl;
  integralFL += errorFL * dt;
  float derivativeFL = (errorFL - prevErrorFL) / dt;
  dpwmFL = Kp*errorFL + Ki*integralFL + Kd*derivativeFL;
  prevErrorFL = errorFL;

  errorFR = targetSpeed - fr;
  integralFR += errorFR * dt;
  float derivativeFR = (errorFR - prevErrorFR) / dt;
  dpwmFR = Kp*errorFR + Ki*integralFR + Kd*derivativeFR;
  prevErrorFR = errorFR;

  errorBL = targetSpeed - bl;
  integralBL += errorBL * dt;
  float derivativeBL = (errorBL - prevErrorBL) / dt;
  dpwmBL = Kp*errorBL + Ki*integralBL + Kd*derivativeBL;
  prevErrorBL = errorBL;

  errorBR = targetSpeed - br;
  integralBR += errorBR * dt;
  float derivativeBR = (errorBR - prevErrorBR) / dt;
  dpwmBR = Kp*errorBR + Ki*integralBR + Kd*derivativeBR;
  prevErrorBR = errorBR;


  // Clamp PWM 0-255
  dpwmFL = constrain(dpwmFL, -255, 255);
  dpwmFR = constrain(dpwmFR, -255, 255);
  dpwmBL = constrain(dpwmBL, -255, 255);
  dpwmBR = constrain(dpwmBR, -255, 255);

  if (targetSpeed < 0) {
    analogWrite(PWM_FL, (-fl - dpwmFL));
    analogWrite(PWM_FR, (-fr - dpwmFR));
    analogWrite(PWM_BL, (-bl - dpwmBL));
    analogWrite(PWM_BR, (-br - dpwmBR));
  }

  analogWrite(PWM_FL, (fl + dpwmFL));
  analogWrite(PWM_FR, (fr + dpwmFR));
  analogWrite(PWM_BL, (bl + dpwmBL));
  analogWrite(PWM_BR, (br + dpwmBR));

}

void resetPID() {
  integralFL = integralFR = integralBL = integralBR = 0;
  prevErrorFL = prevErrorFR = prevErrorBL = prevErrorBR = 0;
  dpwmFL = dpwmFR = dpwmBL = dpwmBR = 0;
}

