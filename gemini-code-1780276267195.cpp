#include "USB.h"
#include "USBHIDKeyboard.h"
#include <FastLED.h>

USBHIDKeyboard Keyboard;

const int NUM_ROWS = 5;
const int NUM_COLS = 14;

byte rowPins[NUM_ROWS] = {4, 5, 6, 7, 15}; 
byte colPins[NUM_COLS] = {1, 2, 3, 8, 9, 10, 11, 12, 13, 14, 16, 17, 18, 21};

#define LED_DATA_PIN   47
#define LED_POWER_PIN  3   
#define NUM_LEDS       68  
CRGB leds[NUM_LEDS];

uint16_t keymap[NUM_ROWS][NUM_COLS] = {
  {KEY_ESC, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', KEY_BACKSPACE},
  {KEY_TAB, 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', KEY_DELETE},
  {KEY_CAPS_LOCK, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', KEY_RETURN, KEY_PAGE_UP},
  {KEY_LEFT_SHIFT, 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', KEY_RIGHT_SHIFT, KEY_UP_ARROW, KEY_PAGE_DOWN},
  {KEY_LEFT_CTRL, KEY_LEFT_GUI, KEY_LEFT_ALT, ' ', ' ', ' ', KEY_RIGHT_ALT, KEY_RIGHT_CTRL, KEY_LEFT_ARROW, KEY_DOWN_ARROW, KEY_RIGHT_ARROW, 0, 0, 0}
};

bool keyState[NUM_ROWS][NUM_COLS];

void setup() {
  Serial.begin(115200);
  
  pinMode(LED_POWER_PIN, OUTPUT);
  digitalWrite(LED_POWER_PIN, LOW); 

  FastLED.addLeds<WS2812B, LED_DATA_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(50); 
  fill_solid(leds, NUM_LEDS, CRGB::Cyan); 
  FastLED.show();

  Keyboard.begin();
  USB.begin();

  for (int r = 0; r < NUM_ROWS; r++) {
    pinMode(rowPins[r], INPUT_PULLUP);
  }

  for (int c = 0; c < NUM_COLS; c++) {
    pinMode(colPins[c], OUTPUT);
    digitalWrite(colPins[c], HIGH);
  }

  for (int r = 0; r < NUM_ROWS; r++) {
    for (int c = 0; c < NUM_COLS; c++) {
      keyState[r][c] = false;
    }
  }
}

void loop() {
  for (int c = 0; c < NUM_COLS; c++) {
    digitalWrite(colPins[c], LOW);
    delayMicroseconds(30); 

    for (int r = 0; r < NUM_ROWS; r++) {
      bool isPressed = (digitalRead(rowPins[r]) == LOW);

      if (isPressed && !keyState[r][c]) {
        keyState[r][c] = true;
        if (keymap[r][c] != 0) {
          Keyboard.press(keymap[r][c]);
        }
      } 
      else if (!isPressed && keyState[r][c]) {
        keyState[r][c] = false;
        if (keymap[r][c] != 0) {
          Keyboard.release(keymap[r][c]);
        }
      }
    }

    digitalWrite(colPins[c], HIGH);
  }

  uint8_t thisHue = beatsin8(15, 0, 255); 
  fill_rainbow(leds, NUM_LEDS, thisHue, 4);
  FastLED.show();
  
  delay(2); 
}