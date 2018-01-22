/**This library implements the I2C protocol for communication with the GY-521
 * breakout board.  This sensor is contains an accelerometer, gyroscope, and
 * thermometer.  The library is designed for use with a small Mickey Mouse
 * plush, and only uses the accelerometer, so the other features are not
 * implemented in this code.  The sensors address, as well as the addresses
 * for its more important registers are listed in the preprocessor commands.
 * Additionally, the standard accelerometer usage is for plus or minus 2g.
 * The function accel() gives the magnitude of the acceleration, disregarding
 * the direction.
 */
#include "Wire.h"
#include "accelerometer.h"
#include "servo.h"

//Register adresses
#define ACCEL_ADDR 0x68
#define ACCEL_XOUT_H 0X3B
#define ACCEL_XOUT_L 0x3C
#define ACCEL_YOUT_H 0x3D
#define ACCEL_YOUT_L 0x3E
#define ACCEL_ZOUT_H 0x3F
#define ACCEL_ZOUT_L 0x40

#define ACCEL_THRESHOLD >0xF900

//The number of bytes to read on a single I2C transaction
#define BYTES_PER_READ 2

/**Writes "message" to register "reg" of the GY-521
 */
void writeI2C(int reg, int message)

{
  Wire.beginTransmission(ACCEL_ADDR);
  Wire.send(reg);
  Wire.send(message);
  Wire.endTransmission();
}

/**Reads "BYTES_PER_READ" bytes from the register "reg"
 * The register value is returned as a signed int
 */
signed int readI2C(int reg)
{
  Wire.beginTransmission(ACCEL_ADDR);
  Wire.send(reg);
  Wire.endTransmission();
  
  Wire.beginTransmission(ACCEL_ADDR);
  Wire.requestFrom(ACCEL_ADDR,BYTES_PER_READ);
  signed int data=Wire.receive();
  Wire.endTransmission();
  
  return data;
}

/**These variables are set during the call of setUpAccel().
 * They are used as an offset in the rest of the measurements.
 * As long as the accelerometer is not rotated, they will represent
 * a gravitational offset, so that the exact value can be found
 */
signed int xBase;
signed int yBase;
signed int zBase;


void calibrate()
{
  xBase=readI2C(ACCEL_XOUT_H);
  yBase=readI2C(ACCEL_YOUT_H);
  zBase=readI2C(ACCEL_ZOUT_H);
}

/**Turns on the sensor to begin acceleration measurements.  Also
 * calibrates for gravity.
 */
void setUpAccel()
{
  //Set the sensitivity to 2g's
  writeI2C(0x1C,0x00);

  //Wake up the sensor
  writeI2C(0x6B,0x00);

  //Find the gravitational offset
  calibrate();
}

/**Reads the indicated axis (x=1, y=2, z=3) and
 * subtracts the gravitaitonal offset
 */
signed int readAxis(int axis)
{
  if(axis==1) return readI2C(ACCEL_XOUT_H);//-xBase;
  else if(axis==2) return readI2C(ACCEL_YOUT_H);//-yBase;
  else return readI2C(ACCEL_ZOUT_H);//-zBase;
}

/**My own square root function.
 */
unsigned int mySqrt(unsigned long in)
{
  //If the number is large, we set mult to 0xFFFF, essentially dropping
  //the least precise 16 bits, and taking the square root of the more
  //significant bits
  int mult;
  if(in>0xFFFFF) mult=0xFFFF;
  else mult=1;
  int x=in/mult;

  //Use a lookup table for input of 0 and 1, because these numbers produce
  //less accurate results with the main algorithm
  if(in==0) return 0;
  else if(in==1) return 1;
  else
  {
    //We initially guess that the square root is 1
    int guess=1;
    
    //We use determine log(in)/4.  Then, we can raise
    //e^2 to that power, and that should be the square root.
    //That's why we call the variable "exponenet"
    int exponent=1.93+3*in/100000-3*in*in/10000000000;
    
    //e^2 is approximately 7, so we raise it to the "exponent" power
    for(int i=0;i<exponent;i++) guess*=7;
    
    //The above has givenus a decent estimate for the square root.    
    //Now, we use a Taylor series error approximation to refine the guess
    for(int i=0;i<4;i++) guess+=(in-guess*guess)/2/guess;
    return guess;
  }
}

unsigned int accel()
{
  //Read the three axes
  signed int axX=readAxis(1);
  signed int axY=readAxis(2);
  signed int axZ=readAxis(3);
  
  //Return the quadrature sum of the axes measurements
  unsigned long sum=axX*axX+axY*axY+axZ*axZ;
  return mySqrt(sum);
}


int accelerated=0;

int accelerating(){return accelerated;}

enum accel_ST {init_ACCEL,waitForStart_ACCEL,delayForNextSense_ACCEL,recalibrate_ACCEL};
void accelTick()
{
  static accel_ST state=init_ACCEL;
  static int delayCounter;
  static int calibrationMeasurement;
  
  switch(state)
  {
    case init_ACCEL:
      accelerated=0;
      delayCounter=0;
      state=waitForStart_ACCEL;
      break;
    case waitForStart_ACCEL:
      if(accel()ACCEL_THRESHOLD)
      {
        state=delayForNextSense_ACCEL;
        accelerated=1;
      }
      else accelerated=0;
      break;
    case delayForNextSense_ACCEL:
      //Because the tick function should 60 times a second, counting to
      //320 gives a 5 second delay
      if(delayCounter>300)
      {
        state=recalibrate_ACCEL;
        delayCounter=0;
        calibrationMeasurement=accel();
      }
      break;
    case recalibrate_ACCEL:
      if(delayCounter>120)
      {
        delayCounter=0;
        int currentAccel=accel();
        if(calibrationMeasurement-currentAccel<200 & calibrationMeasurement-currentAccel>0) calibrate();
        else if(calibrationMeasurement-currentAccel>-200 & calibrationMeasurement-currentAccel<0) calibrate();
        state=waitForStart_ACCEL;
        
      }
      break;
    default:
      break;
  }
  switch(state)
  {
    case init_ACCEL:
      break;
    case waitForStart_ACCEL:
      break;
    case delayForNextSense_ACCEL:
      if(delayCounter>=7) accelerated=0;
      delayCounter++;
      break;
    case recalibrate_ACCEL:
      delayCounter++;
      break;
    default:
      break;
  }
}

