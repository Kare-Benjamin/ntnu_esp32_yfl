/*
Oppdrag-4

Author: Kåre-Benjamin Hammervold Rørvik 8.9.2020
On behalf of NTNU.
Edited 18.1.2021

Coauthor: Nils Kr. Rossing
On behald Skolelaboratoriet
Edited 23.11.2020

Et oppsett med BME 280 sensor koblet via I2C til ESP32, og et rele av typen JQC-3FF-S-Z
Programmet er testet med variantene: ESP32 DEV MODULE og DOIT ESP-32 DEVKIT V1
Sensordata skrives ut i konsollen (serial monitor).
Kontrolleren er koblet til Wi-Fi og kommuniserer med Circus of Things.
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

*/ 

#include <CircusESP32Lib.h>   // Inkluder biblioteket som kommuniserer med CoT.
#include <Adafruit_BME280.h>  // Inkluder biblioteket for BME280.

// ---------------------------------------------------------------------
// CircusESP32Lib relaterte deklarasjoner for oppkobling til WiFi og CoT
// ---------------------------------------------------------------------

char ssid[] = "ssid";                            // Skriv inn navnet til ruteren din her
char password[] = "password";                    // Skriv inn passordet til ruteren din her
char token[] = "token";                          // Plasser din brukeridentitet her (token)
char server[] = "www.circusofthings.com";        // Her ligger serveren
char temperature_key[] = "1234";                 // Nøkkel-informasjon om konsollet for visning av temperatur hos CoT.
char humidity_key[] = "1234";                    // Nøkkel-informasjon om konsollet for visning av luftfuktighet hos CoT.
char pressure_key[] = "1234";                    // Nøkkel-informasjon om konsollet for visning av lufttrykk hos CoT.
CircusESP32Lib circusESP32(server,ssid,password);// Her leses nettadressen til CoT, ruternavn og passord inn. 

// Deklarer et objekt bme280 av typen Adafruit_BME280.
Adafruit_BME280 bme280; 

void setup() {
  Serial.begin(115200);        // Setter datahastigheten mellom ESP32 og PC.
  bme280.begin(0x76);          // Initialiser sensoren BME280.
  circusESP32.begin();         // Initialiser Circus of Things.
  Serial.println("Oppdrag-4"); // Skriv ut hvilket program som ligger i mikrokontrolleren.
}
 
void loop() {
  delay(1000);

  // Les av sensoren BME280, temperatur, lufttrykk og luftfuktighet.
  float temperature = bme280.readTemperature();
  float pressure    = bme280.readPressure()/ 100.0; // Deler på 100.0 og får da hPa og mBar, vanlig benevning for atmosfærisk trykk. 
  float humidity    = bme280.readHumidity();
    
  // Vis verdiene i monitoren
  Serial.println("-------------------------"); 
  Serial.print("Temperature: "); Serial.println(temperature); 
  Serial.print("Humidity: "); Serial.println(humidity);
  Serial.print("Air pressure: "); Serial.println(pressure);
  Serial.println("-------------------------");
  Serial.println(" ");

  // Send verdeiene til Circus of Things
  circusESP32.write(temperature_key,temperature,token); // Rapportert målt temperatur til CoT.
  circusESP32.write(humidity_key,humidity,token);       // Rapportert målt luftfuktighet til CoT.
  circusESP32.write(pressure_key,pressure,token);       // Rapportert målt lufttrykk til CoT.
}
