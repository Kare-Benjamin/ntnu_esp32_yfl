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

/* CoT Related declarations */

char ssid[] = "ARNardo"; // Place your wifi SSID here
char password[] =  "A1R2E3K4"; // Place your wifi password here
char server[] = "www.circusofthings.com";
char order_key[] = "15590"; // Type the Key of the Circus Signal you want the ESP32 listen to. 
char token[] = "eyJhbGciOiJIUzI1NiJ9.eyJqdGkiOiIyMDQ4In0.UJfXNTEHjAK2nywsuIMd8ZaZXGNIxYodu8zwAHwGyTg"; // Place your token, find it in 'account' at Circus. It will identify you.
CircusESP32Lib circusESP32(server,ssid,password); // The object representing an ESP32 to whom you can order to Write or Read

Adafruit_BME280 bme;

int* convert_to_binary(int n, int &i, int*a);
int array_to_int(int* a, int& i);
bool fullAdder(bool b1, bool b2, bool& carry);
std::bitset<MASTER_BIT_WIDTH> bitsetAdd(std::bitset<32>& x, std::bitset<32>& y);

void setup() {
    Serial.begin(115200); // Remember to match this value with the baud rate in your console
    Serial.print("Checking");
    circusESP32.begin(); // Let the Circus object set up itself for an SSL/Secure connection

    pinMode(BUTTON_PIN, INPUT);

    bool status;
    
    /* Initializing EEPROM with EEPROM_SIZE bits*/ 

    EEPROM.begin(0x20);
    delay(2000);

    EEPROM.write(0, 1);
    EEPROM.commit();


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

    std::bitset<NODE_PACKET_ID_MAX_BIT - NODE_PACKET_ID_MIN_BIT> message_nr_temp(EEPROM.read(0));
    std::bitset<MASTER_BIT_WIDTH> node_id(1);
    std::bitset<NODE_TEMPERATURE_ERROR_MIN_BIT - NODE_TEMPERATURE_ERROR_MIN_BIT + 1> temperature_error(1);
    std::bitset<NODE_TEMPERATURE_ERROR_MIN_BIT - NODE_TEMPERATURE_ERROR_MIN_BIT + 1> humidity_error(1);
    std::bitset<NODE_TEMPERATURE_ERROR_MIN_BIT - NODE_TEMPERATURE_ERROR_MIN_BIT + 1> button_error(1);

    /* Declarations */
    int i = 0, temperature_binary, temperature_array_size;
    int cloud_register;
    int button_state;

    /* Read button */
    button_state = digitalRead(BUTTON_PIN); 

    /* Read Temperature */
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
    
    //free(a);
    //*a = NULL;
    std::cout << "message_nr: " << message_nr_temp << std::endl;
    message_nr_temp ^= (message_nr_temp << (1));
    std::cout << "after bitshift: " << message_nr_temp << std::endl;
    std::cout << "Master message_nr: " << message_nr_temp << std::endl;

    EEPROM.write(0, message_nr_temp.to_ulong());
    EEPROM.commit();

    delay(3000); // Letting the reciever catch up
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