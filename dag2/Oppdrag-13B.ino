// Total Løsning med RFID implementert.
// Kåre-Benjamin Hammervold Rørvik og Nils Kr. Rossing 23.7.2020

#include <SPI.h>
#include <TM1637Display.h>
#include <MFRC522.h>
#include <Servo.h>

//Setter alle PIN som blir brukt på ESP32:
const int pinRele = 14;
const int pinServo = 13;
const int pinLED2 = 18;
const int pinLDR2 = 39;
const int pinLED1 = 16;
const int pinLDR1 = 36;
const int SS_PIN = 21;    // Slave hos RFID.
const int RST_PIN = 22;   // Reset hos RFID.
const int PIN_CLK = 32;   // CLK hos Displayet.
const int PIN_DIO = 33;   // DIO hos Displayet.

// Lager objekter
TM1637Display display(PIN_CLK, PIN_DIO);// Lag et objekt med navnet display av typen TM1637Display.
MFRC522 mfrc522(SS_PIN, RST_PIN); // Oppretter instansen mfrc522 av typen MFRC522.
Servo myservo;                    // Lag et objekt med navnet myservo.
                  // Hele 12 slike objekter kan lages ved hvert kort.

// Her lagres kortet sitt UID i 4 variabler. Disse brukes til å identifisere et RFID kort.
// Det kan legges inn 2 kort, hvor begge kan brukes lagret.

// RFID-Kort 0
int ID0_0 = 124;    // Erstatt "124" med avlest byte 0.
int ID0_1 = 17;    // Erstatt "17" med avlest byte 0.
int ID0_2 = 94;   // Erstatt "94" med avlest byte 0.
int ID0_3 = 116;    // Erstatt "116" med avlest byte 0.

//RFID-Kort 1
int ID1_0 = 185;    // Erstatt "185" med avlest byte 0.
int ID1_1 = 191  ;   // Erstatt "191" med avlest byte 1.
int ID1_2 = 45;     // Erstatt "45" med avlest byte 2.
int ID1_3 = 86;    // Erstatt "86" med avlest byte 3.

// LDR Variabler
int verdiLDR1;            //Verdien som leses ut av LDR1.
int terskelLDR1 = 1500;   //Terskelen for målingene hos LDR1.
int verdiLDR2;            //Verdien som leses ut av LDR2.
int terskelLDR2 = 1500;   //Terskelen for målingene hos LDR2.

// Flagg som styrer programflyten, disse stilles til 0 eller 1 for å iverksette endringer i programmet.
int flaggSluse1 = 0;         // Satt til 1 når sluse 1 skal til å åpne.
int flaggSluse2 = 0;         // Satt til 1 når sluse 2 skal til å åpne.
int RFIDflagg = 0;           // Satt til 1 når nytt kort er lest.
int nokkelFlagg = 0;         // Satt til 1 når RFID er lest og godkjent.
int releFlagg = 0;           // Satt til 1 når reléet er aktivt.
int servoFlagg = 0;          // Satt til 1 når porten er åpen.

// Diverse variabler
int antall = 0;              // Antall personer i rommet.
int posLukket = 0;           // Angir posisjonen til servoen i lukket tilstand.
int posAapen  = 90;          // Angir posisjon til servoen i åpen tilstand.

// Her er variablene som definerer ordene som blir skrevet på 7-segment displayet.
// De er definert for å danne ønskede bokstaver på 7-segment displayet.
// Bokstavene er laget ved å skru på eller av ønskede segmenter ...
// i displayet for å danne bokstaver.

//
//      A
//     ---
//  F |   | B
//     -G-
//  E |   | C
//     ---
//      D

// Lager et array av typen byte (byte = 8bit).
// Dette brukes ofte for å styre 7-segment display.
// Merk: Displayene har enten 7 eller 8 LED segment, som akkurat går opp i en byte. 
const byte OPEN[] = {
  SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F,   // O
  SEG_A | SEG_B | SEG_E | SEG_F | SEG_G,           // P
  SEG_A | SEG_D | SEG_E | SEG_F | SEG_G,           // E
  SEG_C | SEG_E | SEG_G,                           // n
  };

