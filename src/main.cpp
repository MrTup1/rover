#include <Arduino.h>
#include "motors.h"
#include "encoders.h"
#include "IMU.h"       
#include "pins.h"

void setup() {
  Serial.begin(115200);
  
  motorsInit();
  encodersInit();
  IMU_init(); //This blocks anything until calibration is complete
}


void loop() {
  updateIMU();

  // Now you can use the variables directly!
  // Example: Stop if the rover tilts too much
  if (abs(pitch) > 45 || abs(roll) > 45) {
      Serial.println("DANGER: TILT DETECTED!");
      stop();
  }
  
  // Example: Print heading
  Serial.print("Pitch: "); Serial.println(pitch);

  delay(50);
}