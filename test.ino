#include <Adafruit_NeoPixel.h>
#include "Adafruit_FreeTouch.h"
#include "HID-Project.h"  // https://github.com/NicoHood/HID
#include "ClickButton.h"

ClickButton button1(PIN_SWITCH, HIGH, LOW);

// Create the neopixel strip with the built in definitions NUM_NEOPIXEL and PIN_NEOPIXEL
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_NEOPIXEL, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800);

// Create the touch pad
Adafruit_FreeTouch qt = Adafruit_FreeTouch(PIN_TOUCH, OVERSAMPLE_4, RESISTOR_50K, FREQ_MODE_NONE);

int16_t neo_brightness = 20; // initialize with 20 brightness (out of 255)

void setup() {
  //Serial.begin(9600);
  button1.debounceTime   = 25;   // Debounce timer in ms
  button1.multiclickTime = 300;  // Time limit for multi clicks
  button1.longClickTime  = 800; // time until "held-down clicks" register
  
  pinMode(PIN_SWITCH, INPUT_PULLDOWN);
  //while (!Serial);
  strip.begin();
  strip.setBrightness(neo_brightness);
  strip.show(); // Initialize all pixels to 'off'

  //if (! qt.begin())
  //  Serial.println("Failed to begin qt");
  Keyboard.begin();
  Mouse.begin();
}

unsigned long lp=0;
unsigned long lastMouse=0;
bool prnt = false;
bool wiggle = false;

void loop() {
  
  if(millis()-lp > 500){
    prnt=true;
    lp=millis();
  }
  else{
    prnt=false;
  }
  
  button1.Update();
  switch (button1.clicks) {
    case 1:
      break;
    case 2:
      wiggle = !wiggle;
      break;
    case -1:
      Keyboard.println(" ");
      break;
    case -2:
      Keyboard.press(KEY_LEFT_CTRL);
      delay(100);
      Keyboard.releaseAll();
      delay(1000);
      Keyboard.println(" ");
      break;
    default:
      // statements
      break;
  }
  //if (button1.clicks != 0) {
    //Serial.println("Clicks: " + (String)button1.clicks);
  //}

  // measure the captouches
  uint16_t touch = qt.measure();
  
  if (wiggle) {
    strip.setBrightness(neo_brightness);
    if(millis()-lastMouse > 300000){
      Mouse.move(1, 0, 0);
      delay(10);
      Mouse.move(-1, 0, 0);
      lastMouse = millis();
    }
  } else {
    strip.setBrightness(0);
  }

  strip.setPixelColor(0, strip.Color(0, 0, 255));
  strip.show();
}
