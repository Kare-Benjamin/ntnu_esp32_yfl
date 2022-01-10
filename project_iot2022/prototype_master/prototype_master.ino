/* Libraries */
#include <CircusESP32Lib.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <EEPROM.h>
#define EEPROM_SIZE 1


/* C++ specific */
#include <bitset>  
#include <iostream>  

#define BUTTON_PIN 5

#define MAX_TEMP 64
#define MAX_BUTTON 2
#define MASTER_BIT_WIDTH 32

/* Register related */
#define NODE_ID_MIN_BIT 0
#define NODE_ID_MAX_BIT 2

#define NODE_PACKET_ID_MIN_BIT 3
#define NODE_PACKET_ID_MAX_BIT 6

#define NODE_TEMPERATURE_MIN_BIT 7
#define NODE_TEMPERATURE_MAX_BIT 12

#define NODE_TEMPERATURE_ERROR_MIN_BIT 13
#define NODE_TEMPERATURE_ERROR_MAX_BIT 13

#define NODE_HUMIDITY_MIN_BIT 14
#define NODE_HUMIDITY_MAX_BIT 19

#define NODE_HUMIDITY_ERROR_MIN_BIT 20
#define NODE_HUMIDITY_ERROR_MAX_BIT 20

#define NODE_BUTTON_MIN_BIT 21
#define NODE_BUTTON_MAX_BIT 21

#define NODE_BUTTON_ERROR_MIN_BIT 22
#define NODE_BUTTON_ERROR_MAX_BIT 22

#define TEMP_BIT 0 // from 0 to 4
#define BUTTON_BIT 6  // from 5 to 5

/* node ID based on MAC */
#ifndef TEST_H
#define TEST_H
  std::array<uint8_t, 6> NODE_ONE_MAC = {36, 111, 40, 158, 159, 92};
#endif

/* For sleep */
#define RATIO_MILIONTH_TO_ONE 1000000  /* Conversion factor for micro seconds to seconds */
#define SLEEP_DURATION  5        /* Time ESP32 will go to sleep (in seconds) */

/* CoT Related declarations */

char ssid[] = "Get-2G-22F101";
char password[] =  "emm5utz3u2"; 
char server[] = "www.circusofthings.com";
char order_key[] = "15590"; // Type the Key of the Circus Signal you want the ESP32 listen to. 
char token[] = "eyJhbGciOiJIUzI1NiJ9.eyJqdGkiOiIyMDQ4In0.UJfXNTEHjAK2nywsuIMd8ZaZXGNIxYodu8zwAHwGyTg"; // Place your token, find it in 'account' at Circus. It will identify you.
CircusESP32Lib circusESP32(server,ssid,password); // The object representing an ESP32 to whom you can order to Write or Read

Adafruit_BME280 bme;

int* convert_to_binary(int n, int &i, int*a);
int array_to_int(int* a, int& i);
bool fullAdder(bool b1, bool b2, bool& carry);
std::bitset<MASTER_BIT_WIDTH> bitsetAdd(std::bitset<32>& x, std::bitset<32>& y);
int determine_node_id();

/* Boot counter, non volatiles */
RTC_DATA_ATTR int number_of_sleeps = 0;
RTC_DATA_ATTR int node_id_non_volatile = determine_node_id();
RTC_DATA_ATTR int message_number_rtc = 1;

