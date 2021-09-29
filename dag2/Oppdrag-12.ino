/* Programmet leser informasjon fra RFID-briken og skriver ut til monitoren
 * Programmet er skrevet av Kåre-Benjamin H. Rørvik 08.07.20
 * Programmet er videreutviklet av Nils Kr. Rossing 17.07.20
 */

#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>

const int RST_PIN = 22; // Reset.
const int SS_PIN = 21;  // Slave.
 
MFRC522 mfrc522(SS_PIN, RST_PIN); // Oppretter instansen mfrc522 av typen MFRC522.
Servo myservo;    // Lag et objekt med navnet myservo.
                  // Hele 12 slike objekter kan lages ved hvert kort.

int posLukket = 0;// variabel som holde lokket posisjon for bom i grader.
int posApen = 90; // Variabel som holde åpen posisjon for bom i grader.
int ID0 = 124;    // Erstatt "124" med avlest byte 0.
int ID1 = 17;     // Erstatt "17" med avlest byte 1.
int ID2 = 94;     // Erstatt "94" med avlest byte 2.
int ID3 = 116;    // Erstatt "116" med avlest byte 3.
int RFIDflagg = 0;// Flagg som indikerer at nytt kort er lest.
 
void setup() {
Serial.begin(115200); // Starter seriell kommunikasjon med Baud rate på 115200 bit/sekund (husk å endre i terminalen).
SPI.begin();          // Starter SPI - Initierer SPI-buss som kommuniserer mot RFID RC522.
mfrc522.PCD_Init();   // Starter RFID leseren.
myservo.attach(13);   // attaches the servo on pin 13 to the servo object.
myservo.write(posLukket); // be servoen om å gå til posisjon lukket.
Serial.println("**Klar til å lese kort**");
}
 
void loop() 
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
if((mfrc522.uid.uidByte[0]==ID0) &&
   (mfrc522.uid.uidByte[1]==ID1) &&
   (mfrc522.uid.uidByte[2]==ID2) &&
   (mfrc522.uid.uidByte[3]==ID3) && RFIDflagg == 1)
   {
    Serial.println("Velkommen inn");
    myservo.write(posApen);          // be servoen om å gå til posisjon pos.
    delay(5000);                     // Stå i posisjon 0 i 5 sek.
    myservo.write(posLukket);        // be servoen om å gå til posisjon pos.
   }
   
 RFIDflagg = 0;  // Resett flagg som forteller at kortet er sjekket.

}
