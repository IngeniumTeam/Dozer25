#include <Arduino.h>

#include "ServoController.h"

SingleServo::SingleServo(uint8_t iPin, int iFrom = 0, int iTo = 90)
{
    from = iFrom;
    to = iTo;
    pin = iPin;
    pinMode(pin, OUTPUT);
}

void SingleServo::setup()
{
    servo.attach(pin);
    move();
}

void SingleServo::open()
{
    servo.write(to);
}

void SingleServo::close()
{
    servo.write(from);
}

bool SingleServo::move()
{
    if (state)
        open();
    else
        close();
    return state;
}

bool SingleServo::move(bool iState)
{
    if (state == iState)
        return;
    state = iState;
    return move();
}

bool SingleServo::toggle()
{
    return move(!state);
}