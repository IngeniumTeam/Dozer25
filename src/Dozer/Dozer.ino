#include <Arduino.h>
#include <Bluetooth.h>
#include <Mecanum.h>
#include <Report.h>
#include <Led.h>
#include <ServoController.h>
#include <Digit.h>
#include <Timino.h>

#define MIN_SPEED 90
#define DEFAULT_SPEED 180
#define MAX_SPEED 255

#define DIAGONAL_THRESHOLD 30

#define DEBUG false
#define TIME_DEBUG false

// Servo
#define SERVO_1 7
#define SERVO_2 8
#define SERVO_3 9
#define SERVO_4 10

// Relais
#define RELAIS_1 29
#define RELAIS_2 30

// Motors
//// 1
#define PWMA_1 2
#define PWMB_1 3
#define INA1_1 50
#define INA2_1 51
#define INB1_1 25
#define INB2_1 26
#define STBY_1 21
//// 2
#define PWMA_2 4
#define PWMB_2 5
#define INA1_2 52
#define INA2_2 53
#define INB1_2 27
#define INB2_2 28
#define STBY_2 22

// Stepper
#define STEP 6
#define DIR 23
#define EN 24

// LEDs RGB
//// 1
#define RGBA_1 42
#define RGBB_1 37
#define RGBC_1 38
//// 2
#define RGBA_2 45
#define RGBB_2 44
#define RGBC_2 41

// Limit switch
#define LMTS_1 36
#define LMTS_2 35

// 7 segments digit
#define DIGITA 40
#define DIGITB 39

// Values positions in the JSON message
#define SWITCH 0
#define KEYPAD 1
#define ESTIMATION 2
#define JOYSTICK_LEFT_X 3
#define JOYSTICK_LEFT_Y 4
#define JOYSTICK_LEFT_CLCK 5
#define JOYSTICK_RIGHT_X 6
#define JOYSTICK_RIGHT_Y 7
#define JOYSTICK_RIGHT_CLCK 8

#define NUM_VALUES 9

//                                            left                                                                right                                     mapping                //
//                       _________________________________________________                   _________________________________________________        ___________________          //
//                       top                       bottom              stby                  top                       bottom             stby        from             to          //
//              _______________________    _______________________    _______      _______________________    _______________________    _______     _______    ________________   //
Mecanum mecanum(INA2_1, INA1_1, PWMA_1,    INB2_1, INB1_1, PWMB_1,    STBY_1,      INA1_2, INA2_2, PWMA_2,    INB2_2, INB1_2, PWMB_2,    STBY_2,     0, 255,    0, DEFAULT_SPEED); //

#include <Mecaside.h>
Mecaside left(Left);
Mecaside right(Right);

int sizes[NUM_VALUES] = { 1, 4, 8, 9, 9, 1, 9, 9, 1 };
Bluetooth bluetooth(&Serial1, sizes, NUM_VALUES, '.');
Report report(&Serial, DEBUG, 100);

LedRGB bluetoothLed(RGBA_1, RGBB_1, RGBC_1, true);
LedRGB led2(RGBA_2, RGBB_2, RGBC_2, true);
Digit digit(DIGITB, DIGITA, 7);

Motor motor(DIR, RELAIS_2, 0, STEP, 0, 5, 0, 255);

SingleServo rackServo(SERVO_3, 50, 8);

int estimation = 60;
int speedStatus = 0;
int key = 0;
bool startToOpen = false;
int correction = 0; // 0 = no correction ; +x = right ; -x = left

void setup() {
  // Serial setup //
  {
#if DEBUG
    Serial.println("Debug mode is on.");
    Serial.println("Serial communication is on.");
    Serial.println("Bluetooth communication is on.");
#endif
    Serial.begin(9600);
#if DEBUG
    Serial.println("Serial communication is on.");
#endif
    Serial1.begin(9600);
#if DEBUG
    Serial.println("Bluetooth communication is on.");
#endif
  }
  // Setup and stop the robot //
  {
    digit.display(estimation);
#if DEBUG
    Serial.println("Estimation is on.");
#endif
    rackServo.setup();
    rackServo.open();
    pinMode(RELAIS_1, OUTPUT);
    digitalWrite(RELAIS_1, LOW);
    pinMode(SERVO_4, OUTPUT);
    digitalWrite(SERVO_4, HIGH);
#if DEBUG
    Serial.println("Servos and magnets are on and set.");
#endif
    motor.backward(5);
    while (!digitalRead(LMTS_2));
    motor.forward(1);
    while(digitalRead(LMTS_2));
    motor.brake();
#if DEBUG
    Serial.println("Rack is on and down.");
#endif
    stop();
#if DEBUG
    Serial.println("All systems are running.");
#endif
  }
}

