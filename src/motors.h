#ifndef MOTORS_H
#define MOTORS_H

extern bool moving; // shows when to use PID control
extern float targetSpeed;
extern float lefttargetSpeed, righttargetspeed;

void motorsInit();

void forward(int speed);
void steer(float leftSpeed, float rightSpeed);
void backward(int speed);
void stop();
void leftturn(int speed);
void rightturn(int speed);
bool turnDegrees(float angle);
void forwardleftturn(int speed);
void forwardrightturn(int speed);
void backwardsleftturn(int speed);
void backwardsrightturn(int speed);

#endif