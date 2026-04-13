#pragma once

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

enum class Cmd {
    homing,
    pause,
    all,
    leftLeg,
    rightLeg,
    straight,
    tiltLeft,
    tiltRight,
};

struct Command {
    Cmd cmd;
    float* angles;
    uint16_t duration;
};

class Robot {
    public:
        Robot(Adafruit_PWMServoDriver& pwm);
        void homing (uint16_t duration = 1500);
        void homing(float* angles, uint16_t duration = 1500);
        void moveAllServos(float* angles, uint16_t duration = 0);
        void moveLeftLeg(float* angles, uint16_t duration = 0);
        void moveRightLeg(float* angles, uint16_t duration = 0);
        void hipStraight(uint16_t duration = 0);
        void tiltLeft(uint16_t duration = 0);
        void tiletRight(uint16_t duration = 0);
        void enableTorque();
        void disableTorque();
        bool isTorqueOn();
        bool isRunning();
        void runCommand(const Command& c);

    private:
        const uint8_t ankleRight   = 0;
        const uint8_t kneeRight    = 1;
        const uint8_t hipLowRight  = 2;
        const uint8_t hipHighRight = 3;
        const uint8_t hipHighLeft  = 4;
        const uint8_t hipLowLeft   = 5;
        const uint8_t kneeLeft     = 6;
        const uint8_t ankleLeft    = 7;
        bool running = false;
        bool torqueOn = false;
        unsigned long ms = 0;
        unsigned long lastDuration = 0;
        void setServoAngle(uint8_t ch, float angle);
        void setServoTicks(uint8_t ch, int ticks);
        Adafruit_PWMServoDriver* pwmDriver;
};