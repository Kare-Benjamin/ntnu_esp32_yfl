#include "prototype_display_declarations.h"
#include "binary_helper_functions.cpp"

/* C++ specific */
#include <bitset>  
#include <iostream>

/* Function prototypes */

std::bitset<MASTER_BIT_WIDTH> get_bits(std::bitset<MASTER_BIT_WIDTH> binary_number, int bit);
int binary_bit_to_integer(std::bitset<MASTER_BIT_WIDTH> binary_number, int start_bit, int stop_bit);


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
  std::bitset<MASTER_BIT_WIDTH> master_register(master_register_int);
  std::bitset<MASTER_BIT_WIDTH> button_status_binary(get_bits(master_register,BUTTON_BIT));

  /* Translate master_register to individual variables */
  int node_id_int = binary_bit_to_integer(master_register, NODE_ID_MIN_BIT, NODE_ID_MAX_BIT) >> NODE_ID_MIN_BIT;
  int packet_id_int = binary_bit_to_integer(master_register, NODE_PACKET_ID_MIN_BIT, NODE_PACKET_ID_MAX_BIT) >> NODE_PACKET_ID_MIN_BIT;
  int temperature_error_int = binary_bit_to_integer(master_register, NODE_TEMPERATURE_ERROR_MIN_BIT, NODE_TEMPERATURE_ERROR_MAX_BIT) >> NODE_TEMPERATURE_ERROR_MIN_BIT;
  int temperature_int = binary_bit_to_integer(master_register, NODE_TEMPERATURE_MIN_BIT, NODE_TEMPERATURE_MAX_BIT) >> NODE_TEMPERATURE_MIN_BIT;
  int humidity_int = binary_bit_to_integer(master_register, NODE_HUMIDITY_MIN_BIT, NODE_HUMIDITY_MAX_BIT) >> NODE_HUMIDITY_MIN_BIT;
  int humidity_error_int = binary_bit_to_integer(master_register, NODE_HUMIDITY_ERROR_MIN_BIT, NODE_HUMIDITY_ERROR_MAX_BIT) >> NODE_HUMIDITY_ERROR_MIN_BIT;
  int button_status_int = binary_bit_to_integer(master_register, NODE_BUTTON_MIN_BIT, NODE_BUTTON_MAX_BIT) >> NODE_BUTTON_MIN_BIT;
  int button_error_status_int = binary_bit_to_integer(master_register, NODE_BUTTON_ERROR_MIN_BIT, NODE_BUTTON_ERROR_MAX_BIT) >> NODE_BUTTON_ERROR_MIN_BIT;

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);

  // Display static text
  display.print("Source:");
  display.print(node_id_int);
  display.print(" packet:");
  display.println(packet_id_int);
  display.print("Natural: ");
  display.println(master_register_int);
  display.print("Button status:(");
  display.print(button_status_int);
  display.print(","); 
  display.print(button_error_status_int);
  display.println(")"); 
  display.print("Temperature:(");
  display.print(temperature_int);
  display.print(",");
  display.print(temperature_error_int);
  display.println(")"); 
  display.print("Humidity:(");
  display.print(humidity_int);
  display.print(",");
  display.print(humidity_error_int);
  display.println(")"); 
  display.display(); 

  // OLED
  Serial.print("Source:");
  Serial.print(node_id_int);
  Serial.print(" packet:");
  Serial.println(packet_id_int);
  Serial.print("Natural: ");
  Serial.println(master_register_int);
  Serial.print("Button status:(");
  Serial.print(button_status_int);
  Serial.print(","); 
  Serial.print(button_error_status_int);
  Serial.println(")"); 
  Serial.print("Temperature:(");
  Serial.print(temperature_int);
  Serial.print(",");
  Serial.print(temperature_error_int);
  Serial.println(")"); 
  Serial.print("Humidity:(");
  Serial.print(humidity_int);
  Serial.print(",");
  Serial.print(humidity_error_int);
  Serial.println(")"); 
}

std::bitset<MASTER_BIT_WIDTH> get_bits(std::bitset<MASTER_BIT_WIDTH> binary_number, int bit)
{
    //std::cout << "We have: " << (binary_number >> 1) << " and: " << bit << " which is: " << ((binary_number >> bit) & std::bitset<MASTER_BIT_WIDTH>(1)) << std::endl;
    return (binary_number >> bit) & std::bitset<MASTER_BIT_WIDTH>(1);
}

int binary_bit_to_integer(std::bitset<MASTER_BIT_WIDTH> binary_number, int start_bit, int stop_bit) {
    int result = 0;
    for (int i = start_bit; i < (stop_bit + 1); ++i) {
        std::bitset<MASTER_BIT_WIDTH> i_bin(i);
        std::bitset<MASTER_BIT_WIDTH> temp(get_bits(binary_number, i) << i);
        result += temp.to_ulong();
    }
    return result;
}