#include <buttonpress.h>

ButtonPress::ButtonPress(int buttonPin, bool pressedStateIs, unsigned long durationShort, unsigned long durationMedium, unsigned long durationLong) {
      pin = buttonPin;
      pressedState = pressedStateIs;
      pinMode(pin, INPUT);  // Set the pin as input
      pressStartTime = 0;
      buttonState = false;
      prevButtonState = false;
      isPressed = false;

      shortPressDuration = durationShort;
      mediumPressDuration = durationMedium;
      longPressDuration = durationLong;
    }

// Method to check button press type (short, medium, or long press)
int ButtonPress::checkPress() {
      buttonState = digitalRead(pin) == pressedState;  // Check if the button is in pressed state
      if (buttonState && !prevButtonState) {  // Button just pressed
        prevButtonState=true;
        pressStartTime = millis();  // Record the press start time
        isPressed = true;
        return -1;  // In progress, still pressing
      }

      if (!buttonState && prevButtonState) {  // Button just released
        prevButtonState=false;
        unsigned long pressDuration = millis() - pressStartTime;
        isPressed = false;
        // Determine the press duration type
        if(pressDuration>longPressDuration)
          return 3;
        else if (pressDuration>mediumPressDuration)
          return 2;
        else if (pressDuration>shortPressDuration)
          return 1; 
      }
      return 0;  // No press or still in progress
    }
