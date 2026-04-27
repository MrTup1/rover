#include <Arduino.h>
#include "navigation.h"
#include "motors.h"
#include "encoders.h"
#include "sensors.h"
#include "IMU.h"
#include "auto.h" 

int ROVER_LENGTH = 420; 
unsigned long lastPrintTime = 0;
String receivedMessage = ""; 
int inputState = 0; // 0 = Wait for Distance Input, 1 = Wait for Angle Input, 2 = Automative state
float targetDistance = 0.0;
float targetAngle = 0.0;
float desiredHeading = 0.0;
float error = 9999;
float distance_error = 9999;
float mainturnAngle = 0;
float avoidTargetHeading = 0;
float previousSideDistance = 0;


bool destinationReached = false;
bool obstacleLocationLogged = false;
bool wallClose;
bool wallFar;
bool edgeDetected;    //Whether a corner of a box has been reached
float edgeDetectedAt; //Snapshot of distance when edge has been detected

//STATE 5 VARIABLES
unsigned long openSpaceStart = 0; 
const int OPEN_SPACE_TIME_MS = 300; // 300ms of consistent open space

//P controller heading variables
static float Kp = 0.6;
int baseSpeed = 90;

void runNavigationMode() {
    if (mode != NAVIGATION) return; // Master wrapper check

    if (inputState == 2) { //target angle and distance given
          destinationReached = false;

          if (turnDegrees(desiredHeading)) {
            resetEncoders();
            inputState = 3;
          }
    }

    if (inputState == 3) {
        //Rotation finished, time to move forward to the goal
        distance_error = sqrt((pow((targetDistance - globalX) , 2) + pow((globalY) , 2)));
        
        if (distance_error < 50 || globalX >= targetDistance) {
            stop();
            destinationReached = true;
            inputState = 0; // Go all the way back to waiting for a new Serial input
            mode = FREEDRIVE;
        }

        else if (frontDistance > 0 && frontDistance < 25) {
            stop();
            //Object detected directly in front of rover
            if (leftDistance > rightDistance) {
            mainturnAngle = -90.0f;
            } else if (rightDistance > leftDistance) {
            mainturnAngle = 90.0f;
            } else {
            mainturnAngle = 90.0f; // default to turn right if both sides have equal distance
            }

            avoidTargetHeading = heading + mainturnAngle;
            inputState = 4;
        }

        else {
            float headingError = desiredHeading - heading;

            //Check whether right of left turn is closer if overlap has occured
            if (headingError > 180) {
            headingError -= 360;
            } else if (headingError < -180) {
            headingError += 360;
            }
            
            //Implement P controller
            float P_term = Kp * headingError; //This is the adjustment if rover is slightly heading to the left or right
            int leftSpeed = baseSpeed + P_term;
            int rightSpeed = baseSpeed - P_term;
            steer(leftSpeed, rightSpeed);
        }
    }
    
    //Turn 90 degrees away from obstacle

    if (inputState == 4) { 
        if (turnDegrees(avoidTargetHeading)) {
        edgeDetected = false;             //reset flag variables
        obstacleLocationLogged = false;
        openSpaceStart = 0;
        stop();
        inputState = 5;
        } 
    }


    if (inputState == 5) { 
        // Safety Check for state 5, ensure front is clear at all times for step 1 and 2
        if (frontDistance > 0 && frontDistance < 25) {
            stop();
            
            // Reset all State 5 progress flags so it starts fresh next time
            edgeDetected = false;             
            obstacleLocationLogged = false;
            openSpaceStart = 0;
            
            // Decide which way to turn, just like in State 3
            if (leftDistance > rightDistance) {
                mainturnAngle = -90.0f;
            } else {
                mainturnAngle = 90.0f; 
            }

            avoidTargetHeading = heading + mainturnAngle;
            inputState = 4; // Loop back to the turning state!
            return; // Skip the rest of State 5 for this loop iteration
        }

        // STEP 2: Go forward until path is clear on closer ultrasonic

        bool sideClear = false;
        if (mainturnAngle == -90.0f) { //Turn left so right side is closer to wall
        sideClear = (rightDistance < 0.0 || rightDistance > 40);
        } else if (mainturnAngle == 90.0f) { 
        sideClear = (leftDistance < 0.0 || leftDistance > 40);
        }

        if (!edgeDetected) { //Detect continuous ultrasonic readings with open space
            if (sideClear) {
                if (openSpaceStart == 0) {
                    openSpaceStart = millis();
                    forward(baseSpeed);
                } else if (millis() - openSpaceStart > OPEN_SPACE_TIME_MS) { //read 3 cont
                    edgeDetected = true;
                } else {
                    forward(baseSpeed);
                }
            } else {
                float currentSideDist = (mainturnAngle == -90.0f) ? rightDistance : leftDistance;
                
                if (currentSideDist > 0 && currentSideDist < 40) {
                    openSpaceStart = 0; 
                }
                openSpaceStart = 0;
                forward(baseSpeed);
            }
        }
        
        // Detect location of obstacle for STEP 2
        if (edgeDetected == true && obstacleLocationLogged == false) {
        edgeDetectedAt = (leftSideDistance + rightSideDistance) / 2.0f;
        obstacleLocationLogged = true;
        }
        
        // STEP 3: Go forward additional distance to ensure rover has enough turning space

        if (edgeDetected == true) {
        float distance_covered = (leftSideDistance + rightSideDistance) / 2.0f - edgeDetectedAt;
        if (distance_covered < ROVER_LENGTH) {
            forward(baseSpeed);
        } else {
            stop();
            inputState = 6;
        }
        }
    }

    // Reangle itself towards target

    if (inputState == 6) { 
        float rad = atan2(-globalY, (targetDistance - globalX)); //Get angle from odometry virtual plane
        desiredHeading = (rad * (180 / PI)) + startHeading;      //Convert odometry into real IMU data
        if (turnDegrees(desiredHeading)) {
        inputState = 3;
        };
    }


}