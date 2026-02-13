#ifndef PINS_H
#define PINS_H

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
#define ENB_FL 27
// (front right A and B enecoders)
#define ENA_FR 32
#define ENB_FR 35
// (back left A and B enecoders)
#define ENA_BL 4
#define ENB_BL 16
// (back right A and B enecoders)
#define ENA_BR 22
#define ENB_BR 23

//IMU
#define IMU_SDA 21
#define IMU_SCL 17
#define LED_PIN 2   // Built-in LED for calibration status

#endif