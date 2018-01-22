/**This library implements a button.  It is set up so that the button
 * should be connected to port D4.  The input pin will be high when the
 * button is disconnected, so the button should connect the pin to ground
 */
#include <avr/io.h>
#include "button.h"

/**This function prepares the button for use by configureing the port
 */
void setUpButton()
{
  //This sets bit 4 of DDRD to 0, setting PortD as input
  DDRD &= 0xEF;
  //This enables the internal pull up resistor on the port.
  PORTD |= 0x10;
}

/**This returns 0 if the button is unpressed and true if pressed
 */
int button()
  {return !(PIND & 0x10);}
  

int pressed;

int pressing(){return pressed;}

enum button_ST {init_BUTTON,waitForStart_BUTTON,delayForNextSense_BUTTON};
void buttonTick()
{
  static button_ST state=init_BUTTON;
  static int delayCounter;
  switch(state)
  {
    case init_BUTTON:
      pressed=0;
      delayCounter=0;
      state=waitForStart_BUTTON;
      break;
    case waitForStart_BUTTON:
      if(button()>0)
      {
        state=delayForNextSense_BUTTON;
        pressed=1;
      }
      else pressed=0;
      break;
    case delayForNextSense_BUTTON:
      //Because the tick function should 60 times a second, counting to
      //160 gives a 2.5 second delay
      if(delayCounter>160)
      {
        state=waitForStart_BUTTON;
        delayCounter=0;
      }
      break;
    default:
      break;
  }
  switch(state)
  {
    case init_BUTTON:
      break;
    case waitForStart_BUTTON:
      break;
    case delayForNextSense_BUTTON:
      delayCounter++;
      break;
    default:
      break;
  }
}


