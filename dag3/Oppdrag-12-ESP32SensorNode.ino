/*
Author: Kåre-Benjamin Hammervold Rørvik og Nils Kristian Rossing 01.01.2021
On behald of NTNU.
Edited 01.01.2021
Et oppsett med BME 280 sensor koblet via I2C til ESP32.

En ESP32 som står i Station Mode og fungerer som sensor node.
Sender datapakker over ESP-NOW.
Sender og mottar data over WiFi opp mot CoT.
Bytter mellom WiFi og ESP-NOW.
Antall meldinger sendt vises på displayet, temperatur fuktighet og trykk i rotering.

Sketchen er testet med variantene: ESP32 DEV MODULE og DOIT ESP-32 DEVKIT V1
Sensordata skrives ut i konsollen (serial monitor).

BME280       |     ESP-32
----------------------------------
SCL/SCK:     |     D22
SDA/DATA:    |     D21
VDD:         |     3V3 (3.3V)
GND:         |     GND

Det er anbefalt å koble opp en kondensator (avkoppling) mellom GND og VDD på sensorer. F.eks.: 0.1uF keramisk kondensator.

// Sources: 
https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/network/esp_now.html
https://randomnerdtutorials.com/esp-now-esp8266-nodemcu-arduino-ide/?fbclid=IwAR0pH-ilF45ceHlyqjB1Im5oDkOJUtxJ7g1TDABbaSOkiBep5xYlsH1y5_M
*/

#include <esp_now.h>
#include <WiFi.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

// Adressen til mottakeren ved ESP-NOW kommunikasjon, dette er MAC addressen skrevet på hex format.
uint8_t broadcastAddress[] = {0x24, 0x6F, 0x28, 0x9E, 0x9F, 0x70};

Adafruit_BME280 bme280;
unsigned long delayTime;

// Teller antall meldinger som er sendt, starter på 0.
int consecutive_messages = 0;

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
  initESPNOW();
  bme280.begin(0x76); // Initier BME28.0 og legg inn I2C-adressen
}
 
void loop() {
  
  // Tell opp antall målinger
  consecutive_messages++;
  
  // Samler inn data fra sensorene
  packet.pressure = bme280.readPressure();
  packet.instance = consecutive_messages;
  packet.temperature = bme280.readTemperature();
  packet.source = "5C";
  packet.humidity = bme280.readHumidity();
  
  // Sender data i packet.
  esp_now_send(broadcastAddress, (uint8_t *) &packet, sizeof(packet));
  delay(2000);
}

void initESPNOW(){
  
  // ESP-32 må være konfigurert i Station Mode for at ESP-NOW skal kunne benyttes.
  WiFi.mode(WIFI_STA);

  // Initier ESP-NOW
  esp_now_init();

  // Lag en struktur med navnet peerInfo av typen esp_now_peer_info_t som inneholder informasjon for å opprette kommunikasjon
  esp_now_peer_info_t peerInfo;

  // Legg inn data i strukturen peerInfo
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;

  // Opprett kommunikasjonen mellom ESP-NOW enhetene 
  esp_now_add_peer(&peerInfo);
}

// callback when data is sent
void activateOnCallback(const uint8_t *mac_addr, esp_now_send_status_t status) {
}
