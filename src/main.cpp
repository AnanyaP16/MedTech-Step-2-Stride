/*
  MIT License

  Copyright (c) 2019 ElectronicCats

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.

  MPU6050 Raw

  A code for obtaining raw data from the MPU6050 module with the option to
  modify the data output format.

  Find the full MPU6050 library documentation here:
  https://github.com/ElectronicCats/mpu6050/wiki
*/
#include <Wire.h>
#include "I2Cdev.h"
#include "MPU6050.h"

/* MPU6050 default I2C address is 0x68*/
MPU6050 mpu;
//MPU6050 mpu(0x69);         //Use for AD0 high
//MPU6050 mpu(0x68, &Wire1); //Use for AD0 low, but 2nd Wire (TWI/I2C) object.

/* OUTPUT FORMAT DEFINITION----------------------------------------------------------------------------------
- Use "OUTPUT_READABLE_ACCELGYRO" if you want to see a tab-separated list of the accel 
X/Y/Z and gyro X/Y/Z values in decimal. Easy to read, but not so easy to parse, and slower over UART.

- Use "OUTPUT_BINARY_ACCELGYRO" to send all 6 axes of data as 16-bit binary, one right after the other. 
As fast as possible without compression or data loss, easy to parse, but impossible to read for a human. 
This output format is used as an output.
--------------------------------------------------------------------------------------------------------------*/ 
#define OUTPUT_READABLE_ACCELGYRO
//#define OUTPUT_BINARY_ACCELGYRO

int16_t ax, ay, az;
int16_t gx, gy, gz;
bool blinkState;

// NeoPixel Ring simple sketch (c) 2013 Shae Erisson
// released under the GPLv3 license to match the rest of the AdaFruit NeoPixel library

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1
#define PIN            6

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS      8

// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
// Note that for older NeoPixel strips you might need to change the third parameter--see the strandtest
// example for more information on possible values.
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

int delayval = 500; // delay for half a second

//L: 
int steps;
int goal = 12;



bool stepDetected = false;
float threshold = 15000;   // tune this if needed
unsigned long lastStepTime = 0;
int debounceTime = 600;    // ms between steps


//L: complementary filter variables
float a = 0.9; //weight for gyro (typical)
float prevAccel = 0;
float angle = 0;
float gyroAngle =0;
float dt;
unsigned long prevTime = 0;
float stepThreshold = 15; //18 degrees is the typical angle 
float prevAngle = 0;
float currentAccel = 0;
float gyroBiasY = 0;
float startAngle = 0;
bool initialized = false;

void setup() {
  /*--Start I2C interface--*/
  #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    Wire.begin(); 
  #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
    Fastwire::setup(400, true);
  #endif

  Serial.begin(38400); //Initializate Serial wo work well at 8MHz/16MHz


 // Serial.println("Connected to VS");
  /*Initialize device and check connection*/ 
 // Serial.println("Initializing MPU...");
  mpu.initialize();
 // Serial.println("Testing MPU6050 connection...");
  if(mpu.testConnection() ==  false){
 //   Serial.println("MPU6050 connection failed");
    while(true);
  }
  else{
 //   Serial.println("MPU6050 connection successful");
  }

  /* Use the code below to change accel/gyro offset values. Use MPU6050_Zero to obtain the recommended offsets */ 
  //Serial.println("Updating internal sensor offsets...\n");
  mpu.setXAccelOffset(0); //Set your accelerometer offset for axis X
  mpu.setYAccelOffset(0); //Set your accelerometer offset for axis Y
  mpu.setZAccelOffset(0); //Set your accelerometer offset for axis Z
  mpu.setXGyroOffset(0);  //Set your gyro offset for axis X
  mpu.setYGyroOffset(0);  //Set your gyro offset for axis Y
  mpu.setZGyroOffset(0);  //Set your gyro offset for axis Z

  angle = atan2(-ax, ay) * 180.0 / PI;   // initial pitch
  gyroBiasY = 0;
  mpu.setFullScaleAccelRange(MPU6050_ACCEL_FS_8);
  for(int i=0;i<500;i++){
      mpu.getMotion6(&ax,&ay,&az,&gx,&gy,&gz);
      gyroBiasY += gy;
      delay(3);
  }
  gyroBiasY /= 500.0;
  /*Print the defined offsets
  Serial.print("\t");
  Serial.print(mpu.getXAccelOffset());
  Serial.print("\t");
  Serial.print(mpu.getYAccelOffset()); 
  Serial.print("\t");
  Serial.print(mpu.getZAccelOffset());
  Serial.print("\t");
  Serial.print(mpu.getXGyroOffset()); 
  Serial.print("\t");
  Serial.print(mpu.getYGyroOffset());
  Serial.print("\t");
  Serial.print(mpu.getZGyroOffset());
  Serial.print("\n");
*/
  //L:
  steps = 0;
  //LED initalization 
  strip.begin();
  strip.show();

}
void updateProgress(int progress, uint32_t color){

  for(int i = 0; i < NUMPIXELS; i++){

    if(i < progress){
      strip.setPixelColor(i, color);
    }
    else{
      strip.setPixelColor(i, 0); // OFF
    }

  }

  strip.show();
}

