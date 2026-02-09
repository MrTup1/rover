#include <Arduino.h>
#include "Encoders.h"
#include "Pins.h"

// counts for calculation each motor speed
volatile long encFL = 0, encFR = 0, encBL = 0, encBR = 0;
long lastFL = 0, lastFR = 0, lastBL = 0, lastBR = 0;

float fl = 0, fr = 0, bl = 0, br = 0;
unsigned long lastSpeedTime = 0;

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
  if (millis() - lastSpeedTime < 100) return;

  noInterrupts();
  long cFL = encFL, cFR = encFR, cBL = encBL, cBR = encBR;
  interrupts();

  fl = -(cFL - lastFL) * 0.5728;
  fr =  (cFR - lastFR) * 0.5728;
  bl = -(cBL - lastBL) * 0.5728;
  br =  (cBR - lastBR) * 0.5728;

  lastFL = cFL;
  lastFR = cFR;
  lastBL = cBL;
  lastBR = cBR;

  lastSpeedTime = millis();
}