void setup() {

    Serial.begin(115200); // Remember to match this value with the baud rate in your console
    Serial.print("Checking");
    circusESP32.begin(); // Let the Circus object set up itself for an SSL/Secure connection

    pinMode(BUTTON_PIN, INPUT);

    bool status;

    // default settings
    // (you can also pass in a Wire library object like &Wire2)
    status = bme.begin(0x76);  
    if (!status) {
     Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
    
  }
}
/* 
void loop() { 
    std::bitset<MASTER_BIT_WIDTH> master_register;
    master_register = 1;

    int i = 0, temperature, temperature_binary, temperature_array_size;
    int cloud_register;
    int button_state;
    button_state = digitalRead(BUTTON_PIN); 
    temperature = bme.readTemperature();  
    temperature_array_size = log2(MAX_TEMP) + log2(MAX_BUTTON) + 1;
    static int* a = (int*)malloc(temperature_array_size * sizeof(int));
    a = convert_to_binary(temperature, i, a);
    cloud_register = array_to_int(a,i);
    //cloud_register |= button_state << (BUTTON_BIT+1);
    cloud_register += button_state * 100000;
    //cloud_register ^= (-button_state ^ cloud_register) & (1UL << (BUTTON_BIT+2));
    circusESP32.write(order_key,master_register.to_ulong() ,token);
    Serial.println(cloud_register);
    //free(a);
    //*a = NULL;
    delay(1000);
}
*/

void loop() { 

    std::bitset<NODE_PACKET_ID_MAX_BIT - NODE_PACKET_ID_MIN_BIT> message_nr_temp(message_number_rtc);
    std::bitset<MASTER_BIT_WIDTH> node_id(node_id_non_volatile);
    std::bitset<NODE_TEMPERATURE_ERROR_MIN_BIT - NODE_TEMPERATURE_ERROR_MIN_BIT + 1> temperature_error(1);
    std::bitset<NODE_TEMPERATURE_ERROR_MIN_BIT - NODE_TEMPERATURE_ERROR_MIN_BIT + 1> humidity_error(1);
    std::bitset<NODE_TEMPERATURE_ERROR_MIN_BIT - NODE_TEMPERATURE_ERROR_MIN_BIT + 1> button_error(1);

    /* Declarations */
    int i = 0, temperature_binary, temperature_array_size;
    int cloud_register;
    int button_state;

    /* Read button */
    button_state = digitalRead(BUTTON_PIN); 

    /* Read Temperature, Humidity and Preassure */
    std::bitset<MASTER_BIT_WIDTH> temperature(bme.readTemperature());  
    std::bitset<MASTER_BIT_WIDTH> humidity(bme.readHumidity());
    std::bitset<MASTER_BIT_WIDTH> message_nr(message_nr_temp.to_ulong());

    /* Make MASTER_BIT_WIDTH-bit master register and initialize with the node id*/
    std::bitset<MASTER_BIT_WIDTH> master_register(node_id);

    /* Write packet id to master register */
    master_register = bitsetAdd(master_register, message_nr <<= NODE_PACKET_ID_MIN_BIT);

    /* Write temperature to master register */
    master_register = bitsetAdd(master_register, temperature <<= NODE_TEMPERATURE_MIN_BIT );

    /* Write temperature error to master register */
    master_register ^= (temperature_error.to_ulong() << (NODE_TEMPERATURE_ERROR_MIN_BIT ));

    /* Write humidity to master register */
    master_register = bitsetAdd(master_register, humidity <<= NODE_HUMIDITY_MIN_BIT );

    /* Write humidity error to master register */
    master_register ^= (humidity_error.to_ulong() << (NODE_HUMIDITY_ERROR_MIN_BIT ));

    /* Write button_state to master register */
    master_register ^= (button_state << (NODE_BUTTON_MIN_BIT));

    /* Write button_state error to master register */
    master_register ^= (button_error.to_ulong() << (NODE_BUTTON_ERROR_MIN_BIT));

    /* Write to terminal */
    Serial.println("Master register has: ");
    std::cout << master_register.to_string('*') << "\n";
    Serial.println(master_register.to_ulong());
    circusESP32.write(order_key,master_register.to_ulong() ,token);

    /* Increment the message counter in RTC memory by one bitshift */
    message_number_rtc ^= (message_number_rtc << (1));
    std::cout << "Master message_nr: " << message_nr_temp << std::endl;

    /* Sleep related */
    ++number_of_sleeps;
    Serial.println("Deep sleep number: " + String(number_of_sleeps));
  
    /* The reason for the wakeup */
    Serial.print("ESP woke up from sleep: ");
    std::cout << source_of_wakeup() << std::endl;

    /* Setting up the sleep duration */
    esp_sleep_enable_timer_wakeup(SLEEP_DURATION * RATIO_MILIONTH_TO_ONE);
    Serial.println("Sleep will last" + String(SLEEP_DURATION) +
    " Seconds");

    /* Commencing the sleep */
    Serial.println("Deep Sleep initiated");
    Serial.flush(); 
    esp_deep_sleep_start();
    //delay(5000);

    /* Code ends here, because the ESP32 is in sleep */
}

