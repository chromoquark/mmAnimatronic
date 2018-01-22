/**This library is written to interface with three servos using PWM.  Two servos
 * are controlled using Timer 1, and their PWM inputs should be connected to
 * output A and output B respectively.  The third servo should be connected to 
 * Timer 2's output A. 
 */
#include <avr/io.h>
#include "servo.h"

#define MAX_TIMER1 20000 //This gives a frequency of 50Hz

/**Read the position of the right shoulder.  OCR1A is the register
 * that stores the pulse width for the PWM on Timer 1's output A.
 * The (0CR1A-410)/11 portion converts that PWM to an degree measurement
 * in the Servo's angle-space.  Subtracting this from 180 converts it so
 * that the angular position is consistent with the other shoulder servo,
 * even though they are facing different directions.
 */
int positionRightShoulder()
  {return 180-(OCR1A-410)/11;}

/**To stay within the bounds of natural motion for the Mickey plush,
 * first ensure that the indicated position will be between 45 and
 * 135.  If not, it is coerced to the closest limit in that range.
 * Then, the formula does the reverse of the conversion in
 * positionRightShoulder(), so the input is the angle measurement
 * in degrees
 */
void setRightShoulder(int pos)
{
  if(pos<45) pos=45;
  if(pos>135) pos=135;
  OCR1A=410+11*(180-pos);
}


/**Read the position of the right shoulder.  OCR1A is the register
 * that stores the pulse width for the PWM on Timer 1's output A.
 * The (0CR1A-400)/11 portion converts that PWM to an degree measurement
 * in the Servo's angle-space.
 */
int positionLeftShoulder()
  {return (OCR1B-400)/11;}
  
/**To stay within the bounds of natural motion for the Mickey plush,
 * first ensure that the indicated position will be between 45 and
 * 135.  If not, it is coerced to the closest limit in that range.
 * Then, the formula does the reverse of the conversion in
 * positionLeftShoulder(), so the input is the angle measurement
 * in degrees
 */
void setLeftShoulder(int pos)
{
  if(pos<45) pos=45;
  if(pos>135) pos=135;
  OCR1B=400+11*pos;
}

/**This function carries out integer division, but the quotient will
 * be represented by the nearest integer, rather than dropping the
 * fractional part, as is typically done in digital integer division
 */
int roundIntDivision(int dividend, int divisor)
{
  //Compute the quotient
  int ans=dividend/divisor;
  //If the remainder is greater than divisor/2, add 1 to ans and return
  if (2*dividend>=(2*ans+1)*divisor) return ans+1;
  //Otherwise, return answer
  else return ans;
}

/**For the positionSpine() and setSpine(int pos) functions, spineAngle
 * must be an stored separate from the PWM register.  This is because
 * the register has a precision that is too low to record differences
 * of one degree, so an increment of one degree repeatedly has no effect
 * positionSpine() simply returns that angle.
 */
int spineAngle;
int positionSpine()
  {return spineAngle;}



/**To stay within the bounds of natural motion for the Mickey plush,
 * first ensure that the indicated position will be between 45 and
 * 135.  If not, it is coerced to the closest limit in that range.
 * Then, the formula converts the spineAngle to the PWM space of the 
 * servo, which is PWM=10+spineAngle/6.
 */void setSpine(int pos)
{
  spineAngle=pos;
  if(pos<45) pos=45;
  if(pos>135) pos=135;
  OCR2A=10+roundIntDivision(pos,6);
}

/**This configures Timer 1 for PWM output on both the A and B outputs
 */
void configurePWM1()
{
  //Clear the timer settings
  TCCR1A &= 0x0C;     
  TCCR1B &= 0x20;     
  
  //Set the timer to work in fast PWM mode.  This version
  //of fast PWM sets ICR1 as the top value in the timer,
  //determining the period.  Then, OCR1A will set the pulse width
  TCCR1A |= (1<<WGM11);
  TCCR1B |= (1<<WGM13) | (1<<WGM12);

  //Set the clock for the timer to be the internal clock with no prescaler
  TCCR1B |= (1<<CS10);

  //Set Timer 1's output A to clear when the compare register is a match (ICR1)
  TCCR1A |= (1<<COM1A1) ;
  //Set Timer 1's output B to clear when the compare register is a match (ICR1)
  TCCR1A|=(1<<COM1B1);

  //Set the max timer value that gives the desired frequency
  ICR1 = MAX_TIMER1;

  //Set the two timer pins (OC1A & OC1B) to be outputs
  DDRB |= 0x07;

  //Set both angles to 90 degrees as a default position
  setLeftShoulder(90);
  setRightShoulder(90);

}

/**This configures Timer 2 for PWM output on the A output
 */
void configurePWM2()
{
  //Clear the servo settings to begin
  TCCR2A=0;
  TCCR2B=0;  
  // set PWM for 50% duty cycle
  TCCR2A |= (1 << COM2A1);

  // set none-inverting mode
  TCCR2A |= (1 << WGM21) | (1 << WGM20);
  // set fast PWM Mode
  TCCR2B |= (1 << CS22);
  // set prescaler to 8 and starts PWM
  //Set OC2A as output
  DDRB |= 0x08;

  //Set both angles to 90 degrees as a default position
  setSpine(spineAngle);
}

void configurePWM()
{
  configurePWM1();
  configurePWM2();
}






