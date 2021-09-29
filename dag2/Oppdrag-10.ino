// Tell antallet som går inn og ut og åpne og lukke bom
// Nils Kr. Rossing 06.07.20
// Kåre-Benjamin Hammervold Rørvik 16.7.2020

#include <TM1637Display.h>
#include <Servo.h>

Servo myservo;  // Lag et objekt med navnet myservo av typen Servo

const byte PIN_CLK = 32;   // Definer CLK pin og velg port D32
const byte PIN_DIO = 33;   // Definer DIO pin og velg port D33

TM1637Display display(PIN_CLK, PIN_DIO);// Lag et objekt med navnet display av typen TM1637Display

int pinLED1 = 16;
int pinLDR1 = 36;
int verdiLDR1;
int terskelLDR1 = 1500;
int flaggSluse1 = 0;
int pinLED2 = 18;
int pinLDR2 = 39;
int verdiLDR2;
int terskelLDR2 = 1500;
int flaggSluse2 = 0;

int pinRele = 14;

int pinServo = 13;
int posLukket = 0;           // Angir posisjonen til servoen i lukket tilstand
int posAapen  = 90;          // Angir posisjon til servoen i åpen tilstand
int servoFlagg = 0;

int antall = 0;

void setup() 
{
  Serial.begin(115200);
  
  pinMode(pinLED1, OUTPUT);
  pinMode(pinLED2, OUTPUT);
  pinMode(pinServo, OUTPUT); 
  pinMode(pinRele, OUTPUT);
  digitalWrite(pinLED1, HIGH);
  digitalWrite(pinLED2, HIGH);

  myservo.attach(pinServo);         // Koble servoen til port D13 
  myservo.write(posLukket);               // Lukk bommen
  delay(100);

  display.setBrightness(7); // Setter lysstyrken på displayet. Fra 0 (min) til 7 (max).
  display.showNumberDec(antall);  //Starter Displayet (får da 0).

  delay(500);
}

void loop() 
{
  verdiLDR1 = analogRead(pinLDR1);
  verdiLDR2 = analogRead(pinLDR2);

  // Registrerer innganger
   
  if((verdiLDR1 < terskelLDR1) && (flaggSluse1 == 0) && (flaggSluse2 == 0))
  {
    while(verdiLDR1 < terskelLDR1) 
    {
      verdiLDR1 = analogRead(pinLDR1);
      delay(100);
    }
    flaggSluse1 = 1;
    delay(100);
  }

  if((verdiLDR2 < terskelLDR2) && (flaggSluse1 == 1) && (flaggSluse2 == 0))
  {
    while(verdiLDR2 < terskelLDR2) 
    {
      verdiLDR2 = analogRead(pinLDR2);
      delay(100);
    }
    flaggSluse1 = 0;
    antall = antall + 1;
    delay(100);
  }
  
  // registrerer utganger
  
  if((verdiLDR2 < terskelLDR2) && (flaggSluse2 == 0) && (flaggSluse1 == 0))
  {
    while(verdiLDR2 < terskelLDR2) 
    {
      verdiLDR2 = analogRead(pinLDR2);
      delay(100);
    }
    flaggSluse2 = 1;
    delay(100);
  }

  if((verdiLDR1 < terskelLDR1) && (flaggSluse2 == 1) && (flaggSluse1 == 0))
  {
    while(verdiLDR1 < terskelLDR1) 
    {
      verdiLDR1 = analogRead(pinLDR1);
      delay(100);
    }
    flaggSluse2 = 0;
    antall = antall - 1;
    delay(100);
  }

  //Styr servoen
  if(flaggSluse2 == 1 || flaggSluse1 == 1)
  {
    myservo.write(posAapen);              // Åpne bommen
    delay(100);
    servoFlagg = 1;
  }
  else if(flaggSluse2 == 0 && flaggSluse1 == 0)
  {
    myservo.write(posLukket);              // Lukket bommen
    delay(100);
    servoFlagg = 0;
  }
  
  display.showNumberDec(antall);          // Vis tall

  Serial.print("Det er foreløpig: ");
  Serial.print(antall);
  Serial.print(" inne i rommet ");
  
  if (servoFlagg == 1) 
  {
     Serial.print("og porten er åpen!");
  }
  
  if (servoFlagg == 0) 
  {
     Serial.print("og porten er stengt!");
  }

  if(antall <= 0)
  {
    digitalWrite(pinRele, LOW);
    Serial.println("Reléet er AV.");
  }
  
  if(antall >= 1)
  {
    digitalWrite(pinRele, HIGH);
    Serial.println("Reléet er PÅ.");
  }

  //Kommandoer som kan brukes for feilsøking 
  //Serial.print("FlaggSluse1: ");
  //Serial.print(flaggSluse1);
  //Serial.print(", FlaggSluse2: ");
  //Serial.print(flaggSluse2);
}