const byte Err[] = {
  SEG_A | SEG_D | SEG_E | SEG_F | SEG_G,            // E
  SEG_E | SEG_G,                                    // r
  SEG_E | SEG_G,                                    // r
  0,                                                // Blir ikke belyst...
  };

void setup() 
{
programInit();
}
void loop() 
{
  lesLDR();
  lesOgGodkjen();
  tellPasseringer();
  servoStyring();
  skrivTilDisplay();
  styrRele();
  skrivTilMonitor();
}

void lesLDR()
// Her leses ut verdiene på LDR1 og LDR2.
// Disse lagres til verdiLDR1 og verdiLDR2.
{
  verdiLDR1 = analogRead(pinLDR1);
  verdiLDR2 = analogRead(pinLDR2);
}

void programInit()
// Inneholder initialiseringen av programmet.
// Starter kommunikasjon med terminalen, SPI, RFID,
// pinnene sin orientering (de er INPUT per default), 
// starter servoen og displayet.
{
  Serial.begin(115200); // Starter seriell kommunikasjon med Baud rate på 115200 bit/sekund (husk å endre i terminalen).
  SPI.begin();          // Starter SPI - Initierer SPI-buss som kommuniserer mot RFID RC522.
  mfrc522.PCD_Init();   // Starter RFID leseren.

  pinMode(pinLED1, OUTPUT);
  pinMode(pinLED2, OUTPUT);
  pinMode(pinServo, OUTPUT); 
  pinMode(pinRele, OUTPUT);
  digitalWrite(pinLED1, HIGH);
  digitalWrite(pinLED2, HIGH);

  myservo.attach(pinServo);         // Koble servoen til port D13 .
  myservo.write(posLukket);         // Lukk bommen.
  delay(100);

  display.setBrightness(7);   // Setter lysstyrken på displayet. Fra 0 (min) til 7 (max).
  Serial.println("**Klar til å lese kort**");
  display.showNumberDec(antall);  //Starter Displayet (får da 0).

  delay(500);
}

void lesOgGodkjen()
// Leser RFID-kort, skriver ut til monitor og sjekker om kortet er registrert.
// Det avleste kortet blir sjekket opp mot registrerte kort i minnet (ID1 og ID2).
// Det vil deretter å indikere om kortet som er tilkoblet har tilgang.

// Styres internt av RFIDflagg (1 når kort er kompatibelt og avlest).
// Påvirker resten av programmet med nokkelFlagg (1 låser opp porten for utsiden).
{  
  // Identifiser kort og les
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) 
    { 
      // Skriv ut kortinformasjon
      Serial.print("Identifisert kort med ID: ");
      Serial.print(mfrc522.uid.uidByte[0]); Serial.print(" ");  // Skriv ut ID byte 0.
      Serial.print(mfrc522.uid.uidByte[1]); Serial.print(" ");  // Skriv ut ID byte 1.
      Serial.print(mfrc522.uid.uidByte[2]); Serial.print(" ");  // Skriv ut ID byte 2.
      Serial.print(mfrc522.uid.uidByte[3]); Serial.println(" ");// Skriv ut ID byte 3.
      mfrc522.PICC_HaltA(); // Stopp dersom kortet er uendret. 
      RFIDflagg = 1;        // Sett et flagg som indikerer at nytt kort er lest.
    }

  // Sjekk at kortet er godkjent og ønsk velkommen inn
  if(((mfrc522.uid.uidByte[0]==ID0_0)|| (mfrc522.uid.uidByte[0]==ID1_0))&&
     ((mfrc522.uid.uidByte[1]==ID0_1)|| (mfrc522.uid.uidByte[1]==ID1_1)) &&
     ((mfrc522.uid.uidByte[2]==ID0_2)|| (mfrc522.uid.uidByte[2]==ID1_2)) &&
     ((mfrc522.uid.uidByte[3]==ID0_3)|| (mfrc522.uid.uidByte[3]==ID1_3)) && RFIDflagg == 1)
     {
     if (RFIDflagg = 1) 
     {
        nokkelFlagg = 1;
        Serial.println("** Kort er registrert og godkjent! **");
     }
     RFIDflagg = 0;  // Resett flagg som forteller at kortet er sjekket.
     }
}

