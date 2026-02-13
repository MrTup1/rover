#include "IMU.h"
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#include "pins.h"

// Create the sensor object with the correct Wire interface
Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28, &Wire);

// Define the global variables
float heading = 0;
float pitch = 0;
float roll = 0;
float accX = 0;
float accY = 0;
float accZ = 0;

void IMU_init() {
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW); // Turn LED LOW to show IMU uncalibrated
  Wire.begin(21, 17); //Pin 21 is SDA, Pin 17 is SCL
  Serial.begin(115200);
  Serial.println("BNO055 Orientation Sensor Test");

  /* Initialize the sensor */
  if (!bno.begin()) {
    Serial.print("No BNO055 detected ... Check your wiring or I2C ADDR!");
    while (1);
  }

  delay(1000);
  bno.setExtCrystalUse(true); //Use adafruit clock built into IMU

  /*IMU Calibration blocking loop
  Gyro: let IMU stay still for a few seconds. 
  Accelerometer: turn in 45 degree increments and stay at these angles for a few seconds*/
  
  uint8_t system, gyro, accel, mag;
  system = gyro = accel = mag = 0;
  
  while(gyro < 3 || accel < 1) {
    bno.getCalibration(&system, &gyro, &accel, &mag);
    Serial.print ("\nG="); Serial.print(gyro);
    Serial.print( "\nA="); Serial.print(accel);

    digitalWrite(LED_PIN, !digitalRead(LED_PIN)); //Blinking LED
    delay(500);
  }

  digitalWrite(LED_PIN, LOW);
  Serial.println("\n Calibration of BNO055 is complete!");
}

void updateIMU() {
    // Get Orientation Data (Euler Angles)
  sensors_event_t event;
  bno.getEvent(&event);

  heading = event.orientation.x;
  pitch = event.orientation.y;
  roll = event.orientation.z;

  imu::Vector<3> linearaccel = bno.getVector(Adafruit_BNO055::VECTOR_LINEARACCEL);
  accX = linearaccel.x();
  accY = linearaccel.y();
  accZ = linearaccel.z();
}