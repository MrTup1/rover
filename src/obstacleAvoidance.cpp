#include <Arduino.h>
#include "sensors.h"
#include "pins.h"
#include "TFluna.h"
#include "motors.h"
#include "IMU.h"

int stopDistance = 20;
int autoModeState = 0;
float savedLeftDistance = 0;
float savedrightDistance = 0;

int baseSpeed = 50;
float newHeading = 0.0f;

bool check_Obstacle() {
    if((leftDistance > 0 && leftDistance <= stopDistance ) || 
    (rightDistance > 0 && rightDistance <= stopDistance) || 
    (frontDistance > 0 && frontDistance <= stopDistance)) {
        return true;
    }

    return false; // Default case
}

void resetAutoMode() {
    autoModeState = 0; // Reset state so it starts fresh next time
}

void runAutoMode() {
    if (autoModeState == 0) {
        if (check_Obstacle()) {
            stop();
            
            // Normalise Distance, treat -1.0 as 9999 to easier math computation
            float leftCompare = (leftDistance < 0.0f) ? 9999.0f : leftDistance;
            float rightCompare = (rightDistance < 0.0f) ? 9999.0f : rightDistance;

            if (leftCompare < 40 && rightCompare < 40) { // U Turn
                autoModeState = 1;
                newHeading = heading + 180.0f;
            } else if (leftCompare >= rightCompare && leftCompare > 40) {  // Turn left
                autoModeState = 1;
                newHeading = heading - 30.0f;
            } else { // Rover must be more open towards the right, turn regardless
                autoModeState = 1;
                newHeading = heading + 30.0f;
            } 

        } else if (frontDistance > stopDistance || frontDistance < 0.0f) {
            forward(baseSpeed);
        }
    } 

    else if (autoModeState == 1) {

        if (!check_Obstacle()) {
            if (turnDegrees(newHeading)) {
                autoModeState = 0;
            }
        } else {
            stop();
            autoModeState = 0;
        }

    }
}