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

// 7-segment display 
const byte PIN_CLK = 32;
const byte PIN_DIO = 33;
TM1637Display display(PIN_CLK, PIN_DIO);

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
  // 
  display.setBrightness(7); 
  pinMode(LED_BUILTIN, OUTPUT);
  // Initier ESP-NOW
  initESPNOW();
}

void loop() {} // void loop er tom

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
