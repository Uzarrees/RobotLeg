#pragma once
// ======================================================================
// Animation data types. Pure data, no hardware.
// ======================================================================

#include <Arduino.h>

constexpr uint8_t NUM_JOINTS = 8;

// One keyframe: a target tick value per joint + how long to get there.
struct Pose {
  uint16_t ticks[NUM_JOINTS];
  uint16_t duration_ms;
};

// A list of keyframes.
//   count == 1  -> static pose: blend to it and hold
//   count  > 1  -> loops forever
struct Animation {
  const char* name;
  const Pose* poses;
  uint8_t     count;
};

// The table's order is the button / Serial number.
extern const Animation ANIMATIONS[];
extern const uint8_t   NUM_ANIMATIONS;
