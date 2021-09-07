#ifndef ClickButton_H
#define ClickButton_H

#if (ARDUINO <  100)
#include <WProgram.h>
#else
#include <Arduino.h>
#endif


#define CLICKBTN_PULLUP HIGH

//example: ClickButton button1(PIN_SWITCH, HIGH, cb_pinMode::cb_INPUT_PULLDOWN);

enum cb_pinMode { cb_INPUT = INPUT, cb_INPUT_PULLUP = INPUT_PULLUP, cb_INPUT_PULLDOWN = INPUT_PULLDOWN, cb_NO_PIN };

class ClickButton
{
  public:
    ClickButton(uint8_t buttonPin);
    ClickButton(uint8_t buttonPin, boolean active);
    ClickButton(uint8_t buttonPin, boolean active, cb_pinMode pinMode);
    void Update();
    void Update(boolean btnState);
    int clicks;                   // button click counts to return
    boolean depressed;            // the currently debounced button (press) state (presumably it is not sad :)
    unsigned long debounceTime;
    unsigned long multiclickTime;
    unsigned long longClickTime;
    unsigned long timeDown;
    unsigned long timeUp;
	boolean inProcess;
  private:
    uint8_t _pin;                 // Arduino pin connected to the button
    boolean _activeHigh;          // Type of button: Active-low = 0 or active-high = 1
    boolean _btnState;            // Current appearant button state
    boolean _lastState;           // previous button reading
    int _clickCount;              // Number of button clicks within multiclickTime milliseconds
    unsigned long _lastBounceTime;         // the last time the button input pin was toggled, due to noise or a press
};

#endif

