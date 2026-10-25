// ======================================================================
// Keyframe data. To add an animation:
//   1. write a Pose array below
//   2. add one ANIM(...) line to the table
// ======================================================================

#include "Animations.h"

// Builds an Animation from a Pose array, counting its frames automatically.
#define ANIM(name, poses) { name, poses, sizeof(poses) / sizeof(poses[0]) }

namespace {

// Shared pose, reused across animations instead of copy-pasting the numbers.
constexpr Pose STRAIGHT = {{340, 326, 340, 367, 312, 335, 381, 326}, 2000};

const Pose straight[] = {
  STRAIGHT,
};

const Pose squat[] = {
  STRAIGHT,
  {{576, 340, 104, 340, 340, 576, 340, 104}, 2000},  // squat
};

const Pose lunge[] = {
  STRAIGHT,
  {{340, 284, 340, 423, 423, 335, 242, 326}, 2000},  // lunge 1
  STRAIGHT,
  {{340, 437, 340, 256, 256, 335, 423, 326}, 2000},  // lunge 2
};

const Pose run[] = {
  STRAIGHT,
  {{340, 131, 340, 367, 312, 335, 117, 326}, 2000},  // stride 1
  {{340, 576, 340, 367, 312, 335, 562, 326}, 2000},  // stride 2
};

const Pose rdl[] = {
  STRAIGHT,
  {{340, 173, 340, 465, 215, 335, 520, 326}, 2000},  // rdl
};

}  // namespace

// Index 0 is the boot / stop pose.
const Animation ANIMATIONS[] = {
  ANIM("straight", straight),  // 0
  ANIM("squat",    squat),     // 1
  ANIM("lunge",    lunge),     // 2
  ANIM("run",      run),       // 3
  ANIM("rdl",      rdl),       // 4
};

const uint8_t NUM_ANIMATIONS = sizeof(ANIMATIONS) / sizeof(ANIMATIONS[0]);
