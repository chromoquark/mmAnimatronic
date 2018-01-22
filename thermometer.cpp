/**This library is written to interface with a temperature sensor which produces
 * an analogue signal which is inversely proportional to the temperature.  The
 * signal is read through the ATMega328's ADC on pin ADC0
 */
#include <avr/io.h>
#include "thermometer.h"

/**Configure the ADC to read the temperature's analog signal
 */
void setUpTemperature()
{
  //Set REFS1 and REFS0 to 0 and 1 respectively so it will use AREF
  ADMUX|=1<<REFS0;
  //Select ADC0 input, and enable the pin as input
  ADMUX|=1<<MUX0;
  DDRC &= 0xFE;
  //Enable the ADC
  ADCSRA|=1<<ADEN;
  //Set ADC refresh frequency to 125kHz
  ADCSRA|=(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);
  //Set free running mode, so individual measurements don't need to be made
  ADCSRB|=(1<<ADTS2)|(1<<ADTS1)|(1<<ADTS0);
  //Begin Measuring
  ADCSRA|=(1<<ADSC);
}

/**Returns a oltage which is inversely proportional to the temperature
 */
int temperature()
  {return ADC;}
  
  



int handHeld;

int holdingHand(){return handHeld;}

enum therm_ST {init_THERMOMETER,waitForStart_THERMOMETER,delayForNextSense_THERMOMETER};
void thermTick()
{
  static therm_ST state=init_THERMOMETER;
  static int delayCounter;
  switch(state)
  {
    case init_THERMOMETER:
      handHeld=0;
      delayCounter=0;
      state=waitForStart_THERMOMETER;
      break;
    case waitForStart_THERMOMETER:
      if(temperature()>0)
      {
        state=delayForNextSense_THERMOMETER;
        handHeld=1;
      }
      else handHeld=0;
      break;
    case delayForNextSense_THERMOMETER:
      //Because the tick function should 60 times a second, counting to
      //160 gives a 2.5 second delay
      if(delayCounter>160)
      {
        state=waitForStart_THERMOMETER;
        delayCounter=0;
      }
      break;
    default:
      break;
  }
  switch(state)
  {
    case init_THERMOMETER:
      break;
    case waitForStart_THERMOMETER:
      break;
    case delayForNextSense_THERMOMETER:
      delayCounter++;
      break;
    default:
      break;
  }
}




