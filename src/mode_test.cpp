// ======================================================================
// State-based animation player
//
// Pick an animation with a button (or by sending a digit trough serial)
//  button/key 0 -> straight  (stop & hold)
//  button/key 1 -> squat     (loops)
//  button/key 2 -> lunge     (loops)
//  button/key 3 -> run       (loops)
//  button/key 4 -> rdl       (loops)
// 
// The selected animation is looping until another one is chosen.
// Switching animations allways blends smoothly from whathever
//  position the servos are at any given moment
// ======================================================================

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include "Animations.h"
#include "AnimationPlayer.h"


namespace {

Adafruit_PWMServoDriver pwmTest;
AnimationPlayer player(pwmTest);

// Button i plays ANIMATIONS[i]. Wire each button from its pin to GND
constexpr uint8_t BUTTON_PINS[] = {2, 3, 4, 5, 7};
constexpr uint8_t NUM_BUTTONS = sizeof(BUTTON_PINS);
constexpr unsigned long DEBOUNCE_MS = 50;

bool isDown[NUM_BUTTONS] = {};
unsigned long lastChange[NUM_BUTTONS] = {};

void select(uint8_t index) {
  if (index >= NUM_ANIMATIONS) return;
  player.play(index);
  Serial.print(F("Playing: "));
  Serial.println(ANIMATIONS[index].name);
}

void readButtons() {
  unsigned long now = millis();
  for (uint8_t i = 0; i < NUM_BUTTONS; i++) {
    bool down = (digitalRead(BUTTON_PINS[i]) == LOW);
    if (down != isDown[i] && now - lastChange[i] > DEBOUNCE_MS) {
      isDown[i]     = down;
      lastChange[i] = now;
      if (down) select(i);  // act on press, not release
    }
  }
}

void readSerial() {
  while (Serial.available()) {
    char c = Serial.read();
    if (c >= '0' && c <= '9') select(c - '0');
  }
}

} // namespace

// ======================================================================
// Entry points used by main.cpp
// ======================================================================

void setup_test() {
  // set up serial comunication
  Serial.begin(115200);

  // set up PCA board
  Wire.begin();
  Wire.setClock(400000);
  pwmTest.begin();
  pwmTest.setPWMFreq(50);

  for (uint8_t pin : BUTTON_PINS) pinMode(pin, INPUT_PULLUP);

  // Snap to the straight pose on boot 
  player.snapTo(ANIMATIONS[0].poses[0]);
 
  Serial.println(F("Ready. Press a button or send:"));
  for (uint8_t i = 0; i < NUM_ANIMATIONS; i++) {
    Serial.print(F("  "));
    Serial.print(i);
    Serial.print(F(" = "));
    Serial.println(ANIMATIONS[i].name);
  }

}

void loop_test() {
  readButtons();
  readSerial();
  player.update();
}
