#include <Arduino.h>
#include "sensors.h"
#include "pins.h"
#include "pid.h"
#include "TFluna.h"

// Distance measurements
volatile float frontDistance = 0.0;
volatile float leftDistance = -1.0;
volatile float rightDistance = -1.0;

// Ultrasonic timing
volatile uint32_t echoStart = 0;
volatile uint32_t echoEnd = 0;
volatile bool newReading = false;

// Control state
bool nextSideRight = false;      // false = left next, true = right next
bool waitingForEcho = false;
bool waitingSideRight = false;   // which side the current echo belongs to
uint32_t triggerTimeUs = 0;

constexpr unsigned long PING_INTERVAL_MS = 100;
constexpr uint32_t ECHO_TIMEOUT_US = 30000;   // ~30 ms timeout

void IRAM_ATTR echoISR() {
  if (digitalRead(echo) == HIGH) {
    echoStart = micros();
  } else {
    echoEnd = micros();
    newReading = true;
  }
}

static void triggerLeft() {
  digitalWrite(USL_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(USL_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(USL_TRIG, LOW);
}

static void triggerRight() {
  digitalWrite(USR_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(USR_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(USR_TRIG, LOW);
}

void sensorsInit() {
  pinMode(USR_TRIG, OUTPUT);
  pinMode(USL_TRIG, OUTPUT);
  pinMode(echo, INPUT);

  digitalWrite(USR_TRIG, LOW);
  digitalWrite(USL_TRIG, LOW);

  attachInterrupt(digitalPinToInterrupt(echo), echoISR, CHANGE);
}

void updateSensors() {
  static unsigned long lastPingMs = 0;

  // 1. Process finished echo
  if (newReading) {
    noInterrupts();
    uint32_t start = echoStart;
    uint32_t end = echoEnd;
    newReading = false;
    interrupts();

    uint32_t duration = end - start;
    float distance = duration / 58.0f;

    if (waitingForEcho) {
      if (waitingSideRight) {
        rightDistance = distance;
      } else {
        leftDistance = distance;
      }
      waitingForEcho = false;
    }
  }

  // 2. Timeout if no echo returned
  if (waitingForEcho && (micros() - triggerTimeUs > ECHO_TIMEOUT_US)) {
    if (waitingSideRight) {
      rightDistance = -1.0f;
    } else {
      leftDistance = -1.0f;
    }
    waitingForEcho = false;
  }

  // 3. Trigger next side every 100 ms, but only if not already waiting
  unsigned long nowMs = millis();
  if (!waitingForEcho && (nowMs - lastPingMs >= PING_INTERVAL_MS)) {
    lastPingMs = nowMs;

    if (nextSideRight) {
      triggerRight();
      waitingSideRight = true;
    } else {
      triggerLeft();
      waitingSideRight = false;
    }

    waitingForEcho = true;
    triggerTimeUs = micros();

    nextSideRight = !nextSideRight;
  }

  // 4. Update front sensor
  frontDistance = tfLuna.getDistance();
}