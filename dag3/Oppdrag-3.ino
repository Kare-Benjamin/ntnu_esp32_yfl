/*
Oppdrag-3.ino
Author: Kåre-Benjamin Hammervold 8.9.2020
On behalf of NTNU.
Edited 18.1.2021

Coauthor: Nils Kr. Rossing
On behald Skolelaboratoriet
Edited 23.11.2020

Et oppsett med et relé (f.eks.: JQC-3FF-S-Z) koblet på PIN 14 og BME 280 sensor koblet via I2C til ESP32.
Programmet er testet med varianten: DOIT ESP-32 DEVKIT V1.
Sensoren BME280 
Sensordata skrives ut i konsollen (serial monitor).

Pinout:
BME280       |     ESP-32
----------------------------------
SCL/SCK:     |     D22
SDA/DATA:    |     D21
VDD:         |     3V3 (3.3V)
GND:         |     GND

Det er anbefalt å koble opp en kondensator (avkoppling) mellom GND og VDD på sensorer. F.eks.: 0.1uF keramisk kondensator.

Pinout:
JQC-3FF-S-Z  |     ESP-32
----------------------------------
VCC:         |     5V (Vin)
GND:         |     GND
IN:          |     D14
*/
 

#include <Adafruit_BME280.h>  // Inkluder biblioteket for BME280.

// Lager et Adafruit_BME280 objekt kalt bme280.  
// Slik får vi tilgang på funksjonene tilhørende objektet.
Adafruit_BME280 bme280;

#define RELAYPIN 14 // Digitalt utgang for ESP32 for styring av vifterele.

void setup() {
  
  Serial.begin(115200);        // Setter datahastigheten mellom ESP32 og PC.
  bme280.begin(0x76);          // Initialiserer BME280 sitt bibliotek og legger inn I2C bussadressen (0x76).
  pinMode(RELAYPIN, OUTPUT);
  Serial.println("Oppdrag-3"); // Skriv ut hvilket program som ligger i mikrokontrolleren.
}


void loop() { 
  // Leser av sensoren og skriver til monitor, se funksjonsbeskrivelsen lengre ned.
  sensorValuesToMonitor();
}


void sensorValuesToMonitor() {
  // bme280 objektet har flere funksjoner, blant annet for temperature, trykk og fuktighet.
  // Disse verdiene lagres lokalt i variabler og blir skrevet ut til monitor.
  // Sjekker om tall er i gyldighetsområdet, hvis ikke kjøres en restart.

  // Les av sensoren BME280, temperatur, lufttrykk og luftfuktighet.
  // Flyttall (float) tillater desimaltall i avlesningen (to desimaler). 
  float temperature = bme280.readTemperature();
  float pressure = bme280.readPressure()/ 100.0; // Deler på 100.0 og får da hPa og mBar, vanlig benevning for atmosfærisk trykk. 
  float humidity = bme280.readHumidity();

  Serial.println("---------------------");
  
  Serial.print("Temperatur: ");
  Serial.print(temperature);
  Serial.println(" *C");
 
  Serial.print("Trykk: ");
  Serial.print(pressure);
  Serial.println(" hPa");

  Serial.print("Fuktighet: ");
  Serial.print(humidity);
  Serial.println(" %");

  Serial.println("---------------------");
  Serial.println();
}
