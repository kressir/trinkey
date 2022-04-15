#include <Adafruit_NeoPixel.h>
#include "Adafruit_FreeTouch.h"
#include "HID-Project.h"  // https://github.com/NicoHood/HID
#include "ClickButton.h"
#include <FlashStorage.h>
/*
 * 
 * 1 click flashes led for number of ms based on the last of these events:
 *   - last number of double-clicks
 *   - how long touchpad was pressed
 * double-click toggles wiggle (slight mouse movement every 5 minutes)
 * wiggle stays on for a number of hours based on the count of clicks
 * double-click keeps it on for 2 hours, triple for 3 hours, 49-click for 49 hours, etc.
 * long single click types store1 and <enter>
 * long double-click types a ctrl key, pauses, then types store1 and <enter> - good for waking and unlocking
 * long 3-click types store2 and <enter>
 * long 4-click sets store2
 * long 5-click sets store1
 * long 11-click prints help
 * must wait 2 seconds between uses
 * 
 */
ClickButton btnKey(PIN_SWITCH, HIGH, cb_pinMode::cb_INPUT_PULLDOWN);
ClickButton touchPad(0, HIGH, cb_pinMode::cb_NO_PIN);
// Create the neopixel strip with the built in definitions NUM_NEOPIXEL and PIN_NEOPIXEL
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_NEOPIXEL, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800);
// Create the touch pad
Adafruit_FreeTouch qt = Adafruit_FreeTouch(PIN_TOUCH, OVERSAMPLE_4, RESISTOR_50K, FREQ_MODE_NONE);
int16_t neo_brightness = 20; // initialize with 20 brightness (out of 255)
int intDir = 1;
unsigned long intLastInc = 0;
unsigned long intLastTouch = 0;

typedef struct {
  char stuff[100];
} flashData;
FlashStorage(my_flash_store, flashData);
FlashStorage(my_flash_store2, flashData);
flashData fd;
flashData fd2;

int writeStop = -1;

unsigned long flashTime=0;
unsigned long lastKB=0;
unsigned long lp=0;
unsigned long lastMouse=0;
unsigned long wiggleStart=0;
bool prnt = false;
bool wiggle = false;
bool prog = false;
bool prog2 = false;
bool touching = false;
int wiggleHours = 0;
bool booting = true;

void setup() {
  Serial.begin(9600);
  btnKey.debounceTime   = 20;   
  btnKey.multiclickTime = 250;  
  btnKey.longClickTime  = 500; 
  touchPad.debounceTime   = 50;   
  touchPad.multiclickTime = 350;  
  touchPad.longClickTime  = 1000; 
  
  strip.begin();
  strip.setBrightness(neo_brightness);
  strip.show(); // Initialize all pixels to 'off'

  if (! qt.begin())
    Serial.println("Failed to begin qt");
  Keyboard.begin();
  Mouse.begin();
  fd = my_flash_store.read();
  fd2 = my_flash_store2.read();

  delay(500);
  if(digitalRead(PIN_SWITCH)!=HIGH){
    fd.stuff[0] = 0;
    my_flash_store.write(fd);
    fd2.stuff[0] = 0;
    my_flash_store2.write(fd2);
  }
  flashTime=millis()+1000;
}

