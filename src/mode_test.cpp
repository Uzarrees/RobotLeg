// ======================================================================
// RUN_MODE 2: Ease movement and animation player
// - everything is very convoluted, the code must be cleaned out 
// ======================================================================

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>


constexpr uint8_t NUM_JOINTS = 8;
constexpr uint8_t CH[NUM_JOINTS] = {0, 1, 2, 3, 4, 5, 6, 7};  // PCA9685 channels

Adafruit_PWMServoDriver pwmTest = Adafruit_PWMServoDriver();

struct Pose {
  int ticks[NUM_JOINTS];
  uint16_t duration_ms; // time to reach this pose from previous pose
};

// ---------- EDIT THESE KEYFRAMES ----------
Pose seqTest[] = {
  {{340, 312, 330, 367, 312, 335, 381, 326}, 2000},  // straight
  {{451, 312, 228, 367, 312, 228, 381, 437}, 2000},  // shift left
  {{367, 312, 284, 367, 312, 228, 381, 409}, 2000},  // prep step
  {{367, 312, 284, 367, 312, 228, 381, 409}, 2000},  // right step
  {{367, 312, 284, 367, 312, 228, 381, 409}, 2000},  // hold
};

Pose seqWalk[] = {
  {{350, 312, 330, 367, 312, 335, 381, 330}, 1000},  // straight
  {{350, 173, 330, 451, 230, 335, 506, 330}, 1000},  // knees bent
  {{350, 187, 330, 451, 230, 335, 492, 330}, 500},  // trans
  {{350, 312, 330, 367, 312, 335, 381, 330}, 1000},  // straight
  {{562, 355, 117, 340, 340, 562, 340, 117}, 2000},  // knees bent
  {{562, 355, 104, 340, 340, 576, 340, 117}, 500},  // trans
  {{350, 312, 330, 367, 312, 335, 381, 330}, 3000},  // straight
};



constexpr uint8_t SEQ_LEN = sizeof(seqTest) / sizeof(seqTest[0]);

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
  targetPose = seqTest[nextIndex];

  segmentStart = millis();
  segmentDuration = targetPose.duration_ms;
  targetIndex = nextIndex;
}

void setup_test() {
  // set up serial comunication
  Serial.begin(115200);

  // set up PCA board
  Wire.begin();
  pwmTest.begin();
  pwmTest.setPWMFreq(50);

  // Start at first pose
  targetPose = seqTest[0];
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