void loop() {
  //L: probably where to get data for condition such as (ax, ay, az could also calculate roll, pitch, and yaw)
  /* Read raw accel/gyro data from the module. Other methods commented*/
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  //Serial.print("AX: "); Serial.print(ax);
  //Serial.print("  AY: "); Serial.print(ay);
  //Serial.print("  AZ: "); Serial.println(az);
  //delay(50);
  //float currentAccel = abs(az);
 // Serial.print("Angle: ");
 // Serial.println(angle);
  
  //acceleration angle (pitch) calculation
  float accelPitch = atan2(-ax, ay) * 180.0/PI;
  
  // gyro angle calculation -- rotation around y axis
  float gyroRate = (gy-gyroBiasY)/ 131.0; // Convert gyro raw value to degrees/s 
  
  // time step
  unsigned long currentTime = millis();
  dt = (currentTime - prevTime) / 1000.0;
  prevTime = currentTime;
  
   if (!initialized) {
        startAngle = accelPitch;
        angle = 0; // relative angle
        initialized = true;
  }
  // complementary filter: angle= a(prev_angle + gyro*dt)+(1-a)*accel_angle
  angle = a * (angle + gyroRate * dt) + (1 - a) * (accelPitch- startAngle);
  if(!stepDetected && -angle<5){
    stepDetected= true;
  }

  if (stepDetected && -angle > stepThreshold && prevAngle < stepThreshold){
    unsigned long timeSinceLastStep = millis() - lastStepTime;
    //Serial.println(timeSinceLastStep);
    if(timeSinceLastStep > debounceTime) {
      //Serial.println(millis()-lastStepTime);
      steps++;
     // Serial.print("Step detected. Total Steps:");
      //Serial.println(steps);
      int progress = (steps * 100) / goal;

      //Serial.print("Daily progress: ");
      //Serial.print(progress);
      //Serial.println("%");
      lastStepTime = millis();
      prevAngle = angle;
      stepDetected = false;
    }
  } 
  
 /*
  if(currentAccel > threshold && prevAccel <= threshold) {
    if(millis() -lastStepTime > debounceTime) {
      steps++;
      lastStepTime = millis();
      Serial.print("Step detected. Total Steps:");
      Serial.println(steps);
      int progress = (steps * 100) / goal;

      Serial.print("Daily progress: ");
      Serial.print(progress);
      Serial.println("%");
    }
  }*/
  
  if(steps== 0.25*goal){
    updateProgress(NUMPIXELS/4, strip.Color(255, 0 ,0));
  } else if(steps == 0.50*goal){
    updateProgress(NUMPIXELS/2, strip.Color(255, 165, 0));
  } else if(steps== 0.75*goal){
    updateProgress((3*NUMPIXELS)/4, strip.Color(255, 255,0));
  } else if (steps == goal) {
    //Serial.println("Goal Reached");
    updateProgress(NUMPIXELS, strip.Color(0, 255,0));
  }

  //prevAccel = currentAccel;
  prevAngle = angle;
  Serial.print(angle, 2);  // 2 decimal places
  Serial.print(",");  
  Serial.println(steps);
}
