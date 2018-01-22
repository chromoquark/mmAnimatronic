#include <avr/io.h>
#include "servo.h"
#include "thermometer.h"
#include "voice.h"
#include "button.h"
#include "accelerometer.h"
#include "control.h"

//The interrupt will function based on timer 0.void interruptSetUp()
void interruptSetUp()
{
  //Clear the timer settings
  TCCR0A=0;
  TCCR0B=0;
  
  //Enable timer.  This prescaler will cause it to tick about 32 times per second
  TCCR0B |= (1<<CS01)|(1<<CS00);
  
  //Enable interupts globally
  SREG |= (1<<7);
    
  //This enables Timer 0's interrupt for the overflow
  TIMSK0 |= (1<<TOIE0);
}

//This is the ISR function for the timer input
int readyToTick;
ISR(TIMER0_OVF_vect)
{
/**  static int t=0;
  t++;
  if(t>)
  {
    readyToTick=1;
    t=0;
  }*/
  readyToTick=1;
}

void mySetup()
{
  configurePWM1();
  configurePWM2();
  setUpTemperature();
  setUpButton();
  setUpVoice();
  setUpAccel();
  interruptSetUp();
}

void myLoop()
{
 static int s=0;
 if(readyToTick)
  {
//Enable the temperature sound to test speakers
//    enableTemperatureSound();
    buttonTick();
    accelTick();
    controlTick();
    if(accelerating()) enableAccelerometerSound();
    else disableAudio();
    thermTick();
    servoTick();
    readyToTick=0;
  }
}




int main(){
  enableAccelerometerSound();
  mySetup();
  while(true){myLoop();}
  return 0;
}
