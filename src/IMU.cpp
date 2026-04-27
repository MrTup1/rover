#include "IMU.h"
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#include "pins.h"

// Create the sensor object with the correct Wire interface
Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28, &Wire1);

// Define the global variables
float heading = 0;
float pitch = 0;
float roll = 0;
float accX = 0;
float accY = 0;
float accZ = 0;

// Auto Mode Variables
float startHeading = 0;
float currentHeading = 0;
float Direction = 0;


void IMU_init() {
  Wire1.begin(IMU_SDA, IMU_SCL); //Pin 17 is SDA, Pin 16 is SCL
  Wire1.setClock(100000); // Forces standard 100kHz I2C speed
  Wire1.setTimeout(1000); // Tells ESP32 to wait longer before throwing Error 263
  

  Serial.println("BNO055 Orientation Sensor Test");

  /* Initialize the sensor */
  if (!bno.begin()) {
    Serial.print("No BNO055 detected ... Check your wiring or I2C ADDR!");
  } else {
    delay(1000);
    bno.setExtCrystalUse(true); //Use adafruit clock built into IMU
  }

  /*IMU Calibration blocking loop
  Gyro: let IMU stay still for a few seconds. 
  Accelerometer: turn in 45 degree increments and stay at these angles for a few seconds*/
  
  uint8_t system, gyro, accel, mag;
  system = gyro = accel = mag = 0;
  
  Serial.println("\n Calibration of BNO055 is complete!");
}

void setStartHeading() {
  sensors_event_t event;
  bno.getEvent(&event);
  startHeading = event.orientation.x;
}

float imuServoRead(){
  sensors_event_t event;
  bno.getEvent(&event);
  return (float)event.orientation.z;
}

void updateIMU() {
    // Get Orientation Data (Euler Angles)
  sensors_event_t event;
  bno.getEvent(&event);

  heading = event.orientation.x;
  pitch = event.orientation.y;
  roll = event.orientation.z;

  // Update auto mode variables (merged from old updatePosition)
  currentHeading = event.orientation.x;
  Direction = currentHeading - startHeading; // calculates relative position

  // Maps to 0 - 360 degrees
  if (Direction < 0) {
    Direction += 360;
  }
  if (Direction >= 360) {
    Direction -= 360;
  }

  imu::Vector<3> linearaccel = bno.getVector(Adafruit_BNO055::VECTOR_LINEARACCEL);
  accX = linearaccel.x();
  accY = linearaccel.y();
  accZ = linearaccel.z();
}