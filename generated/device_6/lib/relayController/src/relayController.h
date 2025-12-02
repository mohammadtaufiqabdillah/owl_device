#ifndef RELAYCONTROLLER_H
#define RELAYCONTROLLER_H
#include <Arduino.h>

class RelayController {
  private:
    uint8_t relayPins[8];      // Pins where the relays are connected
    uint8_t lastCommand;
    bool relayStatus[8];   // Array to hold the relay statuses (true = ON, false = OFF)
    bool relayOnState;     // Whether the relay is ON in HIGH or LOW state (active high or low)

  public:
    // Constructor: Accepts an array of pin numbers and the relay on state (HIGH or LOW)
    RelayController(uint8_t pins[8], bool onState);

    bool onState=false;
    // Method to turn on/off a specific relay and update the relayStatus array
    void execute(uint8_t channel, bool state, uint8_t sourceCommand);
    
    //Inverting state of the array. Turn on if the state is off, turn off if state is on
    void execute(uint8_t channel,uint8_t sourceCommand);
    
    //Getter for last commmand source
    uint8_t getLastCommand();    

    // Getter for a specific channel's relay status
    bool getStatus(int channel);
    
    // Getter to return all relay statuses as a single byte
    uint8_t getAllStatusAsByte();
    
    // Setter: Takes a byte and sets the relays accordingly
    void setStatusFromBit(byte value, uint8_t description);
    
    // Setter: Accepts a C-string like "01001101" and sets the relays accordingly
    void setStatusFromBit(const char* statusString, uint8_t description);
    
    // Getter: Converts all relay statuses to a binary string in char* format
    void getAllStatusAsCString(char* statusString);
};



#endif