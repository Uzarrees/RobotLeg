// ======================================================================
// RUN_MODE 1: Main program, state driven robot leg 
// ======================================================================

#include <Servo.h>
#include <Bounce2.h>
#include <Adafruit_PWMServoDriver.h>
#include "mode_main.h"
#include "Robot.h"

// ========= Program variables =========
// initialize robot class with the PWM driver
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
Robot robot(pwm);

// pins for the animation buttons
const int b0Pin = 2;
const int b1Pin = 3;
const int b2Pin = 4;
const int b3Pin = 5;
const int b4Pin = 6;
const int b5Pin = 7;

Bounce b0 = Bounce();
Bounce b1 = Bounce();
Bounce b2 = Bounce();
Bounce b3 = Bounce();
Bounce b4 = Bounce();
Bounce b5 = Bounce();

// enum to hold all the animations
enum State { STRAIGHT, KNEE_UP, HEEL_BACK, FOOT_FRWRD, FOOT_BKWRD, FOOT_SDWRD };

// define default state 
State state = STRAIGHT;


// ============== Setup ================
void setup_main() {
  // open serial communication
  Serial.begin(9600);

  // ROBOT SETUP
  pwm.begin();
  pwm.setPWMFreq(50);
  
  // BUTTONS SETUP
  // set up pins for the buttons 
  pinMode(b0Pin, INPUT_PULLUP);
  pinMode(b1Pin, INPUT_PULLUP);
  pinMode(b2Pin, INPUT_PULLUP);
  pinMode(b3Pin, INPUT_PULLUP);
  pinMode(b4Pin, INPUT_PULLUP);
  pinMode(b5Pin, INPUT_PULLUP);

  // attach the pins to the Bounce objects
  b0.attach(b0Pin);
  b1.attach(b1Pin);
  b2.attach(b2Pin);
  b3.attach(b3Pin);
  b4.attach(b4Pin);
  b5.attach(b5Pin);

  // set up the ubdate interval rate of the buttons
  b0.interval(25);
  b1.interval(25);
  b2.interval(25);
  b3.interval(25);
  b4.interval(25);
  b5.interval(25);
}

// ========= Loop =========
void loop_main() {
  // update buttons
  b0.update();
  b1.update();
  b2.update();
  b3.update();
  b4.update();
  b5.update();

  // toggle through states when a button is pressed
  if (b0.fell()) { state = STRAIGHT;}
  if (b5.fell()) { state = KNEE_UP;}
  /*if (b0.fell()) { state = STRAIGHT;}
  if (b1.fell()) { state = KNEE_UP;}
  if (b2.fell()) { state = HEEL_BACK;}
  if (b3.fell()) { state = FOOT_FRWRD;}
  if (b4.fell()) { state = FOOT_BKWRD;}
  if (b5.fell()) { state = FOOT_SDWRD;}*/

  switch (state) {
    case STRAIGHT:    animStraight();  break;
    case KNEE_UP:     animSquat();    break;
    case HEEL_BACK:   animHeelBack();  break;
    case FOOT_FRWRD:  animFootFrwrd(); break;
    case FOOT_BKWRD:  animFootBkwrd(); break;
    case FOOT_SDWRD:  animFootSdwrd(); break;
  }
}

// ========= Animations =========
void animStraight() {
  /*========= STRAIGHT =========*/
  float rAngles[4] = {90, 90, 90, 90};
  float lAngles[4] = {90, 90, 90, 90};
  robot.moveRightLeg(rAngles);
  robot.moveLeftLeg(lAngles);
  Serial.println("animStraight");
}

void animSquat() {
  /*========= SQUAT =========*/
  float rAngles[4] = {90, 10, 90, 170};
  float lAngles[4] = {90, 170, 90, 10};
  robot.moveRightLeg(rAngles);
  robot.moveLeftLeg(lAngles);
  Serial.println("animSquat");
}

void animKneeUp() {
  /*========= LUNGE =========*/
  float rAngles[4] = {170, 90, 170, 90};
  float lAngles[4] = {170, 90, 170, 90};
  robot.moveRightLeg(rAngles);
  robot.moveLeftLeg(lAngles);
  Serial.println("animKneeUp");
}

void animHeelBack() {
  float angles[4] = {90, 180, 90, 180};
  robot.moveRightLeg(angles);
  Serial.println("animHeelBack");
}

void animFootFrwrd() {
  float angles[4] = {90, 45, 90, 90};
  robot.moveRightLeg(angles);
  Serial.println("animFootFrwrd");
}

void animFootBkwrd() {
  float angles[4] = {90, 135, 90, 90};
  robot.moveRightLeg(angles);
  Serial.println("animFootBkwrd");
}

void animFootSdwrd() {
  float angles[4] = {135, 90, 90, 90};
  robot.moveRightLeg(angles);
  Serial.println("animFootSdwrd");
}