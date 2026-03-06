#include "tf_luna.h"

#define SDA_PIN 17
#define SCL_PIN 16

TFLuna tfLuna(SDA_PIN, SCL_PIN);

void setup() {

  Serial.begin(115200);

  tfLuna.begin();
}

void loop() {

  int distance = tfLuna.getDistance();

  if(distance != -1) {
      Serial.println(distance);
  }

  delay(50);
}
