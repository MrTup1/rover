#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28);

void setup() {
  Serial.begin(115200);
  Serial.println("BNO055 Orientation Sensor Test");

  /* Initialize the sensor */
  if (!bno.begin()) {
    Serial.print("No BNO055 detected ... Check your wiring or I2C ADDR!");
    while (1);
  }

  delay(1000);
  bno.setExtCrystalUse(true); //Use adafruit clock built into IMU
}


void loop() {
  // Get Orientation Data (Euler Angles)
  sensors_event_t event;
  bno.getEvent(&event);

  Serial.print("Orientation X: ");
  Serial.print(event.orientation.x, 4); // Heading
  Serial.print(" Y: ");
  Serial.print(event.orientation.y, 4); // Pitch (most relevant for the tilt acceleration adjustment)
  Serial.print(" Z: ");
  Serial.println(event.orientation.z, 4); // Roll

  // Get Linear Acceleration (Acceleration without Gravity)
  imu::Vector<3> linearaccel = bno.getVector(Adafruit_BNO055::VECTOR_LINEARACCEL);

  Serial.print("Accel X: "); Serial.print(linearaccel.x());
  Serial.print(" Y: "); Serial.print(linearaccel.y());
  Serial.print(" Z: "); Serial.println(linearaccel.z());

  delay(100);
}