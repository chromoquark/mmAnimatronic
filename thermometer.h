/**This library is written to interface with a temperature sensor which produces
 * an analogue signal which is inversely proportional to the temperature.  The
 * signal is read through the ATMega328's ADC on pin ADC0
 */

//Configure the ADC to read the temperature's analog signal
void setUpTemperature();

//Returns a oltage which is inversely proportional to the temperature
int temperature();

//Returns 1 if the thermometer in the hand is warming up, 0 otherwise
int holdingHand();

//Advances the thermometer state machine one tick.
void thermTick();


