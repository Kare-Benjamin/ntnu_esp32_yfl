/*
Oppdrag-6

Author: Nils Kr. Rossing
On behald Skolelaboratoriet
Edited 05.12.2020

Coauthor: Kåre-Benjamin Hammervold Rørvik 8.9.2020
On behald of NTNU.
Edited 19.1.2021

Et oppsett med et relé (f.eks.: JQC-3FF-S-Z) koblet på PIN 14 og BME 280 sensor koblet via I2C til ESP32.
Det er også koblet på en Servo som bom (SG90). 
Programmet er testet med varianten: DOIT ESP-32 DEVKIT V1.

Reléet blir kontrollert av en digital høy/lav logikk.
Dersom reléet er aktivt lavt, så betyr det at et signal LOW eller 0 skrur på reléet.
Dersom reléet er aktivt høy, så betyr det at et signal HIGH eller 1 skrur på reléet.

Kontrolleren er koblet til Wi-Fi og kommuniserer med Circus of Things.
Det må settes opp en knapp i CoT panelet for å styre Reléet. 
Sensordata som måles lastes opp til CoT regelmessig.
Det må settes opp "view" paneler (display) i CoT panelet for å se verdiene som lastes opp. 

Pinout:
BME280       |     ESP-32
----------------------------------
SCL/SCK:     |     D22
SDA/DATA:    |     D21
VDD:         |     3V3 (3.3V)
GND:         |     GND

Det er anbefalt å koble opp en kondensator (avkoppling) mellom GND og VDD på sensorer. F.eks.: 0.1uF keramisk kondensator.

Pinout rele:
JQC-3FF-S-Z  |     ESP-32
----------------------------------
VCC:         |     5V (Vin)
GND:         |     GND
IN:          |     D14

Pinout servo:
SG90         |     ESP-32
----------------------------------
VCC:         |     5V (Vin)
GND:         |     GND
IN:          |     D13

*/ 

#include <CircusESP32Lib.h>   // Inkluder biblioteket som kommuniserer med CoT.
#include <Servo.h>            // Inkluder servo-biblioteket .
#include <Adafruit_BME280.h>  // Inkluder biblioteket for BME280.

// ---------------------------------------------------------------------
// CircusESP32Lib relaterte deklarasjoner for oppkobling til WiFi og CoT
// ---------------------------------------------------------------------

char ssid[] = "ssid";                            // Skriv inn navnet til ruteren din her
char password[] = "password";                    // Skriv inn passordet til ruteren din her
char token[] = "token";                          // Plasser din brukeridentitet her (token)
char server[] = "www.circusofthings.com";        // Her ligger serveren
char order_key_gate[] = "1234";                  // Nøkkel for å kommunisere med bommen.
char order_key_relay[] = "1234";                 // Nøkkel-informasjon om konsollet for styring av relet hos CoT 
char temperature_key[] = "1234";                 // Nøkkel-informasjon om konsollet for visning av temperatur hos CoT.
char humidity_key[] = "1234";                    // Nøkkel-informasjon om konsollet for visning av luftfuktighet hos CoT.
char pressure_key[] = "1234";                    // Nøkkel-informasjon om konsollet for visning av lufttrykk hos CoT.

CircusESP32Lib circusESP32(server,ssid,password);// Definer objektet circusESP32 av typen CircusESP32Lib.
Adafruit_BME280 bme280;                          // Definer objektet bme280 av typen Adafruit_BME280.
Servo myservo;                                   // Definerer objektet myservo av typen Servo.

// Deklarasjon av konstanter som definerer oppkobling til ESP32.
#define RELAYPIN 14       // Digitalt utgang for ESP32 for styring av vifterele.
#define GATEPIN 13        // Digitalt utgang for ESP32 for styring av bom.
 
void setup() {
    Serial.begin(115200);        // Sett opp kommunikasjonshastigheten til monitoren.
    pinMode(RELAYPIN, OUTPUT);   // Definer RELAYPIN som en utgang.
    pinMode(GATEPIN, OUTPUT);    // Definer GATEPIN som en utgang.
    circusESP32.begin();         // Initialiser Circus of Things.
    bme280.begin(0x76);          // Initialisering av sensor BNE280.
    myservo.attach(GATEPIN);     // Gi informasjon om hvilken port servoen er tilsluttet.
    Serial.println("Oppdrag-6"); // Skriv ut hvilket program som ligger i mikrokontrolleren.
}
 
void loop() { 

  delay(1000);

  // Leser av verdiene fra sensorene BME280.
  float temperature = bme280.readTemperature();
  float pressure    = bme280.readPressure()/ 100.0; // Deler på 100.0 og får da hPa og mBar, vanlig benevning for atmosfærisk trykk.
  float humidity = bme280.readHumidity();

  // Rapporterer verdiene fra sensorene til CoT.
  circusESP32.write(temperature_key,temperature,token); // Rapporter temperatur til Circus of Things.
  circusESP32.write(humidity_key,humidity,token);       // Rapporter luftfuktighet til Circus of Things.
  circusESP32.write(pressure_key,pressure,token);       // Rapporter lufttrykk til Circus of Things.

  // Motta styreinformasjon til reléet fra Circus of thing.
  int dashboard_order_relay = circusESP32.read(order_key_relay,token); 
  
  // Reléet bruker verdien som hentest fra CoT, og styrer direkte Reléet med den.
  digitalWrite(RELAYPIN, dashboard_order_relay); 

  // Motta posisjonen til bommen i grader fra CoT.
  int dashboard_order_gate = circusESP32.read(order_key_gate,token); 
    
  // Still inn bommen til ønsket vinkel dashboard_order_gate.
  myservo.write(int(dashboard_order_gate));
 
}
