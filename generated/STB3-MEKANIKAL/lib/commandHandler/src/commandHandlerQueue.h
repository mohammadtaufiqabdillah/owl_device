#ifndef COMMANDHANDLERQUEUE_H
#define COMMANDHANDLERQUEUE_H
#include <Arduino.h>

extern SemaphoreHandle_t mutex_handler;

// Queue Declarations



// Struct Definitions
#pragma pack(1)

struct headerRadioStruct {
    uint32_t to;
    uint32_t from;
    uint16_t id_message;
    uint8_t type;
    uint16_t key;
    uint16_t commandId;
    uint8_t data [512];
    uint16_t len; 
};
  
struct headerCommandStruct {
    uint16_t id_message;
    uint8_t type;
    uint16_t key;
    uint16_t commandId;
    uint8_t data [512];
    uint16_t len; 
};



#pragma pack()
#endif