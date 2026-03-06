#include <Arduino.h>
#include "Sensors.h"
#include "Pins.h"
#include "PID.h"
#include "TFluna.h"

//IMU
float pitch = 69;

// distance measurments
volatile float frontDistance = 0.0;

// distance measurments
volatile float leftDistance = 0.0;
volatile float rightDistance = 0.0;

// ultrasonic stuff
bool activeside = true;
volatile uint32_t echoStart_R = 0;
volatile uint32_t echoEnd_R = 0;
volatile bool newReading_R = false;
volatile uint32_t echoStart_L = 0;
volatile uint32_t echoEnd_L = 0;
volatile bool newReading_L = false;



void IRAM_ATTR echoISR_R() {
  if (digitalRead(echoR) == HIGH) {
    echoStart_R = micros(); // start of pulse
  } else {
    echoEnd_R = micros();   // end of pulse
    newReading_R = true;    // flag for loop to process
  }
}

void IRAM_ATTR echoISR_L() {
  if (digitalRead(echoL) == HIGH) {
    echoStart_L = micros(); // start of pulse
  } else {
    echoEnd_L = micros();   // end of pulse
    newReading_L = true;    // flag for loop to process
  }
}

void sensorsInit() {
  pinMode(USR_TRIG, OUTPUT);
  pinMode(USL_TRIG, OUTPUT);
  pinMode(echoR, INPUT);
  pinMode(echoL, INPUT);
  attachInterrupt(digitalPinToInterrupt(echoR), echoISR_R, CHANGE);
  attachInterrupt(digitalPinToInterrupt(echoL), echoISR_L, CHANGE);
  
}

void USdistance() {
  if (activeside) { 
    digitalWrite(USR_TRIG, LOW);
    delayMicroseconds(2);
    digitalWrite(USR_TRIG, HIGH);
    delayMicroseconds(10);
    digitalWrite(USR_TRIG, LOW);
  } else if (!activeside) {
    digitalWrite(USL_TRIG, LOW);
    delayMicroseconds(2);
    digitalWrite(USL_TRIG, HIGH);
    delayMicroseconds(10);
    digitalWrite(USL_TRIG, LOW);
  }

  if (newReading_R) {
    newReading_R = false;
    uint32_t duration = echoEnd_R - echoStart_R;
    rightDistance = duration / 58.0;
  }
  if (newReading_L) {
    newReading_L = false;
    uint32_t duration = echoEnd_L - echoStart_L;
    leftDistance = duration / 58.0;
  }
  activeside = !activeside;

}


void updateSensors() {
  static unsigned long lastSensorUpdate = 0;

  if (millis() - lastSensorUpdate >= 100) {   // 100 ms = 0.1 s
    lastSensorUpdate = millis();
    USdistance();
    frontDistance = tfLuna.getDistance();
  }
}

