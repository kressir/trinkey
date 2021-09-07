#include <Adafruit_NeoPixel.h>
#include "Adafruit_FreeTouch.h"
#include "HID-Project.h"  // https://github.com/NicoHood/HID
#include "ClickButton.h"
#include <FlashStorage.h>

ClickButton button1(PIN_SWITCH, HIGH, LOW);
// Create the neopixel strip with the built in definitions NUM_NEOPIXEL and PIN_NEOPIXEL
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_NEOPIXEL, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800);
// Create the touch pad
Adafruit_FreeTouch qt = Adafruit_FreeTouch(PIN_TOUCH, OVERSAMPLE_4, RESISTOR_50K, FREQ_MODE_NONE);
int16_t neo_brightness = 20; // initialize with 20 brightness (out of 255)
int intDir = 1;
unsigned long intLastInc = 0;

typedef struct {
  char stuff[100];
} flashData;
FlashStorage(my_flash_store, flashData);
flashData fd;

int writeStop = -1;

void setup() {
  Serial.begin(9600);
  button1.debounceTime   = 25;   // Debounce timer in ms
  button1.multiclickTime = 300;  // Time limit for multi clicks
  button1.longClickTime  = 700; // time until "held-down clicks" register
  
  pinMode(PIN_SWITCH, INPUT_PULLDOWN);
  //while (!Serial);
  strip.begin();
  strip.setBrightness(neo_brightness);
  strip.show(); // Initialize all pixels to 'off'

  if (! qt.begin())
    Serial.println("Failed to begin qt");
  Keyboard.begin();
  Mouse.begin();
  fd = my_flash_store.read();
}

unsigned long lp=0;
unsigned long lastMouse=0;
bool prnt = false;
bool wiggle = false;
bool prog = false;

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
      prog = false;
      break;
    case 2:
      wiggle = !wiggle;
      break;
    case -1:
      Keyboard.println(fd.stuff);
      break;
    case -2:
      Keyboard.press(KEY_LEFT_CTRL);
      delay(100);
      Keyboard.releaseAll();
      delay(500);
      Keyboard.println(fd.stuff);
      break;
    case -5:
      prog = true;
      writeStop = -1;
      break;
    default:
      break;
  }

  // measure the captouches
  uint16_t touch = qt.measure();
  //if (prnt) {
  //  Serial.println(touch);
  //}
  if (touch>500){
    if(millis()-intLastInc>20){
      intLastInc = millis();
      neo_brightness = neo_brightness + intDir;
      if (neo_brightness == 255) intDir = -1;
      if (neo_brightness == 0) intDir = 1;
    }
  }

  int incomingByte;
  if (prog){
    //use echo asd>COM6
    if (Serial.available() > 0) {
      while (Serial.available() > 0) {
        incomingByte = Serial.read();
        if(incomingByte==10){
          //flush serial
          delay(10);
          while (Serial.available() > 0) {
            incomingByte = Serial.read();
          }
          prog = false;
          writeStop = -1;
          fd.stuff[writeStop] = 0;
          my_flash_store.write(fd);
        }else{
          writeStop++;
          fd.stuff[writeStop] = incomingByte;
        } 
      }
    }
  }
  
  if (wiggle) {
    if(millis()-lastMouse > 300000){
      Mouse.move(1, 0, 0);
      delay(10);
      Mouse.move(-1, 0, 0);
      lastMouse = millis();
    }
  }

  //set the noepixel color
  if(prog){
    strip.setPixelColor(0, strip.Color(255, 0, 0));    
    strip.setBrightness(neo_brightness);
  }else if(wiggle) {
    strip.setPixelColor(0, strip.Color(0, 0, 255));
    strip.setBrightness(neo_brightness);    
  }else{
    strip.setBrightness(0);    
  }

  strip.show();
}
