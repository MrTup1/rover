#ifndef AUTO_H
#define AUTO_H

extern int SPEED;
extern volatile bool obstruction;
extern float Direction;

enum MotionState {
  STOPPED,
  FORWARD,
  LEFTTURN,
  RIGHTTURN,
  BACKWARDS,
  FORWARDRIGHT,
  FORWARDLEFT,
  BACKWARDSRIGHT,
  BACKWARDSLEFT,
  OBSTRUCTION,
  TURNING_90,
  TURNING_180,
  WAITING,
  STARTRETURN
};

extern MotionState motionState;

enum Mode {
  FREEDRIVE,
  AUTO,
  RETURN,
  NAVIGATION
};

extern Mode mode;

struct Move {
  MotionState type;
  float value;   // distance (encoder) OR angle (IMU)
};

void bumperInit();
void runAutoMode();

#endif