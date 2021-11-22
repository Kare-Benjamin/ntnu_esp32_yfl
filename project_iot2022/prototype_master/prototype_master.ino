/* Libraries */
#include <CircusESP32Lib.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

/* C++ specific */
#include <bitset>  
#include <iostream>  

#define BUTTON_PIN 5

#define MAX_TEMP 64
#define MAX_BUTTON 2

/* Register related */
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
    std::bitset<16> master_register;
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

    /* Declarations */
    int i = 0, temperature, temperature_binary, temperature_array_size;
    int cloud_register;
    int button_state;

    /* Read button */
    button_state = digitalRead(BUTTON_PIN); 

    /* Read Temperature */
    temperature = bme.readTemperature();  

    /* Make 16-bit master register and initialize with the temperature*/
    std::bitset<16> master_register(temperature);

    /* Write button_state to master register */
    master_register ^= (button_state << (BUTTON_BIT));

    /* Write to terminal */
    Serial.println("Master register has: ");
    std::cout << master_register.to_string('*') << "\n";
    Serial.println(master_register.to_ulong());
    circusESP32.write(order_key,master_register.to_ulong() ,token);
    
    //free(a);
    //*a = NULL;
    delay(1000);
}

/** Converts decimal numbers to binary 
 * 
 * @param [in] n the decimal number to be converted to binary
 * @param [in, out] i the loop iterator.
 * @param [in] a is the pointer to be manipulated.
 * @returns a the converted binary representation as a vector with i elements.
 */

int * convert_to_binary(int n, int &i, static int* a) { 
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