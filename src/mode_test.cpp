// ======================================================================
// RUN_MODE 2: State-based animation player
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

namespace {

// ---------------------------------------------------------------------
// Hardware
// ---------------------------------------------------------------------

constexpr uint8_t NUM_JOINTS = 8;
constexpr uint8_t CH[NUM_JOINTS] = {0, 1, 2, 3, 4, 5, 6, 7};  // PCA9685 channels

Adafruit_PWMServoDriver pwmTest = Adafruit_PWMServoDriver();

// ---------------------------------------------------------------------
// Keyframes
// ---------------------------------------------------------------------

struct Pose {
  int ticks[NUM_JOINTS];
  uint16_t duration_ms; // time to reach this pose from previous pose
};

// ---------- EDIT THESE KEYFRAMES ----------
Pose poseStraight[] = {
  {{340, 326, 340, 367, 312, 335, 381, 326}, 2000},  // straight
};

Pose poseSquat[] = {
  {{340, 312, 330, 367, 312, 335, 381, 326}, 2000},  // straight
  {{576, 340, 104, 340, 340, 576, 340, 104}, 2000},  // squat
};

Pose poseLunge[] = {
  {{340, 284, 340, 423, 423, 335, 242, 326}, 2000},  // lunge 1
  {{340, 326, 340, 367, 312, 335, 381, 326}, 2000},  // straight
  {{340, 437, 340, 256, 256, 335, 423, 326}, 2000},  // lunge 2
  {{340, 326, 340, 367, 312, 335, 381, 326}, 2000},  // straight
};

Pose poseRun[] = {
  {{340, 131, 340, 367, 312, 335, 117, 326}, 2000},  // lunge
  {{340, 576, 340, 367, 312, 335, 562, 326}, 2000},  // lunge
};

Pose poseRdl[] = {
  {{340, 326, 340, 367, 312, 335, 381, 326}, 2000},  // straight
  {{340, 173, 340, 465, 215, 335, 520, 326}, 2000},  // rdl
};

// ---------------------------------------------------------------------
// Animation table: the enum is the index into ANIMATIONS
// ---------------------------------------------------------------------

enum class AnimId : uint8_t {
  Straight = 0,
  Squat,
  Lunge,
  Run,
  Rdl,
  COUNT
};

struct Animation {
  const char* name;
  const Pose* poses;
  uint8_t     lenght;
  bool        loop;
};

template <size_t N>
Animation makeAnim(const char* name, const Pose (&poses)[N], bool loop) {
  return Animation{name, poses, (uint8_t)N, loop};
}

const Animation ANIMATIONS[] = {
  makeAnim("straight", poseStraight, false),
  makeAnim("squat",    poseSquat,    true),
  makeAnim("lunge",    poseLunge,    true),
  makeAnim("run",      poseRun,      true),
  makeAnim("rdl",      poseRdl,      true),
};

static_assert(sizeof(ANIMATIONS) / sizeof(ANIMATIONS[0]) == (size_t)AnimId::COUNT,
              "ANIMATIONS table must have one row per AnimId");

const Animation& getAnim(AnimId id) {return ANIMATIONS[(uint8_t)id]; }

// ---------------------------------------------------------------------
// Player state machine
//  Holding : not moving, servos keep their last command
//  Moving  : interpolating toward the current keyframe
// ---------------------------------------------------------------------

enum class PlayerState : uint8_t { Holding, Moving };

PlayerState state       = PlayerState::Holding;
AnimId      currentAnim = AnimId::Straight;
uint8_t     frameIndex  = 0;

int currentTicks[NUM_JOINTS]; // where the servos are currently commanded to go
int startTicks[NUM_JOINTS];   // where the current segment started
const Pose* targetPose = nullptr;


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

void startSegment(const Pose& to) {
  for (uint8_t i = 0; i < NUM_JOINTS; i++) startTicks[i] = currentTicks[i];
  targetPose      = &to;
  segmentStart    = millis();
  segmentDuration = to.duration_ms;
  state           = PlayerState::Moving;
}

void playAnimation(AnimId id) {
  currentAnim = id;
  frameIndex = 0;
  Serial.print(F("Playing: "));
  Serial.println(getAnim(id).name);
  startSegment(getAnim(id).poses[0]);
}

void onSegmentFinished() {
  const Animation& anim = getAnim(currentAnim);
  uint8_t next = frameIndex + 1;

  if (next < anim.lenght) {
    frameIndex = next;
    startSegment(anim.poses[frameIndex]);

  } else if (anim.loop && anim.lenght > 1) {
    frameIndex = 0;
    startSegment(anim.poses[frameIndex]);
  } else {
    state = PlayerState::Holding;
  }
}

void updatePlayer() {
  switch (state) {
    case PlayerState::Holding:
      // nothing to do, the PCA9685 keeps holding the last PWM
      break;
    case PlayerState::Moving: {
      unsigned long elapsed = millis() - segmentStart;
      float t = (segmentDuration == 0) ? 1.0f : (float)elapsed / (float)segmentDuration;
      float te = easeInOut(t);

      for (uint8_t i = 0; i < NUM_JOINTS; i++) {
        currentTicks[i] = lerpInt(startTicks[i], targetPose->ticks[i], te);
        setServoTicks(CH[i], currentTicks[i]);
      }

      if (t >= 1.0f) onSegmentFinished();
      break;
    }
  }
}

// A new request switches animation; re-pressing the playing one is ignored.
void requestAnimation(AnimId id) {
  if (id == currentAnim && state == PlayerState::Moving) return;
  playAnimation(id);
}

// ---------------------------------------------------------------------
// Input: physical buttons (wired pin -> button -> GND, using INPUT_PULLUP)
// CHANGE THESE PINS to free GPIOs on your board.
// ---------------------------------------------------------------------
constexpr unsigned long DEBOUNCE_MS = 30;
 
struct Button {
  uint8_t       pin;
  AnimId        anim;
  bool          lastReading;
  bool          stable;
  unsigned long lastChange;
};
 
Button buttons[] = {
  {2, AnimId::Straight},  // button 0
  {3, AnimId::Squat},     // button 1
  {4, AnimId::Lunge},     // button 2
  {5, AnimId::Run},       // button 3
  {7, AnimId::Rdl},       // button 4
};

// Returns true once, on the press edge, after debouncing
bool wasPressed(Button& b) {
  bool reading = (digitalRead(b.pin) == LOW);
  unsigned long now = millis();
 
  if (reading != b.lastReading) {
    b.lastReading = reading;
    b.lastChange  = now;
  }
  if ((now - b.lastChange) > DEBOUNCE_MS && reading != b.stable) {
    b.stable = reading;
    return b.stable;
  }
  return false;
}

void readInputs() {
  for (Button& b : buttons) {
    if (wasPressed(b)) requestAnimation(b.anim);
  }
 
  // Serial: send '0'..'4' to pick an animation (handy for testing without buttons)
  while (Serial.available()) {
    char c = Serial.read();
    uint8_t idx = (uint8_t)(c - '0');
    if (idx < (uint8_t)AnimId::COUNT) requestAnimation((AnimId)idx);
  }
}
 
void printHelp() {
  Serial.println(F("Animation player ready. Press a button or send:"));
  for (uint8_t i = 0; i < (uint8_t)AnimId::COUNT; i++) {
    Serial.print(F("  "));
    Serial.print(i);
    Serial.print(F(" -> "));
    Serial.println(ANIMATIONS[i].name);
  }
} 

}  // namespace

// ======================================================================
// Entry points used by main.cpp
// ======================================================================

void setup_test() {
  // set up serial comunication
  Serial.begin(115200);

  // set up PCA board
  Wire.begin();
  pwmTest.begin();
  pwmTest.setPWMFreq(50);

  for (Button& b : buttons) {
    pinMode(b.pin, INPUT_PULLUP);
    b.lastReading = false;
    b.stable      = false;
    b.lastChange  = 0;
  }
 
  // Snap to the straight pose on boot and hold there
  const Pose& home = getAnim(AnimId::Straight).poses[0];
  for (uint8_t i = 0; i < NUM_JOINTS; i++) {
    currentTicks[i] = home.ticks[i];
    setServoTicks(CH[i], currentTicks[i]);
  }
  currentAnim = AnimId::Straight;
  state       = PlayerState::Holding;
 
  printHelp();
}

void loop_test() {
  readInputs();
  updatePlayer();
}