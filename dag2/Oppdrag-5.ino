// Tell og vis antall passeringer på displayet samt åpne og lukke bommen
// Nils Kr. Rossing 05.07.20
// Kåre-Benjamin Hammervold Rørvik 16.7.2020

#include <TM1637Display.h>
#include <Servo.h>

const byte PIN_CLK = 32;   // Definer CLK pin og velg port D32
const byte PIN_DIO = 33;   // Definer DIO pin og velg port D33

TM1637Display display(PIN_CLK, PIN_DIO);// Lag et objekt med navnet display av typen TM1637Display
Servo myservo;  // Lag et objekt med navnet myservo av typen Servo

int pinLED = 16;
int pinLDR = 36;
int pinServo = 13;
int verdiLDR;
int terskelLDR = 2000;
int antall = 0;
int posLukket = 0;           // Angir posisjonen til servoen i lukket tilstand
int posAapen  = 90;          // Angir posisjon til servoen i åpen tilstand

void setup() 
{
  Serial.begin(115200);
  
  pinMode(pinLED, OUTPUT);
  pinMode(pinServo, OUTPUT);
  pinMode(PIN_CLK, OUTPUT);
  pinMode(PIN_DIO, OUTPUT);
  digitalWrite(pinLED, HIGH);
  
  myservo.attach(pinServo);         // Koble servoen til port D13 

  display.setBrightness(7); // Setter lysstyrken på displayet. Fra 0 (min) til 7 (max).
  display.showNumberDec(antall);  //Starter Displayet (får da 0).
  Serial.print("Antall passeringer ");
  Serial.println(antall);

  delay(500);
}

void loop() 
{
  verdiLDR = analogRead(pinLDR);
  myservo.write(posLukket);               // Lukk bommen
  delay(100);
  
  if(verdiLDR < terskelLDR)
  {
    antall = antall + 1;
    myservo.write(posAapen);              // Åpne bommen
    delay(100);
    display.showNumberDec(antall);          // Vis tall
    Serial.print("Antall passeringer ");
    Serial.println(antall);
    while(verdiLDR < terskelLDR) 
    {
      verdiLDR = analogRead(pinLDR);
      //delay(1000); //Dette kan brukes til å gjevne ut små bevegelser fra sensoren, og forholde døren åpen. Fjern // før "delay" for å bruke denne linjen.
    }
    delay(3500);
    myservo.write(posLukket);             // Lukk bommen
    delay(100);
  }
}
