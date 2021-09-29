/*********
  Complete project details at https://randomnerdtutorials.com  
  Written by BARRAGAN and modified by Scott Fitzgerald
  Modifisert av Nils Kr. Rossing 05.07.20,
  Gjennomgått og testet av Kåre-Benjamin H. Rørvik 15.07.20
*********/

#include <Servo.h>

Servo myservo;  // Lag et objekt med navnet myservo
                // Hele 12 slike objekter kan lages ved hvert kort

int pos;    // variabel som holde posisjonen i grader

void setup() {
  myservo.attach(13);  // attaches the servo on pin 13 to the servo object
}

void loop() 
{
    pos = 0;                         // Her kan du justere posisjonen om du ønsker å få en penere posisjonering.
    myservo.write(pos);              // Be servoen om å gå til posisjon pos
    delay(5000);                     // Stå i posisjon 0 i 5 sek 

    pos = 90;                        // Her kan du justere posisjonen om du ønsker å få en penere posisjonering.
    myservo.write(pos);              // Be servoen om å gå til posisjon pos
    delay(3500);                     // Stå i posisjon 0 i 5 sek
}
