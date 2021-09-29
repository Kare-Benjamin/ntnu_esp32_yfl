// Tell og vis antall passeringer på displayet
// Nils Kr. Rossing 08.07.20
// Gjennomgått og testet av Kåre-Benjamin H. Rørvik 15.07.20

#include <Arduino.h>
#include <TM1637Display.h>

const byte PIN_CLK = 32;   // Definer CLK pin og velg port D32
const byte PIN_DIO = 33;   // Definer DIO pin og velg port D33


TM1637Display display(PIN_CLK, PIN_DIO);
// Definer instansen display av typen SevenSegmentTM1637

int pinLED = 16;
int pinLDR = 36;
int verdiLDR;
int terskelLDR = 2000;
int antall = 0;

void setup() 
{
  Serial.begin(115200);
  display.setBrightness(7); // Setter lysstyrken på displayet. Fra 0 (min) til 7 (max).

  pinMode(pinLED, OUTPUT);
  digitalWrite(pinLED, HIGH);
  
  delay(500);
}

void loop() 
{
  verdiLDR = analogRead(pinLDR);
  
  if(verdiLDR < terskelLDR)
  {
    antall = antall + 1;
    Serial.print("Antall passeringer ");
    Serial.println(antall);
    while(verdiLDR < terskelLDR) 
    {
      verdiLDR = analogRead(pinLDR);
    }
  }
  display.showNumberDec(antall);        // Vis tall

}