enum state_SPINE {init_SPINE, moveLeft_SPINE, moveRight_SPINE, holdStill_SPINE};
int spineTarget;
void setSpineTarget(int target){spineTarget=target;}
int spineAtTarget(){return spineTarget==positionSpine();}

void moveSpine()
{
  static state_SPINE state=init_SPINE;
  switch(state)
  {
    case init_SPINE:
      spineTarget=90;
      state= holdStill_SPINE;
      break;
    case moveLeft_SPINE:
      if(spineTarget>positionSpine()) state=moveLeft_SPINE;
      else if(spineTarget<positionSpine()) state=moveRight_SPINE;
      else state=holdStill_SPINE;
      break;
    case moveRight_SPINE:
      if(spineTarget>positionSpine()) state=moveLeft_SPINE;
      else if(spineTarget<positionSpine()) state=moveRight_SPINE;
      else state=holdStill_SPINE;
      break;
    case holdStill_SPINE:
      if(spineTarget>positionSpine()) state=moveLeft_SPINE;
      else if(spineTarget<positionSpine()) state=moveRight_SPINE;
      else state=holdStill_SPINE;
      break;
    default:
      state=init_SPINE;
      break;
  }

  switch(state)
  {
    case init_SPINE:
      break;
    case moveLeft_SPINE:
        if(spineTarget-positionSpine()<4) setSpine(spineTarget);
        else setSpine(positionSpine()-4);
        break;
    case moveRight_SPINE:
        if(positionSpine()-spineTarget<4) setSpine(spineTarget);
        else setSpine(positionSpine()+4);
        break;
    case holdStill_SPINE:
      break;
    default:
      break;
  }
}


enum state_LEFT {init_LEFT, moveUp_LEFT, moveDown_LEFT, holdStill_LEFT};
int leftTarget;
void setLeftTarget(int target){leftTarget=target;}
int leftAtTarget(){return leftTarget==positionLeftShoulder();}
void moveLeft()
{
  static state_LEFT state=init_LEFT;
  switch(state)
  {
    case init_LEFT:
      leftTarget=90;
      state= holdStill_LEFT;
      break;
    case moveUp_LEFT:
      if(leftTarget>positionLeftShoulder()) state=moveUp_LEFT;
      else if(leftTarget<positionLeftShoulder()) state=moveDown_LEFT;
      else state=holdStill_LEFT;
      break;
    case moveDown_LEFT:
      if(leftTarget>positionLeftShoulder()) state=moveUp_LEFT;
      else if(leftTarget<positionLeftShoulder()) state=moveDown_LEFT;
      else state=holdStill_LEFT;
      break;
    case holdStill_LEFT:
      if(leftTarget>positionLeftShoulder()) state=moveUp_LEFT;
      else if(leftTarget<positionLeftShoulder()) state=moveDown_LEFT;
      else state=holdStill_LEFT;
      break;
    default:
      state=init_LEFT;
      break;
  }

  switch(state)
  {
    case init_LEFT:
      break;
    case moveUp_LEFT:
        if(leftTarget-positionLeftShoulder()<4) setLeftShoulder(leftTarget);
        else setLeftShoulder(positionLeftShoulder()-4);
        break;
    case moveDown_LEFT:
        if(positionLeftShoulder()-leftTarget<4) setLeftShoulder(leftTarget);
        else setLeftShoulder(positionLeftShoulder()+4);
        break;
    case holdStill_LEFT:
      break;
    default:
      break;
  }
}



enum state_RIGHT {init_RIGHT, moveUp_RIGHT, moveDown_RIGHT, holdStill_RIGHT};
int rightTarget;
void setRightTarget(int target){rightTarget=target;}
int rightAtTarget(){return rightTarget==positionRightShoulder();}
void moveRight()
{
  static state_RIGHT state=init_RIGHT;
  switch(state)
  {
    case init_RIGHT:
      rightTarget=90;
      state=holdStill_RIGHT;
      break;
    case moveUp_RIGHT:
      if(rightTarget>positionRightShoulder()) state=moveUp_RIGHT;
      else if(rightTarget<positionRightShoulder()) state=moveDown_RIGHT;
      else state=holdStill_RIGHT;
      break;
    case moveDown_RIGHT:
      if(rightTarget>positionRightShoulder()) state=moveUp_RIGHT;
      else if(rightTarget<positionRightShoulder()) state=moveDown_RIGHT;
      else state=holdStill_RIGHT;
      break;
    case holdStill_RIGHT:
      if(rightTarget>positionRightShoulder()) state=moveUp_RIGHT;
      else if(rightTarget<positionRightShoulder()) state=moveDown_RIGHT;
      else state=holdStill_RIGHT;
      break;
    default:
      state=init_RIGHT;
      break;
  }

  switch(state)
  {
    case init_RIGHT:
      break;
    case moveUp_RIGHT:
        if(rightTarget-positionRightShoulder()<4) setRightShoulder(rightTarget);
        else setRightShoulder(positionRightShoulder()-4);
        break;
    case moveDown_RIGHT:
        if(positionRightShoulder()-rightTarget<4) setRightShoulder(rightTarget);
        else setRightShoulder(positionRightShoulder()+4);
        break;
    case holdStill_RIGHT:
      break;
    default:
      break;
  }
}

void servoTick()
{
  moveRight();
  moveLeft();
  moveSpine();
}




