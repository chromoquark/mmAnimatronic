#ifndef button_h
#define button_h
//Configure the ATmega's port D4 to read a button that connects
//to ground when pressed
void setUpButton();

//Returns 0 if the button is unpressed, true otherwise
int button();

//Returns 1 if the button is in the pressed state of the state machine,
// and returns 0 otherwise.
int pressing();

//Advance the state machine one tick.
void buttonTick();

#endif

