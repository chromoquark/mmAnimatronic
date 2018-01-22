/**This library is written to interface with the soundFX
 * board from ADAFruit.  Because that board has all the
 * necessary audio on it, all this library does is activate
 * the necessary pins.   
 */

//This function initializes the voice box.  It sets the outputs
//to 0 so that it begins not triggering a sound from the soundFX board
void setUpVoice();

//Set Pin D3 to 5V, disabling the soundFX board
void disableAccelerometerSound();

//Set Pin D3 to 0V, enabling the soundFX board
void enableAccelerometerSound();

//Set Pin D2 to 5V, disabling the soundFX board
void enableButtonSound();

//Set Pin D2 to 0V, enabling the soundFX board
void disableButtonSound();

//Set Pin D0 to 5V, disabling the soundFX board
void enableTemperatureSound();

//Set Pin D0 to 0V, enabling the soundFX board
void disableTemperatureSound();

//Turn off all audio
void disableAudio();



