#include <Arduino.h>

#include "Mecanum.h"

/**
 * Setup the mecanum
 */
Mecanum::Mecanum(uint8_t in1_1, uint8_t in1_2, uint8_t pwm1,
                 uint8_t in2_1, uint8_t in2_2, uint8_t pwm2, uint8_t standBy1,
                 uint8_t in3_1, uint8_t in3_2, uint8_t pwm3,
                 uint8_t in4_1, uint8_t in4_2, uint8_t pwm4, uint8_t standBy2,
                 int fromMin = 0, int fromMax = 1023, int toMin = 0, int toMax = 255)
{
    motors[Left][Front] = Motor(in1_1, in1_2, pwm1, standBy1, fromMin, fromMax, toMin, toMax);
    motors[Left][Back] = Motor(in2_1, in2_2, pwm2, standBy1, fromMin, fromMax, toMin, toMax);
    motors[Right][Front] = Motor(in3_1, in3_2, pwm3, standBy2, fromMin, fromMax, toMin, toMax);
    motors[Right][Back] = Motor(in4_1, in4_2, pwm4, standBy2, fromMin, fromMax, toMin, toMax);
}

void Mecanum::forward(int speed)
{
    motors[Left][Front].forward(speed);
    motors[Left][Back].forward(speed);
    motors[Right][Front].forward(speed);
    motors[Right][Back].forward(speed);
}

void Mecanum::backward(int speed)
{
    motors[Left][Front].backward(speed);
    motors[Left][Back].backward(speed);
    motors[Right][Front].backward(speed);
    motors[Right][Back].backward(speed);
}

void Mecanum::move(int speed)
{
    if (speed > 0)
        forward(speed);
    if (speed == 0)
        stop();
    if (speed < 0)
        backward(-speed);
}

void Mecanum::stop()
{
    motors[Left][Front].stop();
    motors[Left][Back].stop();
    motors[Right][Front].stop();
    motors[Right][Back].stop();
}

void Mecanum::brake()
{
    motors[Left][Front].brake();
    motors[Left][Back].brake();
    motors[Right][Front].brake();
    motors[Right][Back].brake();
}

void Mecanum::sideway(int speed, int turn)
{
    if (speed > 0)
        sidewayRight(speed, turn);
    if (speed < 0)
        sidewayLeft(-speed, turn);
}

void Mecanum::sidewayLeft(int speed, int turn)
{
    motors[Left][Front].backward(constrain(speed - turn, 0, 255));
    motors[Left][Back].forward(constrain(speed + turn, 0, 255));
    motors[Right][Front].forward(constrain(speed - turn, 0, 255));
    motors[Right][Back].backward(constrain(speed + turn, 0, 255));
}

void Mecanum::sidewayRight(int speed, int turn)
{
    motors[Left][Front].forward(constrain(speed + turn, 0, 255));
    motors[Left][Back].backward(constrain(speed - turn, 0, 255));
    motors[Right][Front].backward(constrain(speed + turn, 0, 255));
    motors[Right][Back].forward(constrain(speed - turn, 0, 255));
}

void Mecanum::diagonal(int xSpeed, int ySpeed)
{
    if (xSpeed > 0)
        diagonalLeft(ySpeed);
    if (xSpeed < 0)
        diagonalRight(ySpeed);
}

void Mecanum::diagonalLeft(int speed)
{
    if (speed > 0)
        diagonalLeftForward(speed);
    if (speed < 0)
        diagonalLeftBackward(-speed);
}

void Mecanum::diagonalRight(int speed)
{
    if (speed > 0)
        diagonalRightForward(speed);
    if (speed < 0)
        diagonalRightBackward(-speed);
}

void Mecanum::diagonalLeftForward(int speed)
{
    motors[Left][Front].stop();
    motors[Left][Back].forward(speed);
    motors[Right][Front].forward(speed);
    motors[Right][Back].stop();
}

void Mecanum::diagonalRightForward(int speed)
{
    motors[Left][Front].forward(speed);
    motors[Left][Back].stop();
    motors[Right][Front].stop();
    motors[Right][Back].forward(speed);
}

void Mecanum::diagonalLeftBackward(int speed)
{
    motors[Left][Front].stop();
    motors[Left][Back].backward(speed);
    motors[Right][Front].backward(speed);
    motors[Right][Back].stop();
}

void Mecanum::diagonalRightBackward(int speed)
{
    motors[Left][Front].backward(speed);
    motors[Left][Back].stop();
    motors[Right][Front].stop();
    motors[Right][Back].backward(speed);
}

void Mecanum::setMaxSpeed(int speed)
{
    motors[Left][Front].toMax = speed;
    motors[Left][Back].toMax = speed;
    motors[Right][Front].toMax = speed;
    motors[Right][Back].toMax = speed;
}