/** Converts decimal numbers to binary 
 * 
 * @param [in] n the decimal number to be converted to binary
 * @param [in, out] i the loop iterator.
 * @param [in] a is the pointer to be manipulated.
 * @returns a the converted binary representation as a vector with i elements.
 */

int * convert_to_binary(int n, int &i, int* a) { 
	for (i; n > 0; i++)
	{
		a[i] = n % 2;
		n = n / 2;
	}
	return a;

}

/** Converts an integer array to am integer scalar. All vector elements are adjusted for their significance,
 * where each elements is multiplied with 10 to the power of i, then the results for all vector elements are summed.
 * 
 * @param [in] a the vector with i elements that should be converted to a binary number.
 * @param [in, out] i the loop iterator.
 * @returns result is the integer containing all vector elements.
 */

int array_to_int(int* a, int& i) {
	int result = 0;
	for (i = i - 1; i >= 0; i--)
	{
		result += (*(a + i)) * pow(10,i);
	}
	return result;
}

bool fullAdder(bool b1, bool b2, bool& carry)
{
    bool sum = (b1 ^ b2) ^ carry;
    carry = (b1 && b2) || (b1 && carry) || (b2 && carry);
    return sum;
}
// Function to add two bitsets
std::bitset<MASTER_BIT_WIDTH> bitsetAdd(std::bitset<MASTER_BIT_WIDTH>& x, std::bitset<MASTER_BIT_WIDTH>& y)
{
    bool carry = false;
    // bitset to store the sum of the two bitsets
    std::bitset<MASTER_BIT_WIDTH> ans;
    for (int i = 0; i < 32; i++) {
        ans[i] = fullAdder(x[i], y[i], carry);
    }
    return ans;
}

/* Determines the reason for the ESP32 wakeup.
 * @return the cause of the wakeup as a string.
 */
std::string source_of_wakeup(){
  esp_sleep_wakeup_cause_t wakeup_source;
  wakeup_source = esp_sleep_get_wakeup_cause();

  switch(wakeup_source){
    case ESP_SLEEP_WAKEUP_EXT0 : return "RTC_IO";
    case ESP_SLEEP_WAKEUP_EXT1 : return "RTC_CNTL";
    case ESP_SLEEP_WAKEUP_TIMER : return "TIMER";
    case ESP_SLEEP_WAKEUP_TOUCHPAD : return "TIMER";
    case ESP_SLEEP_WAKEUP_ULP : return "TIMER";
    default : return "OTHER"; 
  }
}

/* Detects the node name based on known IP-addresses.
 * 
 */

 int determine_node_id(){

  byte mac_address_hex[6];
  WiFi.macAddress(mac_address_hex);
  std::array<uint8_t, 6> mac_address_array;
  for (int i = 0; i < 6; ++i){
    mac_address_array[i] = mac_address_hex[i];
  }
   
  Serial.print("We have MAC_ADDRESS");
  for (int i = 0; i < 6; ++i){
    Serial.print(mac_address_hex[i]);
    Serial.print(":");
  }
  Serial.println();

  if (mac_address_array == NODE_ONE_MAC){
    Serial.println("Given node ID 1");
    return 1;  
  }

  else {
    Serial.println("The MAC was not recognised");
    Serial.println("Given node ID 7");
    return 7;
  }

  /*
  //string mac_address = WiFi.macAddress();
    switch(mac_address_hex){
    case NODE_ONE : return 1;
    default : return 7;
  }*/
 }
