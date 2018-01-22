#include <avr/io.h>
#include "button.h"
#include "thermometer.h"
#include "accelerometer.h"
#include "servo.h"
#include "voice.h"

//This returns a random number based on Timer1.  The number will be between 0 and 5
//(though 5 will be highly unprobable).The implementation assumes that Timer1 has
//already been configured, and that it resets on compare at 20,000
int getTargetAngle()
{
  int x=TCNT1/4000;
  if(x==0) return 45;
  else if(x==1) return 70;
  else if(x==2) return 90;
  else if(x==3) return 110;
  else return 135;
}

void setTargetAngles()
{
  if(!leftAtTarget()) setLeftTarget(getTargetAngle());
  if(!rightAtTarget()) setRightTarget(getTargetAngle());
  if(!spineAtTarget()) setSpineTarget(getTargetAngle());
}
enum control_ST {init_CONTROL,sense_CONTROL,soundDisable_CONTROL,setMove_CONTROL,waitForMotion_CONTROL,delaySense_CONTROL};

void controlTick()
{
  static control_ST state=init_CONTROL;
  static int moveNumber=0;
  static int delayCount=0;
  
  switch(state)
  {
  case init_CONTROL:
    state=sense_CONTROL;
    break;
  case sense_CONTROL:
    if(accelerating())
    {
      enableAccelerometerSound();
      state=soundDisable_CONTROL;
    }
    else if(pressing())
    {
      enableButtonSound();
      state=soundDisable_CONTROL;
    }
    else state=sense_CONTROL;
    break;
  case soundDisable_CONTROL:
    if(delayCount>=7)
    {
      state=setMove_CONTROL;
      disableAudio();
      delayCount=0;
    }
    break;
  case setMove_CONTROL:
    state=waitForMotion_CONTROL;
    break;
  case waitForMotion_CONTROL:
    if(moveNumber>=6)
    {
      state=delaySense_CONTROL;
      moveNumber=0;
    }
    else if(spineAtTarget()|leftAtTarget()|rightAtTarget()) state=setMove_CONTROL;
    break;
  case delaySense_CONTROL:
    if(delayCount>=160)
    {
      state=sense_CONTROL;
      delayCount=0;
    }
    break;
  default:
    break;
  }


  switch(state)
  {
  case init_CONTROL:
    break;
  case sense_CONTROL:
    break;
  case soundDisable_CONTROL:
    delayCount++;
    break;
  case setMove_CONTROL:
    moveNumber++;
    setTargetAngles();
    break;
  case waitForMotion_CONTROL:
    break;
  case delaySense_CONTROL:
    delayCount++;
    break;
  default:
    break;
  }
}



