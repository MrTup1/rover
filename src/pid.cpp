#include <Arduino.h>
#include "PID.h"
#include "pins.h"
#include "encoders.h"
#include "motors.h"

//Calculating PWMSpeed target with soft start ramp
int ramp = 20; //RPM change per 50ms cycle
float rightRamped = 0.0;
float leftRamped = 0.0;

//Scales the initial power 'lump' to get close and adds minimum pwm for movement
float baseScale = 0.5;
float minPWM = 5.0;

//Individual motor trims so base scales to ~90% final speed @ 100RPM target
float trimFL = 1.09;
float trimFR = 1.03;
float trimBL = 1.10;
float trimBR = 1.08;

// PID variables to finesse the motor speeds 
float Kp = 1.0;
float Ki = 0.5;
float Kd = 0.0;

int integralLock = 20;

//Pid Internals
float errorFL = 0, errorFR = 0, errorBL = 0, errorBR = 0;
float prevErrorFL = 0, prevErrorFR = 0, prevErrorBL = 0, prevErrorBR = 0;
float integralFL = 0, integralFR = 0, integralBL = 0, integralBR = 0;
float dt = 0.00;

//calculated PID weightings
float dpwmFL = 0.0, dpwmFR = 0.0, dpwmBL = 0.0, dpwmBR = 0.0;

//Final PWM outputs
float FinalpwmFL = 0.0, FinalpwmFR = 0.0, FinalpwmBL = 0.0, FinalpwmBR = 0.0;

//Motor Stall Detection (Flag true = stalled)
int stallLimit = 10; //Sets how many cycles of underspeed before stall detection kicks in
float stallThreshold = 0.5; //Sets the stall detection threshold (0.5 = 50% underspeed)
int maxStallCount = 10; //Sets the numebr of cycles under stall threshold before flag kicks in

bool stallFlagFL = false, stallFlagFR = false, stallFlagBL = false, stallFlagBR = false;
int stallCountFL = 0, stallCountFR = 0, stallCountBL = 0, stallCountBR = 0;

void updatePID() {
  dt = elapsedTime/1000000.0; //time since last reading from encoder interrupt (micros to seconds)
  if(dt < 0.0001) dt = 0.0001;

  rampCalc();   //Calculates new ramped targets ahead of PID PWM calc
  stallCheck(); //Checks for motor stall and shuts down if required

  //calculate PWM weighting for each wheel
  errorFL = leftRamped - fl;
  integralFL += errorFL * dt;
  integralFL = constrain(integralFL, -integralLock, integralLock);
  float derivativeFL = (errorFL - prevErrorFL) / dt;
  dpwmFL = Kp*errorFL + Ki*integralFL + Kd*derivativeFL;
  prevErrorFL = errorFL;

  errorFR = rightRamped - fr;
  integralFR += errorFR * dt;
  integralFR = constrain(integralFR, -integralLock, integralLock);
  float derivativeFR = (errorFR - prevErrorFR) / dt;
  dpwmFR = Kp*errorFR + Ki*integralFR + Kd*derivativeFR;
  prevErrorFR = errorFR;

  errorBL = leftRamped - bl;
  integralBL += errorBL * dt;
  integralBL = constrain(integralBL, -integralLock, integralLock);
  float derivativeBL = (errorBL - prevErrorBL) / dt;
  dpwmBL = Kp*errorBL + Ki*integralBL + Kd*derivativeBL;
  prevErrorBL = errorBL;

  errorBR = rightRamped - br;
  integralBR += errorBR * dt;
  integralBR = constrain(integralBR, -integralLock, integralLock);
  float derivativeBR = (errorBR - prevErrorBR) / dt;
  dpwmBR = Kp*errorBR + Ki*integralBR + Kd*derivativeBR;
  prevErrorBR = errorBR;  

  //Add a linearly scaled base amount linked to target
  float leftBase = leftRamped * baseScale;
  float rightBase = rightRamped * baseScale;

  //Switch pid control signs and direction pins if going in reverse
  if(leftRamped > minPWM){
    digitalWrite(DIR_FL, LOW);
    digitalWrite(DIR_BL, LOW);
    FinalpwmFL = constrain((leftBase * trimFL) + dpwmFL, 0, 255);
    FinalpwmBL = constrain((leftBase * trimBL) + dpwmBL, 0, 255);
  }else if(leftRamped < -minPWM){
    digitalWrite(DIR_FL, HIGH);
    digitalWrite(DIR_BL, HIGH);
    FinalpwmFL = constrain((-leftBase * trimFL) - dpwmFL, 0, 255);
    FinalpwmBL = constrain((-leftBase * trimBL) - dpwmBL, 0, 255);
  }else{
    FinalpwmFL = 0;
    FinalpwmBL = 0;
    integralFL = integralBL = 0;
  }

  if(rightRamped > minPWM){
    digitalWrite(DIR_FR, LOW);
    digitalWrite(DIR_BR, LOW);
    FinalpwmFR = constrain((rightBase * trimFR) + dpwmFR, 0, 255);
    FinalpwmBR = constrain((rightBase * trimBR) + dpwmBR, 0, 255);
  }else if(rightRamped < -minPWM){
    digitalWrite(DIR_FR, HIGH);
    digitalWrite(DIR_BR, HIGH);
    FinalpwmFR = constrain((-rightBase * trimFR) - dpwmFR, 0, 255);
    FinalpwmBR = constrain((-rightBase * trimBR) - dpwmBR, 0, 255);
  }else{
    FinalpwmFR = 0;
    FinalpwmBR = 0;
    integralFR = integralBR = 0;
  }

  //Write PWM to motors (if no stall)
    if(stallFlagFL == false) analogWrite(PWM_FL, FinalpwmFL);
    else{ analogWrite(PWM_FL, 0); 
          integralFL = 0;
    }
    if(stallFlagFR == false) analogWrite(PWM_FR, FinalpwmFR);
    else{ analogWrite(PWM_FR, 0); 
          integralFR = 0;
    }
    if(stallFlagBL == false) analogWrite(PWM_BL, FinalpwmBL);
    else{ analogWrite(PWM_BL, 0); 
          integralBL = 0;
    }
    if(stallFlagBR == false) analogWrite(PWM_BR, FinalpwmBR);
    else{ analogWrite(PWM_BR, 0); 
          integralBR = 0;
    }

}