void tellPasseringer()
// Leser av LDR for å vurdere om porten skal åpnes eller lukkes.
// Teller også opp eller ned antall variablen.
// nokkelFlagg må være satt til 1 for at porten skal kunne åpnes via LDR1 (utsiden).
// Immidlertid kan porten åpnes åpnes fra innsiden uten at nokkelFlagget er satt ...
// altså porten kan åpnes fra innsiden uten at døren er låst opp.
{
  // Registrerer innganger
   
  if((verdiLDR1 < terskelLDR1) && (flaggSluse1 == 0) && (flaggSluse2 == 0) && (nokkelFlagg == 1))
  {
    while(verdiLDR1 < terskelLDR1) 
    {
      verdiLDR1 = analogRead(pinLDR1);
      delay(100);
    }
    flaggSluse1 = 1;
    delay(100);
  }

  if((verdiLDR2 < terskelLDR2) && (flaggSluse1 == 1) && (flaggSluse2 == 0) && (nokkelFlagg == 1))
  {
    while(verdiLDR2 < terskelLDR2) 
    {
      verdiLDR2 = analogRead(pinLDR2);
      delay(100);
    }
    flaggSluse1 = 0;
    antall = antall + 1;
    RFIDflagg = 0;
    nokkelFlagg = 0;
    Serial.println("Brukeren har åpnet døren, og adgangen er brukt opp!");
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
    RFIDflagg = 0;
    nokkelFlagg = 0;
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
}

void servoStyring()
// Styrer direkte servoen avhengig av flaggSluse variablene.
// Åpner og lukker bommen med antall grader gitt av posAapen og posLukket.
{
  //Styr servoen
  if((flaggSluse2 == 1 || flaggSluse1 == 1))
  {
    myservo.write(posAapen);  // Åpne bommen.
    delay(100);
    servoFlagg = 1;
  }
  else if(flaggSluse2 == 0 && flaggSluse1 == 0)
  {
    myservo.write(posLukket); // Lukk bommen
    delay(100);
    servoFlagg = 0;
  }
}

void skrivTilDisplay()
// Skriver til Displayet avhengig av teller variablen.
// Setter også OPEN når døren er låst opp samt Err når et kort uten tilgang ble brukt.
{
  if(nokkelFlagg == 1 && !(flaggSluse2 == 1 && flaggSluse1 == 0))
  {
    display.setSegments(OPEN);
  }

  else{
    if(RFIDflagg == 1)
    {
      display.setSegments(Err);
    }
    else
    {  
    display.showNumberDec(antall);          // Vis tall
    }
  }
}

void styrRele()
// Reléet er av når ingen er i rommet (eller negativt antall personer...),
// skrur på når minst én person er inne. Oppdaterer relé flagget.
{
    if(antall <= 0)
  {
    digitalWrite(pinRele, LOW);
    releFlagg = 0;
  }
  
  if(antall >= 1)
  {
    digitalWrite(pinRele, HIGH);
    releFlagg = 1;
  }
}

void skrivTilMonitor()
// Sender mesteparten av kommunikasjon med monitor.
// Brukes hovedsaklig til debugging (feilsøking)
// Den hjelper med feilsøking og prototyping.
// Kan fjernes når programmet er ferdigprogramert, eller skrus av.
{
  Serial.print("Det er foreløpig: ");
  Serial.print(antall);
  Serial.print(" inne i rommet ");
  Serial.print(" RFID lest: ");
  Serial.print(RFIDflagg);
  Serial.print(" kort er akseptert: ");
  Serial.print(nokkelFlagg);

  
  if (servoFlagg == 1) 
  {
     Serial.print(" og porten er åpen!");
  }
  
  if (servoFlagg == 0) 
  {
     Serial.print(" og porten er stengt!");
  }

  if (releFlagg == 0)
  {
     Serial.println("Reléet er AV.");
  }

  if (releFlagg == 1)
  {
     Serial.println("Reléet er PÅ.");
  }

  //Kommandoer som kan brukes for feilsøking ...
  //Serial.print("FlaggSluse1: ");
  //Serial.print(flaggSluse1);
  //Serial.print(", FlaggSluse2: ");
  //Serial.println(flaggSluse2);
}
