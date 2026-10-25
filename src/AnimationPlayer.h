#pragma once
// ======================================================================
// Plays animations from the ANIMATIONS table with smooth easing.
// Knows nothing about buttons or Serial; you just tell it what to play.
// ======================================================================

#include <Adafruit_PWMServoDriver.h>
#include "Animations.h"

class AnimationPlayer {
public:
  explicit AnimationPlayer(Adafruit_PWMServoDriver& pwm);

  void snapTo(const Pose& pose);  // jump instantly (use once at boot)
  void play(uint8_t index);       // blend into ANIMATIONS[index]
  void update();                  // call every loop(), never blocks

  uint8_t currentIndex() const { return index_; }
  bool    isMoving()     const { return moving_; }

private:
  void startSegment(const Pose& to);
  void write(uint8_t joint, int ticks);

  Adafruit_PWMServoDriver& pwm_;

  // The player's state: which animation, which frame, and whether it's moving
  uint8_t index_  = 0;
  uint8_t frame_  = 0;
  bool    moving_ = false;

  const Pose*   target_       = nullptr;
  unsigned long segmentStart_ = 0;
  int start_[NUM_JOINTS]   = {};
  int current_[NUM_JOINTS] = {};
};
