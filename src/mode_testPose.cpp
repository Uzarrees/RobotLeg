// ======================================================================
// RUN_MODE 3: Testing
// ======================================================================

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include "mode_testPose.h"

void setup_testPose() {
  Serial.begin(9600);
  delay(1000);
  Serial.println("I2C scanner starting...");
  Wire.begin();
}

void loop_testPose() {
  byte error, address;
  int nDevices = 0;

  Serial.println("Scanning...");

  for (address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0) {
      Serial.print("I2C device found at 0x");
      if (address < 16) Serial.print("0");
      Serial.println(address, HEX);
      nDevices++;
    }
  }

  if (nDevices == 0) {
    Serial.println("No I2C devices found");
  }

  delay(3000);
}