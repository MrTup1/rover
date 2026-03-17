#include <Arduino.h>
#include "sensors.h"
#include "pins.h"
#include "TFluna.h"

bool check_Obstacle() {
    if((leftDistance > 0 && leftDistance < 20 ) || 
    (rightDistance > 0 && rightDistance < 20) || 
    (frontDistance > 0 && frontDistance < 20)) {
        return true;
    }

    return false; // Default case
}