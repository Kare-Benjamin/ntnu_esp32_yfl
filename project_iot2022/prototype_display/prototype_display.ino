/* Libraries */
#include <CircusESP32Lib.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define BUTTON_BIT 6

/* C++ specific */
#include <bitset>  
#include <iostream>  

#define MAX_TEMP 64
#define MAX_BUTTON 2

/* Register related */
#define TEMP_BIT 0 // from 0 to 4
#define BUTTON_BIT 6  // from 5 to 5

/* Function prototypes */
std::bitset<16> get_bit(std::bitset<16> binary_number, int bit);
int binary_bit_to_integer(std::bitset<16> binary_number, int start_bit, int stop_bit);

/* CoT Related declarations */

char ssid[] = "ARNardo";
char password[] =  "A1R2E3K4"; 
char server[] = "www.circusofthings.com";
char order_key[] = "15590"; 
char token[] = "eyJhbGciOiJIUzI1NiJ9.eyJqdGkiOiIyMDQ4In0.UJfXNTEHjAK2nywsuIMd8ZaZXGNIxYodu8zwAHwGyTg";

/* Object instantiations */
CircusESP32Lib circusESP32(server,ssid,password);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
 
void setup() {
    Serial.begin(115200); // Remember to match this value with the baud rate in your console
    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
      Serial.println(F("SSD1306 allocation failed"));
      for(;;);
    }
    circusESP32.begin(); // Let the Circus object set up itself for an SSL/Secure connection
}


void loop() { 
  int master_register_int = circusESP32.read(order_key, token);
  std::bitset<16> master_register(master_register_int);
  std::bitset<16> button_status_binary(get_bit(master_register,BUTTON_BIT));
  int button_status_int = button_status_binary.to_ulong();
  int temperature_int = binary_bit_to_integer(master_register, 0, 5);


  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);

  // Display static text
  display.print("Natural number");
  display.println(master_register_int);
  display.print("Button:");
  display.println(button_status_int);
  display.print("Temperature:");
  display.println(temperature_int);
  display.display(); 
}

std::bitset<16> get_bit(std::bitset<16> binary_number, int bit)
{
    std::cout << "We have: " << (binary_number >> 1) << " and: " << bit << " which is: " << ((binary_number >> bit) & std::bitset<16>(1)) << std::endl;
    return (binary_number >> bit) & std::bitset<16>(1);
}

int binary_bit_to_integer(std::bitset<16> binary_number, int start_bit, int stop_bit) {
    int result = 0;
    for (int i = start_bit; i < (stop_bit + 1); ++i) {
        std::bitset<16> i_bin(i);
        std::bitset<16> temp(get_bit(binary_number, i) << i);
        result += temp.to_ulong();
    }
    return result;
}