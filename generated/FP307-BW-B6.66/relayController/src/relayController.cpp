#include "relayController.h"

    RelayController::RelayController(uint8_t pins[8], bool onState) {
      relayOnState = onState;
      
      // Initialize relay pins and set them to OUTPUT
      for (uint8_t i = 0; i < 8; i++) {
        relayPins[i] = pins[i];
        pinMode(relayPins[i], OUTPUT);
        relayStatus[i] = false;  // Initially, all relays are OFF
        digitalWrite(relayPins[i], !relayOnState);  // Ensure all relays are OFF
      }
    }

    // Method to turn on/off a specific relay and update the relayStatus array
    void RelayController:: execute(uint8_t channel, bool state, uint8_t description) {
      if (channel >= 0 && channel < 8) {
        relayStatus[channel] = state;  // Update status array
        digitalWrite(relayPins[channel], state ? relayOnState : !relayOnState);  // Turn relay ON/OFF
      }
      lastCommand=description;
    }

    //Invert if before is on the off, etc
    void RelayController:: execute(uint8_t channel,uint8_t description) {
      bool state=!relayStatus[channel];
      if (channel >= 0 && channel < 8) {
        relayStatus[channel] = state;  // Update status array
        digitalWrite(relayPins[channel], state ? relayOnState : !relayOnState);  // Turn relay ON/OFF
      }
      lastCommand=description;
    }

    uint8_t RelayController:: getLastCommand(){
      return lastCommand;
    }
    // Getter for a specific channel's relay status
    bool RelayController:: getStatus(int channel) {
      if (channel >= 0 && channel < 8) {
        return relayStatus[channel];
      }
      return false;  // Default return if out of bounds
    }

    // Getter to return all relay statuses as a single byte
    uint8_t RelayController:: getAllStatusAsByte() {
      byte result = 0;
      for (int i = 0; i < 8; i++) {
        if (relayStatus[i]) {
          result |= (1 << i);  // Set the corresponding bit for ON relays
        }
      }
      return result;
    }

    // Setter: Takes a byte and sets the relays accordingly
    void RelayController:: setStatusFromBit(byte value, uint8_t description) {
      for (int i = 0; i < 8; i++) {
        bool newState = (value >> i) & 0x01;  // Extract each bit from the byte
        execute(i, newState,description);  // Turn the relay ON/OFF based on the byte value
      }
    }
    // Setter: Accepts a C-string like "01001101" and sets the relays accordingly
    void RelayController:: setStatusFromBit(const char* statusString, uint8_t description) {
      for (int i = 0; i < 8; i++) {
        if (statusString[i] == '1') {
          execute(i, true,description);  // Turn relay ON
        } else if (statusString[i] == '0') {
          execute(i, false,description);  // Turn relay OFF
        }
        // If the string contains characters other than '1' or '0', they are ignored
      }
    }
    // Getter: Converts all relay statuses to a binary string in char* format
    void RelayController:: getAllStatusAsCString(char* statusString) {
      for (int i = 0; i < 8; i++) {
        statusString[i] = relayStatus[i] ? '1' : '0';  // Set '1' for ON, '0' for OFF
      }
      statusString[8] = '\0';  // Null-terminate the string
    }
