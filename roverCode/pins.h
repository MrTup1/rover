#ifndef PINS_H
#define PINS_H

// button: 3 / second echo line??
// servo 21

// IMU tx: 17
// IMU RX: 16

// -------------------- esp32 pins -----------------
// motors
//motor 1 (front left)
#define DIR_FL  13
#define PWM_FL  12
//motor 2 (front right)
#define DIR_FR  25
#define PWM_FR  33
//motor 3 (Back left)
#define DIR_BL  15
#define PWM_BL  2
//motor 4 (Back right)
#define DIR_BR  18
#define PWM_BR  19

// encoders
// (front left A and B enecoders)
#define ENA_FL 14
#define ENB_FL 35
// (front right A and B enecoders)
#define ENA_FR 39
#define ENB_FR 36
// (back left A and B enecoders)
#define ENA_BL 4
#define ENB_BL 5
// (back right A and B enecoders)
#define ENA_BR 22
#define ENB_BR 23

//ultrasonic sensors
#define echoR 34
#define echoL 3
#define USR_TRIG 1
#define USL_TRIG 32
//lidar


#endif