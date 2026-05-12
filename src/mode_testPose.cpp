// ======================================================================
// RUN_MODE 3: Testing
// ======================================================================

#include <DFRobot_BMI160.h>

DFRobot_BMI160 bmi160;
const int8_t i2c_addr = 0x69;
void setup_testPose(){
  Serial.begin(9600);
  delay(100);
  
  //init the hardware bmin160  
  if (bmi160.softReset() != BMI160_OK){
    Serial.println("reset false");
    while(1);
  }
  
  //set and init the bmi160 i2c address
  if (bmi160.I2cInit(i2c_addr) != BMI160_OK){
    Serial.println("init false");
    while(1);
  }
}

void loop_testPose(){  
  int i = 0;
  int rslt;
  int16_t accelGyro[6]={0}; 
  
  //get both accel and gyro data from bmi160
  //parameter accelGyro is the pointer to store the data
  rslt = bmi160.getAccelGyroData(accelGyro);
  Serial.print("accelX:");
  Serial.print(accelGyro[3]);
  Serial.print(",accelY:");
  Serial.print(accelGyro[4]);
  Serial.print(",accelZ:");
  Serial.println(accelGyro[5]);
  delay(100);
}
  /*
  if(rslt == 0){
    for(i=0;i<6;i++){
      if (i<3){
        //the first three are gyro data
        Serial.print(accelGyro[i]*3.14/180.0);Serial.print(",");
      }else{
        //the following three data are accel data
        Serial.print(accelGyro[i]/16384.0);Serial.print(",");
      }
    }
    Serial.println();
  }else{
    Serial.println("err");
  }*/
