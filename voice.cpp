/**This library is written to interface with the soundFX
 * board from ADAFruit.  Because that board has all the
 * necessary audio on it, all this library does is activate
 * the necessary pins.   
 */
#include <avr/io.h>
#include "voice.h"

/**This function initializes the voice box.  It sets the outputs
 * to 0 so that it begins not triggering a sound from the soundFX board
 */
void setUpVoice()
{
  //Initialize the three pins as high, since low initialization will play a tone
  PORTD |= 0x0D;
  //Enable pins D0, D1, and D3 as output
  DDRD |= 0x0D;
}

/**Set Pin D3 to 5V, disabling the soundFX board
 */
void disableAccelerometerSound()
  {PORTD |= 0x08;}

/**Set Pin D3 to 0V, enabling the soundFX board
 */
void enableAccelerometerSound()
  {PORTD &= 0xF7;}

/**Set Pin D2 to 5V, disabling the soundFX board
 */
void enableButtonSound()
  {PORTD &= 0xFB;}

/**Set Pin D2 to 0V, enabling the soundFX board
 */
void disableButtonSound()
  {PORTD |= 0x04;}

/**Set Pin D0 to 5V, disabling the soundFX board
 */
void enableTemperatureSound()
  {PORTD &= 0xFE;}

/**Set Pin D0 to 0V, enabling the soundFX board
 */
void disableTemperatureSound()
  {PORTD |= 0x01;}

void disableAudio()
  {
    disableAccelerometerSound();
    disableTemperatureSound();
    disableButtonSound();
  }