int playTime = 5;
void loop() {
  
  if(millis()-lp > 500){
    prnt=true;
    lp=millis();
  }
  else{
    prnt=false;
  }
  
  if(booting && digitalRead(PIN_SWITCH)==HIGH){
    flashTime=millis()+100;
  }else{
    booting = false;
  }
  
  int incomingByte;
  btnKey.Update();
  if(!booting){
    switch (btnKey.clicks) {
      case 1:
        prog = false;
        flashTime = millis()+playTime;
        break;
      case -1:
        if(millis()>lastKB+2000){
          Keyboard.println(fd.stuff);
          flashTime = millis()+100;
          lastKB = millis();
        }
        break;
      case -2:
        if(millis()>lastKB+2000){
          Keyboard.press(KEY_LEFT_CTRL);
          delay(10);
          Keyboard.releaseAll();
          delay(300);
          Keyboard.println(fd.stuff);
          flashTime = millis()+100;
          lastKB = millis();
        }
        break;
      case -3:
        if(millis()>lastKB+2000){
          Keyboard.println(fd2.stuff);
          flashTime = millis()+100;
          lastKB = millis();
        }
        break;
      case -4:
        prog2 = true;
        writeStop = -1;
        //purge the buffer initially
        delay(10);
        while (Serial.available() > 0) {
          incomingByte = Serial.read();
        }
        break;
      case -5:
        prog = true;
        writeStop = -1;
        //purge the buffer initially
        delay(10);
        while (Serial.available() > 0) {
          incomingByte = Serial.read();
        }
        break;
      case -11:
        if(millis()>lastKB+2000){
          Keyboard.println("The Rick clicker neo key");
          Keyboard.println("  1 click flashes led for number of ms based on the last of these events:");
          Keyboard.println("    - last number of double-clicks");
          Keyboard.println("    - how long touchpad was pressed");
          Keyboard.println("  double-click toggles wiggle (slight mouse movement every 5 minutes)");
          Keyboard.println("  wiggle stays on for a number of hours based on the count of clicks");
          Keyboard.println("  led will glow blue when wiggle is on");
          Keyboard.println("  brightness can be changed by holding the touch pad - double clicking the touch pad will toggle full dim/full bright");
          Keyboard.println("  double-click keeps it on for 2 hours, triple for 3 hours, 49-click for 49 hours, etc.");
          Keyboard.println("  long single click types store1 and <enter>");
          Keyboard.println("  long double-click types a ctrl key, pauses, then types store1 and <enter> - good for waking and unlocking");
          Keyboard.println("  long 3-click types store2 and <enter>");
          Keyboard.println("  long 4-click sets store2");
          Keyboard.println("  long 5-click sets store1");
          Keyboard.println("  to send text to a store, you must know the com port for the neo key...");
          Keyboard.println("    in a cmd prompt, type the following:");
          Keyboard.println("    echo this is the text to store!>com5");
          Keyboard.println("  the neo key will light up a different color when in store mode and the light will change when store is successful");
          Keyboard.println("  if the neo key is removed and re-inserted or pc is shut down or hibernated, storage will be deleted ");
          Keyboard.println("    unless holding in the key while plugging in and holding for 2 seconds");
          Keyboard.println("  a green constant light is an indication of a blank store");
          Keyboard.println("  long 11-click prints help");
          Keyboard.println("  must wait 2 seconds between uses");
          flashTime = millis()+100;
          lastKB = millis();
        }
        break;
      default:
        if(btnKey.clicks>1){
          playTime = btnKey.clicks;
          wiggle = !wiggle;
          if (wiggle){
            wiggleStart = millis();
            wiggleHours = btnKey.clicks;
          }
        }
        break;
    }
  }

  // measure the captouches
  uint16_t touch = qt.measure();
  //check the touch level and create a debounced button for it
  touchPad.Update(touch>350?HIGH:LOW);
  if (touchPad.depressed){
    if(millis()-intLastInc>15 && millis()>touchPad.timeDown+500){
      intLastInc = millis();
      neo_brightness = neo_brightness + intDir;
      if (neo_brightness >= 255) intDir = -1;
      if (neo_brightness <= 0) intDir = 1;
    }
    playTime = millis()-touchPad.timeDown;
  }
  if (touchPad.clicks==2){
    if (neo_brightness<125) neo_brightness=255;
    else neo_brightness=1;
  }

  if (prog || prog2){
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
          if(prog){
            fd.stuff[writeStop] = 0;
            my_flash_store.write(fd);
            prog = false;
          }else{
            fd2.stuff[writeStop] = 0;
            my_flash_store2.write(fd2);
            prog2 = false;
          }
          writeStop = -1;
        }else{
          writeStop++;
          if(prog){
            fd.stuff[writeStop] = incomingByte;
          }else{
            fd2.stuff[writeStop] = incomingByte;
          }
        } 
      }
    }
  }
  
  if (wiggle) {
    if(millis() > wiggleStart+(1000*60*60*wiggleHours)){
      //only leave it on for x hours
      wiggle=false;
      flashTime = millis()+1000;
    }
    if(millis()-lastMouse > 300000){
      Mouse.move(1, 0, 0);
      delay(10);
      Mouse.move(-1, 0, 0);
      lastMouse = millis();
    }
  }

  //set the noepixel color
  
  if(flashTime> millis()){
    strip.setPixelColor(0, strip.Color(255, 255, 255));    
    strip.setBrightness(255);
  }else if(prog){
    strip.setPixelColor(0, strip.Color(255, 0, 0));    
    strip.setBrightness(255);
  }else if(prog2){
    strip.setPixelColor(0, strip.Color(255, 255, 0));    
    strip.setBrightness(255);
  }else if(wiggle) {
    strip.setPixelColor(0, strip.Color(0, 0, 255));
    strip.setBrightness(neo_brightness);    
  }else if(fd.stuff[0] == 0){
    strip.setPixelColor(0, strip.Color(0, 255, 0));  
    strip.setBrightness(neo_brightness);    
  }else{
    strip.setBrightness(0);    
  }

  strip.show();
}
