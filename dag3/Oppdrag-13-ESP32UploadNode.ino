/*
Author: Kåre-Benjamin Hammervold Rørvik og Nils Kristian Rossing 23.9.2020
On behald of NTNU.
Edited 27.12.2020

En ESP32 som står i Station Mode og fungerer som uplink og gateway.
Mottar datapakker over ESP-NOW.
Sender og mottar data over WiFi opp mot CoT.
Bytter mellom WiFi og ESP-NOW.
Antall meldinger sendt vises på displayet, temperatur og fuktighet i rotering.

Pinout:

7-Segment    |     ESP-32
----------------------------------
CLK          |     D32
DIO          |     D33
VDD:         |     3V3 (3.3V)
GND:         |     GND

// Sources: 
https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/network/esp_now.html
https://randomnerdtutorials.com/esp-now-esp8266-nodemcu-arduino-ide/?fbclid=IwAR0pH-ilF45ceHlyqjB1Im5oDkOJUtxJ7g1TDABbaSOkiBep5xYlsH1y5_M
*/

#include <esp_now.h>
#include <WiFi.h>
#include <Wire.h>
#include <TM1637Display.h>
#include <CircusESP32Lib.h>

// 7-segment display 
const byte PIN_CLK = 32;
const byte PIN_DIO = 33;
TM1637Display display(PIN_CLK, PIN_DIO);
 
// Disse deklarasjonene inneholder informasjonen som er nødvendig for å koble opp mot Circus of Things

char ssid[] = "TP-Link_5C89";                     // SSID til ruteren som brukes for kommunikasjon med nettet
char password[] =  "31686617";                    // Passordet til ruteren som brukes for kommunikasjon med nettet
char token[] = "eyJhbGciOiJIUzI1NiJ9.eyJqdGkiOiIyNjQ5In0.msHnAJHwdnxX-eS0qyIFUEWV4vdBxoyHGOb7Yjsliik"; // Plasser din brukeridentitet her (token)
char server[] = "www.circusofthings.com";         // Nettsiden hvor CoT-serveren ligger
char temperature_key[] = "21251";                 // Nøkkel for å kommunisere med temperatursensoren hos BME280  
char humidity_key[] = "20852";                    // Nøkkel for å kommunisere med luftfuktighetssensoren hos BME280
char pressure_key[] = "13531";                    // Nøkkel for å kommunisere med lufttrykksensoren hos BME280

// Legger inn et objekt og vi får tilgang til ruter og server ved CoT. 
CircusESP32Lib circusESP32(server,ssid,password);

// Transmitted data
typedef struct {
    int instance;
    float temperature;
    float humidity;
    float pressure;
    String source;
} ESPNOWData;

// Oppretter et packet objekt for strukturen ESPNOWData.
ESPNOWData packet;

void setup() {
  Serial.begin(115200); 
  // Initier ESP-NOW
  initESPNOW();
  // Sett lysstyrke på display
  display.setBrightness(7); 
  
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  uploadTheData(10000);
}

void initESPNOW() {
 
  // ESP-32 må være konfigurert i Station Mode for at ESP-NOW skal kunne benyttes.
  WiFi.mode(WIFI_STA);

  // Initier ESP-NOW
  esp_now_init();

  // esp_now_register_recv_cb aktiverer callback funksjonen "activateOnCallback" når ESP-32 får inn en datapakke.
  esp_now_register_recv_cb(activateOnCallback);
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
  delay(500);                           // Vent i 500 millisek. (0,5 sek.)
}

void uploadTheData(int delayLength){
  // Laster opp data til CoT.
  // Tar i mot en delayLength som bestemmer hvor ofte data skal lastes opp til skyen.
  digitalWrite(LED_BUILTIN, HIGH);
  circusESP32.begin(); // Let the Circus object set up itself for an SSL/Secure connection
  circusESP32.write(temperature_key,packet.temperature,token); // Report the temperature measured to Circus.
  circusESP32.write(humidity_key,packet.humidity,token); // Report the humidity measured to Circus.
  circusESP32.write(pressure_key,packet.pressure,token); // Report the pressure measured to Circus.
  digitalWrite(LED_BUILTIN, LOW);
  delay(10);
  WiFi.disconnect(true);
  WiFi.mode(WIFI_STA);
  delay(delayLength);
}

void activateOnCallback(const uint8_t * mac, const uint8_t *incomingData, int len) {
  // Memcpy kopierer verdiene i incomingData til packet objektet.
  // Sizeof operasjonen angir antall bytes som utgjør packet, 
  // og denne informasjonen brukes i memcpy for å bestemme antallet bytes som skal kopieres.
  memcpy(&packet, incomingData, sizeof(packet));

  // Dataene skrives ut til monitoren
  Serial.print("Data packet nummer: ");
  Serial.print(packet.instance);
  Serial.print(" fra: ");
  Serial.println(packet.source);
  Serial.print("Temperaturen er: ");
  Serial.print(packet.temperature);
  Serial.println("C");
  Serial.print("Luftfuktigheten er : ");
  Serial.print(packet.humidity);
  Serial.println("%");
  Serial.print("Pressure is : ");
  Serial.print(packet.pressure/100);
  Serial.println("hPa");
  Serial.println();
  
  // Vis temperatur på 7-segment displayet
  showAtDisplay(packet.temperature);
}
