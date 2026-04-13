#include "Robot.h"

Robot::Robot(Adafruit_PWMServoDriver& pwm) {
    pwmDriver = &pwm;
    running = false;
}

void Robot::homing (uint16_t duration) {
    float angles[8] = {90, 90, 90, 90, 90, 90, 90, 90};
    moveAllServos(angles, duration);
}

void Robot::homing(float* angles, uint16_t duration) {
    moveAllServos(angles, duration);
}

void Robot::moveAllServos(float* angles, uint16_t duration) {
    setServoAngle(hipHighLeft,  angles[0]);
    setServoAngle(hipLowLeft,   angles[1]);
    setServoAngle(kneeLeft,     angles[2]);
    setServoAngle(ankleLeft,    angles[3]);
    setServoAngle(hipHighRight, angles[4]);
    setServoAngle(hipLowRight,  angles[5]);
    setServoAngle(kneeRight,    angles[6]);
    setServoAngle(ankleRight,   angles[7]);
}

void Robot::moveLeftLeg(float* angles, uint16_t duration) {
    setServoAngle(hipHighLeft, angles[0]);
    setServoAngle(hipLowLeft,  angles[1]);
    setServoAngle(kneeLeft,    angles[2]);
    setServoAngle(ankleLeft,   angles[3]);
}

void Robot::moveRightLeg(float* angles, uint16_t duration) {
    setServoAngle(hipHighRight, angles[0]);
    setServoAngle(hipLowRight,  angles[1]);
    setServoAngle(kneeRight,    angles[2]);
    setServoAngle(ankleRight,   angles[3]);
}

void Robot::hipStraight(uint16_t duration) { }

void Robot::tiltLeft(uint16_t duration) { }

void Robot::tiletRight(uint16_t duration) { }

void Robot::enableTorque() { }
 
void Robot::disableTorque() { }

bool Robot::isTorqueOn() { return false; }

bool Robot::isRunning() { return false; }

void Robot::runCommand(const Command& c) { 
    if (c.cmd == Cmd::all && c.angles) {
        moveAllServos(c.angles, c.duration);
    } else if (c.cmd == Cmd::leftLeg && c.angles) {
        moveLeftLeg(c.angles, c.duration);
    } else if (c.cmd == Cmd::rightLeg && c.angles) {
        moveRightLeg(c.angles, c.duration);
    }
}

void Robot::setServoAngle(uint8_t ch, float angle) {
  angle = constrain(angle, 0, 180);
  int ticks = map(angle, 0, 180, 90, 590);
  pwmDriver->setPWM(ch, 0, ticks);
}

void Robot::setServoTicks(uint8_t ch, int ticks) {
  ticks = constrain(ticks, 0, 4095);
  pwmDriver->setPWM(ch, 0, ticks);
}
