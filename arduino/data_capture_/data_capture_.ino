/*
  IMU Capture
  This example uses the on-board IMU to start reading acceleration and gyroscope
  data from on-board IMU and prints it to the Serial Monitor for one second
  when the significant motion is detected.
  You can also use the Serial Plotter to graph the data.
  The circuit:
  - Arduino Nano 33 BLE or Arduino Nano 33 BLE Sense board.
  Created by Don Coleman, Sandeep Mistry
  Modified by Dominic Pajak, Sandeep Mistry
  This example code is in the public domain.
*/

// #include <Arduino_LSM9DS1.h>

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

const float accelerationThreshold = 40; // threshold of significant in G's
const int numSamples = 100;


int samplesRead = numSamples;
Adafruit_MPU6050 mpu;

int max = 0;

void setup(void) {

   Serial.begin(115200);
  while (!Serial) {
    delay(10); // will pause Zero, Leonardo, etc until serial console opens
  }

  // Try to initialize!
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  // print the header
  Serial.println("aX,aY,aZ,gX,gY,gZ");

  mpu.setAccelerometerRange(MPU6050_RANGE_16_G);
  // Serial.println(mpu.getAccelerometerRange());
  mpu.setGyroRange(MPU6050_RANGE_250_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  Serial.println("");
  
  delay(100);
}


void loop() {
  // Serial.println("hi\n");
 
  float aX, aY, aZ, gX, gY, gZ;
  
  // wait for significant motion
  while (samplesRead == numSamples) {
      sensors_event_t a, g, temp;
      mpu.getEvent(&a, &g, &temp);
      aX = a.acceleration.x;
      aY = a.acceleration.y;
      aZ = a.acceleration.z;

      float aSum = fabs(aX) + fabs(aY) + fabs(aZ);
      // Serial.println(aSum);
      // if(aSum > 50) {Serial.print("MOTION DETECTED : "); Serial.println(aSum); delay(1000); }
      

      if (aSum >= accelerationThreshold) {
        // reset the sample read count
        samplesRead = 0;
        break;
      }
  }

  // check if the all the required samples have been read since
  // the last time the significant motion was detected
  while (samplesRead < numSamples) {
      sensors_event_t a, g, temp;
      mpu.getEvent(&a, &g, &temp);

      samplesRead++;

      // print the data in CSV format
      Serial.print(a.acceleration.x, 3);
      Serial.print(',');
      Serial.print(a.acceleration.y, 3);
      Serial.print(',');
      Serial.print(a.acceleration.z, 3);
      Serial.print(',');
      Serial.print(g.gyro.x, 3);
      Serial.print(',');
      Serial.print(g.gyro.y, 3);
      Serial.print(',');
      Serial.print(g.gyro.z, 3);
      Serial.println();

      if (samplesRead == numSamples) {
        // add an empty line if it's the last sample
        Serial.println("--------------------------------------------------------------------------------------------");
      }
    }
}
