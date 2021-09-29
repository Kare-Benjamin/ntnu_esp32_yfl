// Tell antallet som går inn og ut og åpne og lukke bom
// Nils Kr. Rossing 06.07.20
// Kåre-Benjamin H. Rørvik 16.07.20

#include <Servo.h>

Servo myservo;  // Lag et objekt med navnet myservo av typen Servo

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
  digitalWrite(pinLED1, HIGH);
  digitalWrite(pinLED2, HIGH);

  myservo.attach(pinServo);         // Koble servoen til port D13 
  myservo.write(posLukket);               // Lukk bommen
  delay(100);
  
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
    servoFlagg = 1;
    delay(100);
  }
  else if(flaggSluse2 == 0 && flaggSluse1 == 0)
  {
    myservo.write(posLukket);              // Lukket bommen
    servoFlagg = 0;
    delay(100);
  }
  
  Serial.print("Det er foreløpig: ");
  Serial.print(antall);
  Serial.print(" inne i rommet ");
  
  if (servoFlagg == 1) 
  {
     Serial.println("og porten er åpen!");
  }
  
  if (servoFlagg == 0) 
    {
       Serial.println("og porten er stengt!");
    }
  
  
  //Kommandoer som kan brukes for feilsøking 
  //Serial.print("FlaggSluse1: ");
  //Serial.print(flaggSluse1);
  //Serial.print(", FlaggSluse2: ");
  //Serial.print(flaggSluse2);
}
