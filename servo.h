/**This library is written to interface with three servos using PWM.  Two servos
 * are controlled using Timer 1, and their PWM inputs should be connected to
 * output A and output B respectively.  The third servo should be connected to 
 * Timer 2's output A. 
 */
//Configure the three servos
void configurePWM1();
void configurePWM2();

//Returns the position of the left shoulder in degrees
int positionLeftShoulder();

//Returns the position of the right shoulder in degrees
int positionRightShoulder();

//Returns the position of the spine in degrees
int positionSpine();

//Sets the position of the left shoulder.  The argument should be in degrees
void setLeftShoulder(int pos);

//Sets the position of the right shoulder.  The argument should be in degrees
void setRightShoulder(int pos);

//Sets the position of the spine.  The argument should be in degrees
void setSpine(int pos);


//set__Target sets a target for gradual movement (in degrees)
//__AtTarget returns 1 if the target is reached, 0 otherwise
//move__ advances the item slightly; each represents a different state machine,
//and these state machines are called by servoTick()

void setSpineTarget(int target);
int spineAtTarget();
void moveSpine();

void setLeftTarget(int target);
int leftAtTarget();
void moveLeft();

void setRightTarget(int target);
int rightAtTarget();
void moveRight();


//Advances state machine one tick (calling the three move__ functions above)
void servoTick();


