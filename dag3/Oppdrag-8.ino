/*
Oppdrag-8

Author: Nils Kr. Rossing
On behald Skolelaboratoriet
Edited 08.12.2020

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

Temperaturen vises på et 7-segment display.

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

Pinout 7-segment display:
TM1637       |     ESP-32
----------------------------------
VCC:         |     5V (Vin)
GND:         |     GND
DIO:         |     D33
CLK:         |     D32

*/ 

#include <CircusESP32Lib.h>   // Inkluder biblioteket som kommuniserer med CoT 
#include <Servo.h>            // Inkluder servo-biblioteket 
#include <Adafruit_BME280.h>  // Inkluder biblioteket for BME280       
#include <TM1637Display.h>    // Inkluder bibliotek for TM1637

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
#define PIN_CLK 32        // Digitalt utgang for ESP32 for klokkesignalet til 7-segment displayet.
#define PIN_DIO 33        // Digitalt utgang for ESP32 for data signalet til 7-segment displayet.

TM1637Display display(PIN_CLK, PIN_DIO); // Definer objektet display av typen TM1637Display.


void setup() {
  Serial.begin(115200);        // Sett opp kommunikasjonshastigheten til monitoren.
  pinMode(RELAYPIN, OUTPUT);   // Definer RELAYPIN som en utgang.
  pinMode(GATEPIN, OUTPUT);    // Definer GATEPIN som en utgang.
  circusESP32.begin();         // Initialiser Circus of Things.
  bme280.begin(0x76);          // Initialisering av sensor BNE280.
  myservo.attach(GATEPIN);     // Gi informasjon om hvilken port servoen er tilsluttet.
  display.setBrightness(7);    // Setter lysstyrken på displayet. Fra 0 (min) til 7 (max).
  Serial.println("Oppdrag-8"); // Skriv ut hvilket program som ligger i mikrokontrolleren.
}
 
void loop() { 

  // Les av temperatur og send til CoT.
  float temperatur = readTempBME280AndSendToCoT();

  // Les av luftfuktighet og send til CoT.
  readHumBME280AndSendToCoT();

  // Les av lufttrykk og send til CoT.
  readPresBME280AndSendToCoT();
  
   // Vis temperatur på display.
  showAtDisplay (temperatur);


  // Motta styreinforasjon for releet og sett det i ønsket stilling.
  readCoTAndSetRelay();

  // Motta styreinforasjon for bommen og sett den i ønsket stilling.
  readCoTAndSetServo();
    
    delay(500); 
}

float readTempBME280AndSendToCoT(){
  
  // Leser av temperatur fra sensorene BME280.
  float temperature = bme280.readTemperature();
 
  // Rapporterer temperatur fra sensorene til CoT
  circusESP32.write(temperature_key,temperature,token); // Rapporter temperatur til Circus of Things.

  return temperature;
}

float readHumBME280AndSendToCoT(){
    
  // Leser av luftfuktighet fra sensorene BME280.
  float humidity    = bme280.readHumidity();

  // Rapporterer luftfuktighet fra sensorene til CoT.
  circusESP32.write(humidity_key,humidity,token);       // Rapporter luftfuktighet til Circus of Things.

  return humidity;
}

float readPresBME280AndSendToCoT(){
  
  // Leser av lufttrykk fra sensorene BME280
  float pressure    = bme280.readPressure()/ 100.0F;

  // Rapporterer lufttrykk fra sensorene til CoT
  circusESP32.write(pressure_key,pressure,token);       // Rapporter lufttrykk til Circus of Things. 

  return pressure;
}


void readCoTAndSetRelay(){
  
  // Leser kommando til releet
  double dashboard_order_relay = circusESP32.read(order_key_relay,token);

  // Slå på vifta når dashboard_order_realy er lik 1
  if(dashboard_order_relay == 1){
      digitalWrite(RELAYPIN, HIGH); // Koble til relekontakt
  } 
  else {
      digitalWrite(RELAYPIN, LOW);  // Koble fra relekontakt
  }
}

void readCoTAndSetServo(){

  // Leser posisjonen til bommen i grader fra CoT
  double dashboard_order_gate = circusESP32.read(order_key_gate,token); 

  // Still inn bommen til ønsket vinkel dashboard_order_gate
  myservo.write(int(dashboard_order_gate));
}

void showAtDisplay (float showNumber){
  
  // Vis showNumber på display. showNumber er et desimaltall. 
  // Vårt display kan bare vise heltall, men vi kan sette komma fast. 
  // Vi har valgt å sette komma fast etter andre siffer (f.eks. 23.00)
  // Ved å multiplisere showNumber med 100, 
  // konvertere tallet til et heltall får vi vist den med to desimaler
  
  showNumber = showNumber*100;
  display.clear();                      // Tøm displayet for data
  display.showNumberDecEx(int(showNumber), 0b01000000, false, 4, 0);  // Vis tall
  delay(500);
}
