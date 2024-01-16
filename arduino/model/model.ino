#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

#include <TensorFlowLite_ESP32.h>
#include <tensorflow/lite/micro/all_ops_resolver.h>
#include <tensorflow/lite/micro/micro_error_reporter.h>
#include <tensorflow/lite/micro/micro_interpreter.h>
#include <tensorflow/lite/schema/schema_generated.h>


#include "model.h"

Adafruit_MPU6050 mpu;
const float accelerationThreshold = 40; // threshold of significant in G's
const int numSamples = 100;

int samplesRead = numSamples;

// global variables used for TensorFlow Lite (Micro)
tflite::MicroErrorReporter tflErrorReporter;

// pull in all the TFLM ops, you can remove this line and
// only pull in the TFLM ops you need, if would like to reduce
// the compiled size of the sketch.
tflite::AllOpsResolver tflOpsResolver;

const tflite::Model* tflModel = nullptr;
tflite::MicroInterpreter* tflInterpreter = nullptr;
TfLiteTensor* tflInputTensor = nullptr;
TfLiteTensor* tflOutputTensor = nullptr;

// Create a static memory buffer for TFLM, the size may need to
// be adjusted based on the model you are using
constexpr int tensorArenaSize = 8 * 1024;
byte tensorArena[tensorArenaSize] __attribute__((aligned(16)));

// array to map gesture index to a name
const char* GESTURES[] = {
  "down-to-up",
  "left-to-right"
};

#define NUM_GESTURES (sizeof(GESTURES) / sizeof(GESTURES[0]))

void setup() {
  Serial.begin(115200);
  while (!Serial);

  // // initialize the IMU
  // if (!IMU.begin()) {
  //   Serial.println("Failed to initialize IMU!");
  //   while (1);
  // }
    if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }

  // print out the samples rates of the IMUs
  // Serial.print("Accelerometer sample rate = ");
  // Serial.print(IMU.accelerationSampleRate());
  // Serial.println(" Hz");
  // Serial.print("Gyroscope sample rate = ");
  // Serial.print(IMU.gyroscopeSampleRate());
  // Serial.println(" Hz");

  // Serial.println();
  mpu.setAccelerometerRange(MPU6050_RANGE_16_G);
  // Serial.println(mpu.getAccelerometerRange());
  mpu.setGyroRange(MPU6050_RANGE_250_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  Serial.println("");
  
  delay(100);

  // get the TFL representation of the model byte array
  tflModel = tflite::GetModel(model);
  if (tflModel->version() != TFLITE_SCHEMA_VERSION) {
    Serial.println("Model schema mismatch!");
    while (1);
  }

  // Create an interpreter to run the model
  tflInterpreter = new tflite::MicroInterpreter(tflModel, tflOpsResolver, tensorArena, tensorArenaSize, &tflErrorReporter);

  // Allocate memory for the model's input and output tensors
  tflInterpreter->AllocateTensors();

  // Get pointers for the model's input and output tensors
  tflInputTensor = tflInterpreter->input(0);
  tflOutputTensor = tflInterpreter->output(0);
}

void loop() {
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
    // check if new acceleration AND gyroscope data is available
    // if (IMU.accelerationAvailable() && IMU.gyroscopeAvailable()) {
    //   // read the acceleration and gyroscope data
    //   IMU.readAcceleration(aX, aY, aZ);
    //   IMU.readGyroscope(gX, gY, gZ);
      sensors_event_t a, g, temp;
      mpu.getEvent(&a, &g, &temp);
      aX = a.acceleration.x;
      aY = a.acceleration.y;
      aZ = a.acceleration.z;
      gX = g.gyro.x;
      gY = g.gyro.y;
      gZ = g.gyro.z;

      // normalize the IMU data between 0 to 1 and store in the model's
      // input tensor
      tflInputTensor->data.f[samplesRead * 6 + 0] = (aX);
      tflInputTensor->data.f[samplesRead * 6 + 1] = (aY);
      tflInputTensor->data.f[samplesRead * 6 + 2] = (aZ);
      tflInputTensor->data.f[samplesRead * 6 + 3] = (gX);
      tflInputTensor->data.f[samplesRead * 6 + 4] = (gY);
      tflInputTensor->data.f[samplesRead * 6 + 5] = (gZ);

      samplesRead++;

      if (samplesRead == numSamples) {
        // Run inferencing
        TfLiteStatus invokeStatus = tflInterpreter->Invoke();
        if (invokeStatus != kTfLiteOk) {
          Serial.println("Invoke failed!");
          while (1);
          return;
        }

        // Loop through the output tensor values from the model
        for (int i = 0; i < NUM_GESTURES; i++) {
          Serial.print(GESTURES[i]);
          Serial.print(": ");
          Serial.println(tflOutputTensor->data.f[i], 6);
        }
        Serial.println();
      }
    }
}
