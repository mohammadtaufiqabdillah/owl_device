#ifndef COMMANDHANDLERQUEUE_H
#define COMMANDHANDLERQUEUE_H
#include <Arduino.h>

extern SemaphoreHandle_t mutex_handler;

// Queue Declarations
extern QueueHandle_t wifiQueue;
extern QueueHandle_t intervalQueue;



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

struct pom_data_struct {
    uint32_t transaksi_id;
    float liter;
    char plat[15];
    uint32_t karyawan_id;
    float stock;
    char date[23];
};

struct wifi_struct {
    char ssid[25];
    char password[25];
};

struct interval_struct {
    uint32_t get_data;
    uint32_t upload_data;
    uint32_t standby;
};



#pragma pack()
#endif