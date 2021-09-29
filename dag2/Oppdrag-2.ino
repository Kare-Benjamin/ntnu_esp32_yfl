/* 
* I denne øvelsen skal vi teste displayet 
* og lære hvordan vi skriver ut tall til
* Nils Kr. Rossing 03.07.20 og Kåre-Benjamin H. Rørvik 15.07.20
*/

#include <Arduino.h>
#include <TM1637Display.h>

const byte PIN_CLK = 32;   // Definer CLK pin og velg port D32
const byte PIN_DIO = 33;   // Definer DIO pin og velg port D33


TM1637Display display(PIN_CLK, PIN_DIO);
// Definer instansen display av typen TM1637Display

int antall = 0; // Definer en variabel antall og sett den lik 0

void setup() 
{
  display.setBrightness(7); // Setter lysstyrken på displayet. Fra 0 (min) til 7 (max).
} 

void loop() 
{
  display.clear();                      // Tøm displayet for data
  display.showNumberDec(antall);        // Vis tall
  delay(1000);                          // Vent i 1000 millisek. (1 sek.)
  antall = antall + 1;                  // Øk telleren med 1
  while(antall == 11){}                 // Stopp og vent her for alltid
}
