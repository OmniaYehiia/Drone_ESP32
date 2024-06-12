#include "Arduino.h"
#include "imu.h"
#include "I2Cdev.h"
#include <MPU6050_6Axis_MotionApps20.h>


#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
#include "Wire.h"
#endif

MPU6050 mpu;

#define SCL 22 // pin 22 for I2C SCL for MPU6050
#define SDA 21 // pin 21 for I2C SDA for MPU6050
#define LED_PIN 2 // built-in LED of ESP32 dev board
bool blinkState = false;

// MPU control/status vars
uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)

// orientation/motion vars

// ================================================================
// ===               INTERRUPT DETECTION ROUTINE                ===
// ================================================================
volatile bool mpuInterrupt = false;     // indicates whether MPU interrupt pin has gone high
void dmpDataReady() {
  mpuInterrupt = true;
}
// ================================================================
// ===                      INITIAL SETUP                       ===
// ================================================================
void initIMU() {
  // join I2C bus (I2Cdev library doesn't do this automatically)
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
  Wire.begin(SDA, SCL);
  Wire.setClock(400000); // 400kHz I2C clock. Comment this line if having compilation difficulties
#elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
  Fastwire::setup(400, true);
#endif

  // initialize device
  Serial.println(F("Initializing I2C devices..."));
  mpu.initialize();
  // verify connection
  Serial.println(F("Testing device connections..."));
  Serial.println(mpu.testConnection() ? F("MPU6050 connection successful") : F("MPU6050 connection failed"));

  // load and configure the DMP
  Serial.println(F("Initializing DMP..."));
  devStatus = mpu.dmpInitialize();

  //           X Accel  Y Accel  Z Accel   X Gyro   Y Gyro   Z Gyro
  //OFFSETS     -332,    2848,    1596,     32,      -23,     25
  // supply your own gyro offsets here, scaled for min sensitivit
  mpu.setXAccelOffset(-332);
  mpu.setYAccelOffset(2848);
  mpu.setZAccelOffset(1596);
  mpu.setXGyroOffset(32);
  mpu.setYGyroOffset(-23);
  mpu.setZGyroOffset(25);

  // make sure it worked (returns 0 if so)
  if (devStatus == 0) {
    // Calibration Time: generate offsets and calibrate our MPU6050
    mpu.CalibrateAccel(6);
    mpu.CalibrateGyro(6);
    mpu.PrintActiveOffsets();
    // turn on the DMP, now that it's ready
    Serial.println(F("Enabling DMP..."));
    mpu.setDMPEnabled(true);
    // enable Arduino interrupt detection
  }
  // configure LED for output
  pinMode(LED_PIN, OUTPUT);
}

void getRotation(float* x, float* y, float* z) {
  int16_t gx, gy, gz;
  mpu.getRotation(&gx, &gy, &gz);
  *x = gx / 32.8;
  *y = gy / 32.8;
  *z = gz / 32.8;
}

