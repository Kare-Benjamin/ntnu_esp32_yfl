/*
Oppdrag-1

Author: Nils Kristian Rossing og Kåre-Benjamin Hammervold Rørvik 11.26.2020
On behalf of Skolelaboratoriet
Edited 18.1.2021

Et oppsett med et relé (f.eks.: JQC-3FF-S-Z) koblet på PIN 14.
Programmet er testet med varianten: DOIT ESP-32 DEVKIT V1.
Reléet blir kontrollert av en digital høy/lav logikk.
Dersom reléet er aktivt lavt, så betyr det at et signal LOW eller 0 skrur på reléet.
Dersom reléet er aktivt høyt, så betyr det at et signal HIGH eller 1 skrur på reléet.

Pinout rele:
JQC-3FF-S-Z  |     ESP-32
----------------------------------
VCC:         |     5V (Vin)
GND:         |     GND
IN:          |     D14

*/

int RelayPIN = 14;
 
void setup() {
  pinMode(RelayPIN, OUTPUT); // Define our pin for the relay as an output 
}
 
void loop() {
  digitalWrite(RelayPIN , HIGH);
  delay(1000);
  digitalWrite(RelayPIN , LOW);
  delay(1000);
}
