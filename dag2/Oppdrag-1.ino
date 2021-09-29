// Optisk portal-1

int pinLED = 16;
int pinLDR = 36;
int verdiLDR;
int terskelLDR = 500;
int antall = 0;

void setup() 
{
  pinMode(pinLED, OUTPUT);
  
  Serial.begin(9600);

  digitalWrite(pinLED, HIGH);
  delay(500);
}

void loop() 
{
  verdiLDR = analogRead(pinLDR);

  if(verdiLDR < terskelLDR)
  {
    Serial.println(verdiLDR);
    antall = antall + 1;
    
    while(verdiLDR < terskelLDR) 
    {
      verdiLDR = analogRead(pinLDR);
    }
  }
  
  Serial.println(antall);
  delay(100);
}