//Calculate new ramped values
void rampCalc(){
  
  float leftRampError = 0.0, rightRampError = 0.0; //difference between target speed and current ramp
  float LRR = 0.0, RRR = 0.0; //Adjusted ramp rates for each side, scaled to current errors

  if(abs(lefttargetSpeed - leftRamped) < 0.1 && abs(righttargetspeed - rightRamped) < 0.1) return; //return out if BOTH at target speeds already

  //Calculate current error magnitudes
  leftRampError = abs(lefttargetSpeed - leftRamped); 
  rightRampError = abs(righttargetspeed - rightRamped);

  //Scale the ramp rates accordingly, float maths and rounded up to avoiid crushing to 0.
  if(leftRampError >= rightRampError) LRR = ramp;
  else LRR = (ramp / rightRampError) * leftRampError; 
  if(rightRampError >= leftRampError) RRR = ramp;
  else RRR = (ramp / leftRampError) * rightRampError;

  //Calculate new left and right ramped targets (different maths when ramped bigger than target vs smaller than target. Ignored if same as target)
  if(lefttargetSpeed > leftRamped){
    leftRamped = min(lefttargetSpeed, leftRamped + LRR);
  }else{
    leftRamped = max(lefttargetSpeed, leftRamped - LRR);
  }

  if(righttargetspeed > rightRamped){
    rightRamped = min(righttargetspeed, rightRamped + RRR);
  }else{
    rightRamped = max(righttargetspeed, rightRamped - RRR);
  }

}

//Check for stalls - if <50% target speed add to counter, if counter reaches limit toggle flag.
void stallCheck(){

  if(abs(fl) < abs(leftRamped) * stallThreshold && abs(leftRamped) >= minPWM) stallCountFL++;
  else if(stallCountFL > 0) stallCountFL--;
  if(stallCountFL >= maxStallCount) stallFlagFL = true;

  if(abs(fr) < abs(rightRamped) * stallThreshold && abs(rightRamped) >= minPWM) stallCountFR++;
  else if(stallCountFR > 0) stallCountFR--;
  if(stallCountFR >= maxStallCount) stallFlagFR = true;

  if(abs(bl) < abs(leftRamped) * stallThreshold && abs(leftRamped) >= minPWM) stallCountBL++;
  else if(stallCountBL > 0) stallCountBL--;
  if(stallCountBL >= maxStallCount) stallFlagBL = true;

  if(abs(br) < abs(rightRamped) * stallThreshold && abs(rightRamped) >= minPWM) stallCountBR++;
  else if(stallCountBR > 0) stallCountBR--;
  if(stallCountBR >= maxStallCount) stallFlagBR = true;
}