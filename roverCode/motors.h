#ifndef MOTORS_H
#define MOTORS_H

void motorsInit();

void forward(int speed);
void backward(int speed);
void stop();
void leftturn(int speed);
void rightturn(int speed);
void updateleft(int speed);
void updateright(int speed);

#endif