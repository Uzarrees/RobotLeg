#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>


constexpr uint8_t NUM_JOINTS = 3;
constexpr uint8_t CH[NUM_JOINTS] = {0, 1, 2};  // PCA9685 channels

Adafruit_PWMServoDriver pwmTest = Adafruit_PWMServoDriver();

struct Pose {
  int ticks[NUM_JOINTS];
  uint16_t duration_ms; // time to reach this pose from previous pose
};

// ---------- EDIT THESE KEYFRAMES ----------
Pose sequence[] = {
  {{410, 520, 600}, 600},   // pose 0
  {{480, 560, 520}, 600},   // pose 1
  {{520, 480, 610}, 600},   // pose 2
  {{410, 520, 600}, 600},   // back to pose 0
};
constexpr uint8_t SEQ_LEN = sizeof(sequence) / sizeof(sequence[0]);

// ---------- Player state ----------
Pose startPose;         // where we started this segment
Pose targetPose;        // where we’re going
uint8_t targetIndex = 0;

unsigned long segmentStart = 0;
uint16_t segmentDuration = 600;

float easeInOut(float t) {
  t = constrain(t, 0.0f, 1.0f);
  return t * t * (3.0f - 2.0f * t); // smoothstep
}

int lerpInt(int a, int b, float t) {
  return (int)lroundf(a + (b - a) * t);
}

void setServoTicks(uint8_t ch, int ticks) {
  ticks = constrain(ticks, 0, 4095);
  pwmTest.setPWM(ch, 0, ticks);
}

void applyPoseInstant(const Pose& p) {
  for (uint8_t i = 0; i < NUM_JOINTS; i++) {
    setServoTicks(CH[i], p.ticks[i]);
  }
}

void beginSegment(uint8_t nextIndex) {
  // Start pose = current output (we use last target as "current" for simplicity)
  startPose = targetPose;
  targetPose = sequence[nextIndex];

  segmentStart = millis();
  segmentDuration = targetPose.duration_ms;
  targetIndex = nextIndex;
}

void setup_test() {
  Serial.begin(115200);
  Wire.begin();
  pwmTest.begin();
  pwmTest.setPWMFreq(50);

  // Start at first pose
  targetPose = sequence[0];
  applyPoseInstant(targetPose);

  segmentStart = millis();
  segmentDuration = targetPose.duration_ms;
  targetIndex = 0;

  // Immediately queue next segment
  beginSegment(1);
}

void loop_test() {
  unsigned long now = millis();
  float t = (segmentDuration == 0) ? 1.0f : (float)(now - segmentStart) / (float)segmentDuration;
  float te = easeInOut(t);

  // Interpolate each joint
  for (uint8_t i = 0; i < NUM_JOINTS; i++) {
    int cur = lerpInt(startPose.ticks[i], targetPose.ticks[i], te);
    setServoTicks(CH[i], cur);
  }

  // Segment done? Advance.
  if (t >= 1.0f) {
    uint8_t next = (targetIndex + 1) % SEQ_LEN;
    beginSegment(next);
  }

  // Your buttons/sensors can run here—no delays
}