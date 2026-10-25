#include "AnimationPlayer.h"

namespace {

// PCA9685 channel for each joint
constexpr uint8_t CHANNEL[NUM_JOINTS] = {0, 1, 2, 3, 4, 5, 6, 7};

float smoothstep(float t) {
  return t * t * (3.0f - 2.0f * t);
}

}  // namespace

AnimationPlayer::AnimationPlayer(Adafruit_PWMServoDriver& pwm) : pwm_(pwm) {}

void AnimationPlayer::snapTo(const Pose& pose) {
  for (uint8_t j = 0; j < NUM_JOINTS; j++) write(j, pose.ticks[j]);
  moving_ = false;
}

void AnimationPlayer::play(uint8_t index) {
  if (index >= NUM_ANIMATIONS) return;
  if (index == index_ && moving_) return;  // already playing it

  index_ = index;
  frame_ = 0;
  startSegment(ANIMATIONS[index_].poses[0]);
}

void AnimationPlayer::update() {
  if (!moving_) return;

  unsigned long elapsed = millis() - segmentStart_;
  uint16_t duration = target_->duration_ms;
  float t = (duration == 0) ? 1.0f : (float)elapsed / duration;
  if (t > 1.0f) t = 1.0f;
  float e = smoothstep(t);

  for (uint8_t j = 0; j < NUM_JOINTS; j++) {
    int diff = (int)target_->ticks[j] - start_[j];
    write(j, start_[j] + (int)lroundf(diff * e));
  }

  if (t < 1.0f) return;

  // Segment finished: single poses hold, multi-frame animations loop
  const Animation& anim = ANIMATIONS[index_];
  if (anim.count <= 1) {
    moving_ = false;
    return;
  }
  frame_ = (frame_ + 1) % anim.count;
  startSegment(anim.poses[frame_]);
}

// Always starts from where the servos are NOW, so switching mid-move is smooth
void AnimationPlayer::startSegment(const Pose& to) {
  for (uint8_t j = 0; j < NUM_JOINTS; j++) start_[j] = current_[j];
  target_       = &to;
  segmentStart_ = millis();
  moving_       = true;
}

void AnimationPlayer::write(uint8_t joint, int ticks) {
  current_[joint] = constrain(ticks, 0, 4095);
  pwm_.setPWM(CHANNEL[joint], 0, current_[joint]);
}
