#ifndef COMMONVAR_H_
#define COMMONVAR_H_
#include <Arduino.h>
#include <RTClib.h>
#include <commandHandler.h>
#include <esp_log.h>
#include <NimBLEDevice.h>
#include <change_log.h>
#include <WiFi.h>
#include <httpClient.h>
#include <LittleFS.h>

#define PRODUCT_TYPE "STB3-MEKANIKAL"
#define PRODUCT_NAME "Environment & Factory Monitoring"
#define COMPANY_ID 100
#define DEVICE_ID 

struct gps_status_struct {
    bool status=false;  
    float latitude;
    float longitude;
    float altitude;
    float speed;
    float top_speed;
    uint32_t unix;
    float km_season;
};

struct power_status_struct {
    bool isCharge=false;
    uint8_t bat;
    float vCharge;
    float vBat;
};

struct radio_acknowledge_struct {
    uint32_t message_id;
    uint8_t message_type;
    uint32_t status_send;
    float rssi;
    float snr;
    uint16_t timeonair;
};

extern CommandHandler handler;

extern QueueHandle_t gpsStatusQueue;

extern QueueHandle_t powerStatusQueue;

extern QueueHandle_t unixTimeStatusQueue;

extern QueueHandle_t bleEventQueue;

extern QueueHandle_t bleAdvertismentQueue;

extern QueueHandle_t bleDataQueue;

extern xTaskHandle mqttTaskHandler;

extern xTaskHandle bleTaskHandler;

extern xTaskHandle scheduleTaskHandler;

extern xTaskHandle gpsTaskHandler;

extern xTaskHandle radioTaskHandler;

extern xTaskHandle watchDogTaskHandler;

extern void print_high_water (const char *task, uint32_t *millis);

extern bool update_advertisment_data (headerCommandStruct &data);

extern bool send_ble_data (BLECharacteristic * characteristic, headerCommandStruct &data);

extern bool send_ble_data (BLECharacteristic * characteristic, JsonDocument &doc);

extern bool get_bt_mac_string(char *mac_str, size_t buffer_size);

extern bool send_radio_external(uint32_t message_id,const uint8_t *array,int len,uint32_t timeout=1000);

extern bool send_radio_external(headerRadioStruct &radioData,uint32_t timeout=1000);

extern bool send_esp_now_external(headerRadioStruct &radioData,uint32_t timeout=1000);

extern void download_ota();

struct ble_adv_struct {
    char data [512];
    uint8_t len;
};

struct ble_send_struct {
    BLECharacteristic * characteristic;
    char data [512];
    uint8_t len;
};

extern gps_status_struct getGpsStatus();

struct mqtt_send_struct {
    char topic[150];
    char payload[1400];
    uint8_t qos=0;
};

extern power_status_struct getPowerStatus();

extern bool send_mqtt_external(JsonDocument &doc,const char * description, int qos, int timeout=2000);

extern float mapFloat(float Input, float batasBawah, float batasAtas, float valueBawah, float valueAtas);

extern DateTime getDateTime();

extern float get_rtc_temp();

extern uint32_t mac_to_chip_id(const char* mac);

extern adv_ble_struct advertising_state;

extern void default_command();

#pragma pack(1)
typedef struct {
    char dataName[150];
    uint32_t dataSize=0;
    uint32_t uploaded_row=0;
} FileHeader;
#pragma pack()

bool saveStructData(const char* dataName, void* structData, uint32_t structSize);
// 1. Fungsi untuk mengecek konsistensi data dan mendapatkan jumlah data
bool checkDataConsistency(const char* dataName, FileHeader &header, uint32_t structSize, uint32_t* recordCount,bool is_backup=false);
// 2. Fungsi untuk membaca data dari posisi tertentu
uint32_t readData(const char* dataName, void* buffer, uint32_t bufferSize, uint32_t startRecord, bool is_backup=false);
// 3. Fungsi untuk mengupdate data pada posisi tertentu
bool updateStructData(const char* dataName, void* structData, uint32_t structSize, uint32_t recordPosition, bool is_backup=false);
bool updateFlagData(const char* dataName, uint32_t updated_row, bool is_backup);
#endif