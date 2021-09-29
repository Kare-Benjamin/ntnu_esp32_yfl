/*
Oppdrag-2

Author: Nils Kr. Rossing
On behalf of Skolelaboratoriet
Coauther: Kåre Benjamin H. Rørvik
Edited 18.1.2021

Et oppsett med et relé (f.eks.: JQC-3FF-S-Z) koblet på PIN 14.
Programmet er testet med varianten: DOIT ESP-32 DEVKIT V1.
Reléet blir kontrollert av en digital høy/lav logikk.
Dersom reléet er aktivt lavt, så betyr det at et signal LOW eller 0 skrur på reléet.
Dersom reléet er aktivt høy, så betyr det at et signal HIGH eller 1 skrur på reléet.
Kontrolleren er koblet til Wi-Fi og kommuniserer med Circus of Things.
Det må settes opp en knapp i CoT panelet for å styre Reléet. 

Pinout rele:
JQC-3FF-S-Z  |     ESP-32
----------------------------------
VCC:         |     5V (Vin)
GND:         |     GND
IN:          |     D14
*/

//Inkluderer biblioteket for å kommunisere med Circus of Things
#include <CircusESP32Lib.h>  // Biblioteket til Circus of Things.
 
// ---------------------------------------------------------------------
// CircusESP32Lib relaterte deklarasjoner for oppkobling til WiFi og CoT
// ---------------------------------------------------------------------

char ssid[] = "ssid";                            // Skriv inn navnet til ruteren din her
char password[] = "password";                    // Skriv inn passordet til ruteren din her
char token[] = "token";                          // Plasser din brukeridentitet her (token)
char server[] = "www.circusofthings.com";        // Her ligger serveren
char order_key_relay[] = "1234";                 // Nøkkel-informasjon om konsollet for styring av relet hos CoT 
CircusESP32Lib circusESP32(server,ssid,password);// Her leses nettadressen til CoT, ruternavn og passord inn. 

// ------------------------------------------------
// Deklarasjon relatert til oppkobling til rele
// ------------------------------------------------

#define RELAYPIN 14 // Digitalt utgang for ESP32 for styring av vifterele.

// ------------------------------------------------
 
void setup() {
    pinMode(RELAYPIN, OUTPUT);
    Serial.begin(115200); // Setter datahastigheten mellom ESP32 og PC.    
    circusESP32.begin();  // Initialiserer oppkobling mot CoT
    Serial.println("Oppdrag-2"); // Skriv ut hvilket program som ligger i mikrokontrolleren
}
 
void loop() {
    delay(1000);

    // Motta styreinformasjon til reléet fra Circus of thing.
    int dashboard_order_relay = circusESP32.read(order_key_relay,token);  

    // Reléet bruker verdien som hentest fra CoT, og styrer direkte Reléet med den.
    digitalWrite(RELAYPIN, dashboard_order_relay);     
}
