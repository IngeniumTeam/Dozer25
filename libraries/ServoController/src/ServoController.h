#include <Arduino.h>
#include <Servo.h>

class DoubleServo
{
    public:
        DoubleServo(uint8_t iPin1, uint8_t iPin2, int iFrom1 = 0, int iTo1 = 90, int iFrom2 = 0, int iTo2 = 90);
        void setup();
        void open();
        void openAll();
        Servo servo1;
        Servo servo2;
    private:
        uint8_t from1;
        uint8_t to1;
        uint8_t from2;
        uint8_t to2;
        uint8_t pin1;
        uint8_t pin2;
};

class SingleServo
{
    public:
        SingleServo(uint8_t iPin, int iFrom = 0, int iTo = 90);
        void setup();
        void open();
        void close();
        bool move(bool iState);
        bool move();
        bool toggle();
        Servo servo;
        bool state = false;
    private:
        uint8_t from;
        uint8_t to;
        uint8_t pin;
};

class Vacuum
{
    public:
        Vacuum(uint8_t iMotorPin, SingleServo iServo, bool iReversed = false);
        void setup();
        void on();
        void off();
        bool move();
        bool move(bool iState);
        bool toggle();
        SingleServo servo{0, 0, 0};
    private:
        uint8_t motorPin;
        bool state = false;
        bool reversed = false;
};
