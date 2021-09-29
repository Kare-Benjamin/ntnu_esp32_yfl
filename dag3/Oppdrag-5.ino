/*
Oppdrag-5

Author: Kåre-Benjamin Hammervold Rørvik 8.9.2020
On behald of NTNU.
Edited 27.11.2020

Coauthor: Nils Kr. Rossing
On behald Skolelaboratoriet
Edited 05.12.2020

Et oppsett med et relé (f.eks.: JQC-3FF-S-Z) koblet på PIN 14 og BME 280 sensor koblet via I2C til ESP32.
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
*/ 

// Inkludering av biblioteker
#include <CircusESP32Lib.h>  // Biblioteket til Circus of Things.
#include <Wire.h>            // Biblioteket for å kunne bruke I2C.
#include <Adafruit_BME280.h> // Biblioteket for lesing av sensordata.

// ---------------------------------------------------------------------
// CircusESP32Lib relaterte deklarasjoner for oppkobling til WiFi og CoT
// ---------------------------------------------------------------------

char ssid[] = "ssid";                            // Skriv inn navnet til ruteren din her
char password[] = "password";                    // Skriv inn passordet til ruteren din her
char token[] = "token";                          // Plasser din brukeridentitet her (token)
char server[] = "www.circusofthings.com";        // Her ligger serveren
char order_key_relay[] = "1234";                 // Nøkkel-informasjon om konsollet for styring av relet hos CoT 
char temperature_key[] = "1234";                 // Nøkkel-informasjon om konsollet for visning av temperatur hos CoT.
char humidity_key[] = "1234";                    // Nøkkel-informasjon om konsollet for visning av luftfuktighet hos CoT.
char pressure_key[] = "1234";                    // Nøkkel-informasjon om konsollet for visning av lufttrykk hos CoT.
CircusESP32Lib circusESP32(server,ssid,password);// Her leses nettadressen til CoT, ruternavn og passord inn.

Adafruit_BME280 bme280;

#define RELAYPIN 14            // Digitalt utgang for ESP32 for styring av vifterele.
void setup() {
  Serial.begin(115200);        // Setter datahastigheten mellom ESP32 og PC
  pinMode(RELAYPIN, OUTPUT);   // Sett porten RELAYPIN som utgang
  bme280.begin(0x76);          // Initialiser sensoren BME280.
  circusESP32.begin();         // Initialiser kommunikasjon med Circus of Things.
  Serial.println("Oppdrag-5"); // Skriv ut hvilket program som ligger i mikrokontrolleren
}
 
void loop() {
  delay(1000);
  
  // Les av sensoren BME280, temperatur, lufttrykk og luftfuktighet.
  
  float temperature = bme280.readTemperature();
  float pressure    = bme280.readPressure()/ 100.0; // Deler på 100.0 og får da hPa og mBar, vanlig benevning for atmosfærisk trykk.
  float humidity    = bme280.readHumidity();
  
  // Vis verdiene for feilfinning
  Serial.println("-------------------------"); 
  Serial.print("Temperature: "); Serial.println(temperature); 
  Serial.print("Humidity: "); Serial.println(humidity);
  Serial.print("Air pressure: "); Serial.println(pressure);
  Serial.println("-------------------------");
  Serial.println(" ");
  
  // Send sensorverdier til Circus of Things.
  circusESP32.write(temperature_key,temperature,token); // Rapportert målt temperatur til CoT.
  circusESP32.write(humidity_key,humidity,token);       // Rapportert målt luftfuktighet til CoT.
  circusESP32.write(pressure_key,pressure,token);       // Rapportert målt lufttrykk til CoT.
  
  // Motta styreinformasjon til reléet fra Circus of thing.
  int dashboard_order_relay = circusESP32.read(order_key_relay,token); 
  
  // Reléet bruker verdien som hentest fra CoT, og styrer direkte Reléet med den.
  digitalWrite(RELAYPIN, dashboard_order_relay);  
}
