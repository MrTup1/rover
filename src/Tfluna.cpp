#include "TFluna.h"


TFLuna::TFLuna(uint8_t sda, uint8_t scl) {
    _sda = sda;
    _scl = scl;
}

void TFLuna::begin() {
    Wire.begin(_sda, _scl);
}

int TFLuna::getDistance() {

    // Trigger measurement
    Wire.beginTransmission(TF_ADDRESS);
    Wire.write(triggerCmd, 5);
    Wire.endTransmission();

    // Request data
    Wire.requestFrom(TF_ADDRESS, DATA_LENGTH);

    uint8_t data[DATA_LENGTH];
    int index = 0;

    while (Wire.available() && index < DATA_LENGTH) {
        data[index++] = Wire.read();
    }

    if (index == DATA_LENGTH) {
        uint16_t distance = data[2] + data[3] * 256;
        return distance;
    }

    return -1;   // return -1 if reading fails
}