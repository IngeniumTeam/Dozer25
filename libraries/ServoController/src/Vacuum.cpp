#include <Arduino.h>

#include "ServoController.h"

Vacuum::Vacuum(uint8_t iMotorPin, SingleServo iServo, bool iReversed = false)
{
    servo = iServo;
    motorPin = iMotorPin;
    reversed = iReversed;
    pinMode(motorPin, OUTPUT);
}

void Vacuum::setup()
{
    servo.setup();
    move();
}

void Vacuum::on()
{
    digitalWrite(motorPin, !reversed);
    servo.close();
}

void Vacuum::off()
{
    digitalWrite(motorPin, reversed);
    servo.open();
}

bool Vacuum::move()
{
    if (!state)
    {
        off();
    }
    return state;
}

bool Vacuum::move(bool iState)
{
    if (state == iState)
        return;
    state = iState;
    return move();
}

bool Vacuum::toggle()
{
    return move(!state);
}