void loop() {
#if TIME_DEBUG
  int start = millis();
#endif
  //report.print();
  if (digitalRead(LMTS_1)) {
    motor.backward(1);
    while(digitalRead(LMTS_1));
    motor.brake();
  }
  if (digitalRead(LMTS_2)) {
    motor.forward(1);
    while(digitalRead(LMTS_2));
    motor.brake();
  }
  switch (bluetooth.receive()) {
    case 0:
      report.ok++;
      report.prob = 0;
      bluetoothLed.on(0, 0, 255);
      // Estimation //
      {
#if DEBUG
        Serial.print("estimation: "); Serial.print(bluetooth.message.get(ESTIMATION)); Serial.print(" | ");
#endif
        if (bluetooth.message.get(ESTIMATION) != 0 && bluetooth.message.get(ESTIMATION) != estimation) {
          estimation = bluetooth.message.get(ESTIMATION);
          digit.display(estimation);
        }
      }
      // Switch //
      {
#if DEBUG
        Serial.print("switch: "); Serial.print(bluetooth.message.get(SWITCH) != 0); Serial.print(" | ");
#endif
        if (bluetooth.message.get(SWITCH) != rackServo.state) {
          rackServo.toggle();
        }
      }
      // Speed change //
      {
        const int joystickY = bluetooth.message.get(JOYSTICK_LEFT_Y);
        if (joystickY > 255 && speedStatus != 1) {
#if DEBUG
          Serial.print("boost | ");
#endif
          mecanum.setMaxSpeed(MAX_SPEED);
          speedStatus = 1;
        }
        if (joystickY == 255 && speedStatus != 0) {
#if DEBUG
          Serial.print("default | ");
#endif
          mecanum.setMaxSpeed(DEFAULT_SPEED);
          speedStatus = 0;
        }
        if (joystickY < 255 && speedStatus != 2) {
#if DEBUG
          Serial.print("slow | ");
#endif
          mecanum.setMaxSpeed(MIN_SPEED);
          speedStatus = 2;
        }
      }
      // Motors //
      {
#if DEBUG
        Serial.print("left: "); Serial.print(constrain((-(bluetooth.message.get(JOYSTICK_RIGHT_X) - 255) + (bluetooth.message.get(JOYSTICK_LEFT_X) - 255)), -255, 255)); Serial.print(" | ");
        Serial.print("right: "); Serial.print(constrain((-(bluetooth.message.get(JOYSTICK_RIGHT_X) - 255) - (bluetooth.message.get(JOYSTICK_LEFT_X) - 255)), -255, 255)); Serial.print(" | ");
        Serial.print("sideway: "); Serial.print(constrain(bluetooth.message.get(JOYSTICK_RIGHT_Y) - 255, -255, 255)); Serial.print(" | ");
#endif
        left.move(constrain((-(bluetooth.message.get(JOYSTICK_RIGHT_X) - 255) + (bluetooth.message.get(JOYSTICK_LEFT_X) - 255)) - correction, -255, 255));
        right.move(constrain((-(bluetooth.message.get(JOYSTICK_RIGHT_X) - 255) - (bluetooth.message.get(JOYSTICK_LEFT_X) - 255)) + correction, -255, 255));
        mecanum.sideway(constrain(bluetooth.message.get(JOYSTICK_RIGHT_Y) - 255, -255, 255), bluetooth.message.get(JOYSTICK_LEFT_X) - 255);
        /*if (abs(bluetooth.message.get(JOYSTICK_RIGHT_X) - 255) > DIAGONAL_THRESHOLD && abs(bluetooth.message.get(JOYSTICK_RIGHT_Y) - 255) > DIAGONAL_THRESHOLD) {
#if DEBUG
          Serial.print("diagonal: "); Serial.print(bluetooth.message.get(JOYSTICK_RIGHT_X) - 255); Serial.print(", "); Serial.print(bluetooth.message.get(JOYSTICK_RIGHT_Y) - 255); Serial.print(" | ");
#endif
          mecanum.diagonal(bluetooth.message.get(JOYSTICK_RIGHT_X) - 255, bluetooth.message.get(JOYSTICK_RIGHT_Y) - 255);
        }*/
        if (startToOpen) {
          if (bluetooth.message.get(JOYSTICK_RIGHT_Y) != 255 || bluetooth.message.get(JOYSTICK_LEFT_Y) != 255) {
            startToOpen = false;
            digitalWrite(RELAIS_1, LOW);
          }
        }
      }
      // Brake //
      {
        if (bluetooth.message.get(JOYSTICK_LEFT_CLCK)) {
#if DEBUG
          Serial.print("brake |");
#endif
          mecanum.brake();
        }
      }
      // Keypad //
      {
        if (bluetooth.message.get(KEYPAD) != key && bluetooth.message.get(KEYPAD) != 0) {
          key = bluetooth.message.get(KEYPAD);
#if DEBUG
          Serial.print("key: "); Serial.print(key); Serial.print(" | ");
#endif
          switch (key) {
            case 1:
              motor.backward(5);
              break;
            case 2:
              motor.brake();
              break;
            case 3:
              motor.forward(5);
              break;
            case 4:
              motor.backward(1);
              delay(400);
              motor.brake();
              break;
            case 5:
              startToOpen = false;
              digitalWrite(RELAIS_1, LOW);
              break;
            case 6:
              startToOpen = true;
              digitalWrite(RELAIS_1, HIGH);
              break;
            case 7:
              motor.forward(1);
              delay(200);
              motor.brake();
              break;
            case 8:
              break;
            case 9:
              break;
            case 10:
              digitalWrite(SERVO_4, HIGH);
              break;
            case 11:
              digitalWrite(SERVO_4, LOW);
              stop();
              break;
          }
        }
      }
      break;
    case 1:
      report.inv++;
      report.prob = 0;
      bluetoothLed.off();
      break;
    case 2:
      report.ntr++;
      report.prob++;
      bluetoothLed.off();
      break;
  }
  if (report.prob >= 20) {
    stop();
  }
#if TIME_DEBUG
  Serial.print("time: "); Serial.print(millis() - start);
#endif
#if DEBUG
  Serial.println();
#endif
}

void stop() {
#if DEBUG
  Serial.print("stop | ");
#endif
  mecanum.stop();
  //motor.brake();
  //digitalWrite(RELAIS_1, LOW);
}
