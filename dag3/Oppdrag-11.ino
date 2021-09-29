/*
Oppdrag-11

Author: Nils Kr. Rossing
On behald Skolelaboratoriet
Edited 12.12.2020

Coauthor: Kåre-Benjamin Hammervold Rørvik 8.9.2020
On behald of NTNU.
Edited ??.??.2020

Oppkoblingen består av en BME 280 sensor koblet via I2C til en ESP32, 
et rele av typen JQC-3FF-S-Z, en servo av typen SQ90 og et display av 
typen TM1637 er også tilkoblet.
 
Skissen kombinerer styring av releet og bommen og avlesninng av sensorer hos CoT. 
I tillegg skrives temperaturen ut til display. I oppdrag 10 legges det inn en 
tersketemperatur for styring av vifta. Når temperaturen overstiger terskelen 
starter vifta.

Skissen er testet med varianten: DOIT ESP-32 DEVKIT V1
Sensordata skrives ut i konsollen (serial monitor).

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

char ssid[] = "TP-Link_5C89";                     // SSID til ruteren som brukes for kommunikasjon med nettet
char password[] =  "31686617";                    // Passordet til ruteren som brukes for kommunikasjon med nettet
//char ssid[] = "ice.net-19BD5C";                     // SSID til ruteren som brukes for kommunikasjon med nettet
//char password[] =  "D74EB1E0";                      // Passordet til ruteren som brukes for kommunikasjon med nettet
char token[] = "eyJhbGciOiJIUzI1NiJ9.eyJqdGkiOiIyNjQ5In0.msHnAJHwdnxX-eS0qyIFUEWV4vdBxoyHGOb7Yjsliik"; // Plasser din brukeridentitet her (token)
char server[] = "www.circusofthings.com";         // Nettsiden hvor CoT-serveren ligger
char order_key_relay[] = "27094";                 // Nøkkel for å kommunisere med viftereleet
char order_key_treshhold[] = "10032";             // Nøkkel for overføring av terskelverdien
char order_key_gate[] = "19499";                  // Nøkkel for å kommunisere med bommen
char temperature_key[] = "21251";                 // Nøkkel for å kommunisere med temperatursensoren hos BME280  
char humidity_key[] = "20852";                    // Nøkkel for å kommunisere med luftfuktighetssensoren hos BME280
char pressure_key[] = "13531";                    // Nøkkel for å kommunisere med lufttrykksensoren hos BME280
char time_key[] = "17726";                        // Nøkkel for å kommunisere tid siden start

CircusESP32Lib circusESP32(server,ssid,password); // Definer objektet circusESP32 av typen CircusESP32Lib
Adafruit_BME280 bme280;                           // Definer objektet bme280 av typen Adafruit_BME280
Servo myservo;                                    // Definerer objektet myservo av typen Servo

// Deklarasjon av konstanter som definerer oppkobling til ESP32
#define RELAYPIN 14       // Digitalt utgang for ESP32 for styring av vifterele
#define GATEPIN 13        // Digitalt utgang for ESP32 for styring av bom
#define PIN_CLK 32        // Definer CLK pin og velg port D32
#define PIN_DIO 33        // Definer DIO pin og velg port D33
#define MEGA 1000000      // Multiplikasjonsfaktor å gjøre om til sekunder
#define MAALEINTERVALL 10 // Antallsekunder mellom hver måling 

RTC_DATA_ATTR unsigned long millisOffset = 0; // Lager en variabel som holder på tidspunktet da ESP32 gikk i dvale

TM1637Display display(PIN_CLK, PIN_DIO); // Definer objektet display av typen TM1637Display


void setup() {
    Serial.begin(115200);        // Sett opp kommunikasjonshastigheten til monitoren
    pinMode(RELAYPIN, OUTPUT);   // Definer RELAYPIN som en utgang   
    pinMode(GATEPIN, OUTPUT);    // Definer GATEPIN som en utgang
    circusESP32.begin();         // Initialiser Circus of Things
    bme280.begin(0x76);          // Initialisering av sensor BNE280
    myservo.attach(GATEPIN);     // Gi informasjon om hvilken port servoen er tilsluttet
    display.setBrightness(7);    // Setter lysstyrken på displayet. Fra 0 (min) til 7 (max).
    Serial.println("Oppdrag-8"); // Skriv ut hvilket program som ligger i mikrokontrolleren
    esp_sleep_enable_timer_wakeup(MAALEINTERVALL * MEGA); // Setter opp antall sekunder mellom hver måling
}
 
void loop() { 
    
    // Les av temperatur og send til CoT
    float temperatur = readTempBME280AndSendToCoT();

    // Les av luftfuktighet og send til CoT
    readHumBME280AndSendToCoT();

    // Les av lufttrykk og send til CoT
    readPresBME280AndSendToCoT();
    
     // Vis temperatur på display
    showAtDisplay (temperatur);


    // Motta styreinforasjon for releet og sett det i ønsket stilling
    readCoTAndSetRelay(temperatur);

    // Motta styreinforasjon for bommen og sett den i ønsket stilling
    readCoTAndSetServo();
    
    delay(500);
    
    // Les av tiden og rapportert til CoT
    showTimeToCoT();
    
    esp_deep_sleep_start();
}

float showTimeToCoT(){
 
  // Leser av tid siden start
  millisOffset = millis() + millisOffset + MAALEINTERVALL*1000;
  
  float timeSec = float(millisOffset / 1000.0);

  // Rapporterer tid siden start i sekunder til CoT
  circusESP32.write(time_key,timeSec,token); // Rapporter tid siden start til Circus of Things.

}

float readTempBME280AndSendToCoT(){
    
    // Leser av temperatur fra sensorene BME280
    float temperature = bme280.readTemperature();
   
    // Rapporterer temperatur fra sensorene til CoT
    circusESP32.write(temperature_key,temperature,token); // Rapporter temperatur til Circus of Things.

    return temperature;
}

float readHumBME280AndSendToCoT(){
    
    // Leser av luftfuktighet fra sensorene BME280
    float humidity    = bme280.readHumidity();

    // Rapporterer luftfuktighet fra sensorene til CoT
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


void readCoTAndSetRelay(float temperatur){
    
    // Leser kommando til releet
    double dashboard_order_relay     = circusESP32.read(order_key_relay,token);
    double dashboard_order_treshhold = circusESP32.read(order_key_treshhold,token);
 
    // Slå på vifta når dashboard_order_realy er lik 1
    if(dashboard_order_relay == 1)      // ON
    {
      digitalWrite(RELAYPIN, HIGH); // Koble til relekontakt
    } 
    else if(dashboard_order_relay == 2) // AUTO
    {
      if(temperatur > dashboard_order_treshhold)
      {
        digitalWrite(RELAYPIN, HIGH); // Koble til relekontakt når tempeperaturen er høyere en terskelverdien
      }
      else
      {
        digitalWrite(RELAYPIN, LOW); // Koble fra relekontakt når tempeperaturen er høyere en terskelverdien
      }   
    }
    else if (dashboard_order_relay == 0) // OFF
    {
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
  display.showNumberDecEx(int(showNumber), 0b01000000, false, 4, 0);    // Vis tall
  delay(500);                           // Vent i 1000 millisek. (1 sek.)
}
