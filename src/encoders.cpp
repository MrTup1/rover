#include <Arduino.h>
#include "encoders.h"
#include "pins.h"
#include "IMU.h" 
#include "PID.h"

// counts for calculation each motor speed
volatile long encFL = 0, encFR = 0, encBL = 0, encBR = 0;
long lastFL = 0, lastFR = 0, lastBL = 0, lastBR = 0;

float fl = 0, fr = 0, bl = 0, br = 0, leftside = 0, rightside = 0;
unsigned long lastSpeedTime = 0;

float distFL = 0, distFR = 0, distBL = 0, distBR = 0;
float leftSideDistance = 0, rightSideDistance = 0;
const float r = 76; // Size of big wheels
const float tick_to_distance =   2 * PI * r / 700; // Check motor datasheet page 3
long prevFL = 0, prevFR = 0, prevBL = 0, prevBR = 0; 
float globalX = 0.0;
float globalY = 0.0;

long timeNow = 0;
long elapsedTime = 0;

// ---------------------------------- interupts --------------------------------------

void IRAM_ATTR isrFL();
void IRAM_ATTR isrFR();
void IRAM_ATTR isrBL();
void IRAM_ATTR isrBR();

void IRAM_ATTR isrFL() {
  if (digitalRead(ENB_FL)) encFL++;
  else encFL--;
}

void IRAM_ATTR isrFR() {
  if (digitalRead(ENB_FR)) encFR++;
  else encFR--;
}

void IRAM_ATTR isrBL() {
  if (digitalRead(ENB_BL)) encBL++;
  else encBL--;
}

void IRAM_ATTR isrBR() {
  if (digitalRead(ENB_BR)) encBR++;
  else encBR--;
}

void encodersInit() {
  // encoders setup
  pinMode(ENA_FL, INPUT_PULLUP);
  pinMode(ENB_FL, INPUT_PULLUP);
  pinMode(ENA_FR, INPUT_PULLUP);
  pinMode(ENB_FR, INPUT_PULLUP);
  pinMode(ENA_BL, INPUT_PULLUP);
  pinMode(ENB_BL, INPUT_PULLUP);
  pinMode(ENA_BR, INPUT_PULLUP);
  pinMode(ENB_BR, INPUT_PULLUP);
  // interrupts setup
  attachInterrupt(digitalPinToInterrupt(ENA_FL), isrFL, RISING);
  attachInterrupt(digitalPinToInterrupt(ENA_FR), isrFR, RISING);
  attachInterrupt(digitalPinToInterrupt(ENA_BL), isrBL, RISING);
  attachInterrupt(digitalPinToInterrupt(ENA_BR), isrBR, RISING);
}

void updateSpeeds() {

  timeNow = micros();
  elapsedTime = timeNow - lastSpeedTime;

  if (elapsedTime < 50000) return;

  noInterrupts();
  long cFL = encFL, cFR = encFR, cBL = encBL, cBR = encBR;
  interrupts();

  float timeAdjustment = 50000.0 / (float)elapsedTime; //corrects for when elapsed time != 50ms

  fl = -(cFL - lastFL) * 1.7123 * timeAdjustment;
  fr =  (cFR - lastFR) * 1.7123 * timeAdjustment;
  bl = -(cBL - lastBL) * 1.7123 * timeAdjustment;
  br =  (cBR - lastBR) * 1.7123 * timeAdjustment;

  lastFL = cFL;
  lastFR = cFR;
  lastBL = cBL;
  lastBR = cBR;

  leftside = (fl + bl) / 2;
  rightside = (fr + br) / 2;

  lastSpeedTime = timeNow;

  updatePID();
}
void updateDistances() {

  noInterrupts();
  long totalFL = encFL;
  long totalFR = encFR;
  long totalBL = encBL;
  long totalBR = encBR;
  interrupts();
  
  distFL = -(totalFL * tick_to_distance);
  distFR = (totalFR * tick_to_distance);
  distBL = -(totalBL * tick_to_distance);
  distBR = (totalBR * tick_to_distance);

  leftSideDistance = (distFL + distBL) / 2.0; //Average distance of the 2 left wheels
  rightSideDistance = (distFR + distBR) / 2.0;

  //new version
  long deltaFL = totalFL - prevFL;
  long deltaFR = totalFR - prevFR;
  long deltaBL = totalBL - prevBL;
  long deltaBR = totalBR - prevBR;

  float dFL_mm = -(deltaFL * tick_to_distance);
  float dFR_mm = (deltaFR * tick_to_distance);
  float dBL_mm = -(deltaBL * tick_to_distance);
  float dBR_mm = (deltaBR * tick_to_distance);

  float dLeft = (dFL_mm + dBL_mm) / 2.0; //Average distance of the 2 left wheels
  float dright = (dFR_mm + dBR_mm) / 2.0;
  float deltacenter = (dLeft + dright) / 2.0;

  float relative_heading = heading - startHeading;
  float relative_heading_rad = relative_heading * (PI / 180.0); // Convert IMU degrees to rad

  globalX += deltacenter * cos(relative_heading_rad);
  globalY += deltacenter * sin(relative_heading_rad);

  prevFL = totalFL;
  prevFR = totalFR;
  prevBL = totalBL;
  prevBR = totalBR;

}

void resetEncoders() {
  noInterrupts();
  encFL = 0;
  encFR = 0;
  encBL = 0;
  encBR = 0;
  interrupts(); 

  // reset last variables for updateSpeed function
  lastFL = 0;
  lastFR = 0;
  lastBL = 0;
  lastBR = 0;

  // reset calculated distance for updateDistances function
  distFL = 0;
  distFR = 0;
  distBL = 0;
  distBR = 0;
  leftSideDistance = 0;
  rightSideDistance = 0;

  prevFL = 0;
  prevFR = 0;
  prevBL = 0;
  prevBR = 0;

  globalX = 0.0;
  globalY = 0.0;
  
  startHeading = heading; // <-- TAKES THE 0 DEGREE SNAPSHOT!
}