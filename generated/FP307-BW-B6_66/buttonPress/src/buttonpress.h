#ifndef BUTTONPRESS_H_
#define BUTTONPRESS_H_
#include <Arduino.h>

#define BUTTONPRESS_STILL_PRESSING -1
#define BUTTONPRESS_NO_PRESSING 0
#define BUTTONPRESS_SHORT_PRESSING 1
#define BUTTONPRESS_MEDIUM_PRESSING 2
#define BUTTONPRESS_LONG_PRESSING 3


class ButtonPress {
  private:
    int pin;               // Pin number for the button
    bool pressedState;     // Whether pressed is HIGH or LOW
    unsigned long pressStartTime;  // Time when button was first pressed
    bool buttonState;      // To keep track of the button state (pressed or not)
    bool prevButtonState;  // Previous state of the button
    bool isPressed;        // Whether the button is currently pressed

    unsigned long shortPressDuration;  // Duration for short press
    unsigned long mediumPressDuration; // Duration for medium press
    unsigned long longPressDuration;   // Duration for long press

  public:
    // Constructor: Initializes the pin, pressed state, and dynamic press durations
    ButtonPress(int buttonPin, bool pressedStateIs, unsigned long durationShort = 1000, unsigned long durationMedium = 2500, unsigned long durationLong = 5000);
    
    // Method to check button press type (short, medium, or long press)
    int checkPress();
};

#endif
