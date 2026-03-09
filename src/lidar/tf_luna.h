#ifndef TF_LUNA_H
#define TF_LUNA_H

#include <Wire.h>

class TFLuna {

public:
    TFLuna(uint8_t sda, uint8_t scl);

    void begin();
    int getDistance();

private:
    uint8_t _sda;
    uint8_t _scl;

    const uint8_t TF_ADDRESS = 0x10;
    const uint8_t DATA_LENGTH = 9;

    unsigned char triggerCmd[5] = {0x5A, 0x05, 0x00, 0x01, 0x60};
};

#endif
