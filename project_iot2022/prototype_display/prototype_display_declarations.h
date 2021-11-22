#ifndef PROTOTYPE_DISPLAY_DECLARATIONS
#define PROTOTYPE_DISPLAY_DECLARATIONS

/* Libraries */
#include <CircusESP32Lib.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

/* Defines */
#define MAX_TEMP 64
#define MAX_BUTTON 2

#define MASTER_BIT_WIDTH 32

/* Register related */
#define TEMP_BIT 0 // from 0 to 4
#define BUTTON_BIT 6  // from 5 to 5

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define BUTTON_BIT 6

/* Node 1 Packet Info */
/** Register related **/
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

#endif