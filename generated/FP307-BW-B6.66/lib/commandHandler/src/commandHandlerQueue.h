#ifndef COMMANDHANDLERQUEUE_H
#define COMMANDHANDLERQUEUE_H
#include <Arduino.h>

extern SemaphoreHandle_t mutex_handler;

// Queue Declarations
extern QueueHandle_t exe_proofQueue;
extern QueueHandle_t pom_dataQueue;
extern QueueHandle_t restartQueue;
extern QueueHandle_t wifiQueue;
extern QueueHandle_t mqttQueue;
extern QueueHandle_t testingQueue;



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

struct exe_proof_struct {
    // no fields defined for exe_proof
};

struct pom_data_struct {
    uint32_t pom_data_0;
    float pom_data_1;
    char pom_data_2[15];
    uint32_t pom_data_3;
    float pom_data_4;
    char pom_data_5[23];
};

struct restart_struct {
    bool restart_0;
};

struct wifi_struct {
    char wifi_0[25];
    char wifi_1[25];
};

struct mqtt_struct {
    char mqtt_0[125];
    uint16_t mqtt_1;
    char mqtt_2[25];
    char mqtt_3[25];
};

struct testing_struct {
    // no fields defined for testing
};



#pragma pack()
#endif