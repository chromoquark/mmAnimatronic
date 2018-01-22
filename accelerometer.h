//Prepare the accelerometer for use
void setUpAccel();

//Read the acceleration, calibrated to remove
//the pull of gravity measured during setUpAccel
unsigned int accel();

//Advance the state machine one tick.
void accelTick();

//Returns 1 if the state machine is in the accelerating
//state, 0 otherwise
int accelerating();

