// Tell antallet som går inn og ut
// Nils Kr. Rossing 05.07.20
// Kåre-Benjamin Hammervold Rørvik 16.7.2020

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

int antall = 0;

void setup() 
{
  Serial.begin(115200);
  
  pinMode(pinLED1, OUTPUT);
  pinMode(pinLED2, OUTPUT);
  digitalWrite(pinLED1, HIGH);
  digitalWrite(pinLED2, HIGH);
  
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
  // Skriv ut antallet til monitor
  Serial.print("Det er foreløpig: ");
  Serial.print(antall);
  Serial.println(" inne i rommet.");
}
