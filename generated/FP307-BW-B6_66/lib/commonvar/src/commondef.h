#ifndef COMMONDEF_H_
#define COMMONDEF_H_
#include <Arduino.h>
#include <ArduinoJson.h>

#if CONFIG_IDF_TARGET_ESP32
    #define CI_USE_PSRAM false
    //BC
    #define USE_BATCONTROL true
    #define HIGH_LEVEL_TRIGGER_BC true //true = HIGH, false = LOW
    #define CONTROL_BC_PIN 13
    //3.3 CONTROL
    #define USE_33CONTROL true
    #define HIGH_LEVEL_TRIGGER_3C true //true = HIGH, false = LOW
    #define CONTROL_3_3V_PIN 12
    //BATSENSE
    #define USE_BATSENSE true
    #define BATSENSE 39 //BATTERY SENSE VOLTAGE  (HASIL PEMBACAAN DIKALI 2)
    #define BATSENSE_CHANNEL ADC1_CHANNEL_3
    #define BATSENSE_CAL 0
    #define BATSENSE_MULTIPLIER 2
    //BATCONTROL SENSE
    #define USE_BATCONTROL_SENSE false
    #define BATCONTROL_SENSE 38 //BACA BATCONTROLL (HASIL PEMBACAAN DIKALI 2)
    #define BATCONTROL_CAL 0
    #define BATCONTROL_CHANNEL ADC1_CHANNEL_6
    #define BATCONTROL_SENSE_MULTIPLIER 2
    //SOLAR OR CHARGE SENSE
    #define USE_SOLAR_SENSE false
    #define SOLAR_VOLTAGE_SENSE 36 //BACA SOLAR CELL VOLTAGE (HASIL PEMBACAAN DI KALI 11)
    #define SOLAR_VOLTAGE_CHANNEL ADC1_CHANNEL_0
    #define SOLAR_VOLTAGE_CAL 0
    #define SOLAR_VOLTAGE_MULTIPLIER 11
    //CHARGE ENABLE
    #define USE_CHARGE_ENABLE false
    #define CHARGE_ENABLE_PIN 3 //ENABLE PIN UNTUK CHARGE
    #define DEFAULT_CUTOFF_CHARGE      4100 //mv (mili volt)
    #define DEFAULT_RESUME_CHARGE      3800 //mv (mili volt)
    #define HIGH_LEVEL_TRIGGER_CE false //true = HIGH, false = LOW    
#endif

#if CONFIG_IDF_TARGET_ESP32S3
    #define CI_USE_PSRAM true
    //BC
    #define USE_BATCONTROL true
    #define HIGH_LEVEL_TRIGGER_BC true //true = HIGH, false = LOW
    #define CONTROL_BC_PIN 46
    //3.3 CONTROL
    #define USE_33CONTROL true
    #define HIGH_LEVEL_TRIGGER_3C true //true = HIGH, false = LOW
    #define CONTROL_3_3V_PIN 45
    //BATSENSE
    #define USE_BATSENSE true
    #define BATSENSE 5 //BATTERY SENSE VOLTAGE  (HASIL PEMBACAAN DIKALI 2)
    #define BATSENSE_CHANNEL ADC1_CHANNEL_4
    #define BATSENSE_CAL 0
    #define BATSENSE_MULTIPLIER 2
    //BATCONTROL SENSE
    #define USE_BATCONTROL_SENSE false
    #define BATCONTROL_SENSE 38 //BACA BATCONTROLL (HASIL PEMBACAAN DIKALI 2)
    #define BATCONTROL_CAL 0
    #define BATCONTROL_CHANNEL ADC1_CHANNEL_6
    #define BATCONTROL_SENSE_MULTIPLIER 2
    //SOLAR OR CHARGE SENSE
    #define USE_SOLAR_SENSE false
    #define SOLAR_VOLTAGE_SENSE 36 //BACA SOLAR CELL VOLTAGE (HASIL PEMBACAAN DI KALI 11)
    #define SOLAR_VOLTAGE_CHANNEL ADC1_CHANNEL_0
    #define SOLAR_VOLTAGE_CAL 0
    #define SOLAR_VOLTAGE_MULTIPLIER 11
    //CHARGE ENABLE
    #define USE_CHARGE_ENABLE true
    #define CHARGE_ENABLE_PIN 3 //ENABLE PIN UNTUK CHARGE
    #define DEFAULT_CUTOFF_CHARGE      4100 //mv (mili volt)
    #define DEFAULT_RESUME_CHARGE      3800 //mv (mili volt)
    #define HIGH_LEVEL_TRIGGER_CE false //true = HIGH, false = LOW
#endif

// ...existing code...
#define FS_SD        1
#define FS_LITTLEFS  2
#define FS_FATFS     3
#define FS_FAT32     4

#define USED_FILE_SYSTEM FS_LITTLEFS

#if USED_FILE_SYSTEM == FS_SD
    #include <SD.h>
    #define FILE_SYSTEM SD
    #define GET_FS_TYPE "SD"
    #define GET_TOTAL_SPACE SD.totalBytes()/1024
    #define GET_USED_SPACE SD.usedBytes()/1024

#elif USED_FILE_SYSTEM == FS_LITTLEFS
    #define FILE_SYSTEM LittleFS
    #define GET_FS_TYPE "LITTLEFS"
    #define GET_TOTAL_SPACE LittleFS.totalBytes()/1024
    #define GET_USED_SPACE LittleFS.usedBytes()/1024

#elif USED_FILE_SYSTEM == FS_FATFS || USED_FILE_SYSTEM == FS_FAT32
    #define FILE_SYSTEM FFat
    #define GET_FS_TYPE "FATFS"
    #define GET_TOTAL_SPACE FFat.totalBytes()/1024
    #define GET_USED_SPACE FFat.usedBytes()/1024    
#endif

#define HOSTNAME "CoV"

#define DATA_DIRECTORY "/data"

//USED FUNCTION ALL
extern uint16_t getID16();

extern uint32_t getID32();

//ROLE RADIO
#define ROLE_GATEWAY true
#define ROLE_NODE false
#define ROLE_REPEATER false

//ROLE ESP_NOW
#define ESP_NOW_ROLE_GATEWAY true
#define ESP_NOW_ROLE_NODE false
#define ESP_NOW_ROLE_REPEATER false

//DEFINITION
#define COMMAND_HANDLER_TYPE_SET 1
#define COMMAND_HANDLER_TYPE_GET 2
#define COMMAND_HANDLER_TYPE_DEL 3
#define COMMAND_HANDLER_TYPE_EXE 4
#define COMMAND_HANDLER_TYPE_DAT 5
#define COMMAND_HANDLER_TYPE_DAT_WITHOUT_ACK 6
#define COMMAND_HANDLER_TYPE_RES 7


//BAT CONTROL
#if USE_BATCONTROL
    #define INIT_BATCONTROL pinMode(CONTROL_BC_PIN, OUTPUT); //pastikan pin batcontrol di set sebagai output
    #if HIGH_LEVEL_TRIGGER_BC
        #define TURN_ON_BATCONTROL digitalWrite(CONTROL_BC_PIN, HIGH)
        #define TURN_OFF_BATCONTROL digitalWrite(CONTROL_BC_PIN, LOW)
    #else
        #define TURN_ON_BATCONTROL digitalWrite(CONTROL_BC_PIN, LOW)
        #define TURN_OFF_BATCONTROL digitalWrite(CONTROL_BC_PIN, HIGH)
    #endif

#else
    #define INIT_BATCONTROL
    #define TURN_ON_BATCONTROL
    #define TURN_OFF_BATCONTROL 
#endif


//3.3V CONTROL
#if USE_33CONTROL
    #define INIT_3_3V_CONTROL pinMode(CONTROL_3_3V_PIN, OUTPUT); //pastikan pin 3.3v control di set sebagai output
    #if HIGH_LEVEL_TRIGGER_3C
        #define TURN_ON_3_3V digitalWrite(CONTROL_3_3V_PIN, HIGH)
        #define TURN_OFF_3_3V digitalWrite(CONTROL_3_3V_PIN, LOW)
    #else
        #define TURN_ON_3_3V digitalWrite(CONTROL_3_3V_PIN, LOW)
        #define TURN_OFF_3_3V digitalWrite(CONTROL_3_3V_PIN, HIGH)
    #endif
    
#else
  #define INIT_3_3V_CONTROL
  #define TURN_ON_3_3V
  #define TURN_OFF_3_3V 

#endif

//BAT SENSE
#if USE_BATSENSE
    #define INIT_BATSENSE pinMode(BATSENSE, INPUT); //pastikan pin batsense di set sebagai input
#else
    #define INIT_BATSENSE
#endif

//BAT CONTROL SENSE
#if USE_BATCONTROL_SENSE
    #define INIT_BATCONTROL_SENSE pinMode(BATCONTROL_SENSE, INPUT); //pastikan pin batsense di set sebagai input
#else
    #define INIT_BATCONTROL_SENSE
#endif

//SOLAR SENSE
#if USE_SOLAR_SENSE
    #define INIT_SOLAR_SENSE pinMode(SOLAR_VOLTAGE_SENSE, INPUT); //pastikan pin solar sense di set sebagai input
#else
    #define INIT_SOLAR_SENSE
#endif


//CHARGE ENABLE
#if USE_CHARGE_ENABLE
    #define INIT_CE pinMode(CHARGE_ENABLE_PIN, OUTPUT); //pastikan pin charge enable di set sebagai output
    #if HIGH_LEVEL_TRIGGER_CE
        #define TURN_ON_CE digitalWrite(CHARGE_ENABLE_PIN, HIGH)
        #define TURN_OFF_CE digitalWrite(CHARGE_ENABLE_PIN, LOW)
    #else
        #define TURN_ON_CE digitalWrite(CHARGE_ENABLE_PIN, LOW)
        #define TURN_OFF_CE digitalWrite(CHARGE_ENABLE_PIN, HIGH)
    #endif
#else
    #define INIT_CE
    #define TURN_ON_CE
    #define TURN_OFF_CE
#endif

#define PACKAGE_TRANSMIT_FAIL -1 //gagal transmit di library ada masalah hardware??
#define PACKAGE_TRANSMITTED 0 //di library sukses belum ada respon dari penerima
#define PACKAGE_TRANSMITTED_TO_NODE 1 //package sukses terkirim ke node dan sudah di acknowledge
#define PACKAGE_TRANSMITTED_TO_REPEATER 2 //ada balasan tapi dari repeater
#define PACKAGE_TRANSMITTED_TO_GATEWAY_FAILED_UPLOAD 3 //paket atau data sampai ke gateway tetapi belum berhasil di forward ke internet
#define PACKAGE_TRANSMITTED_TO_GATEWAY_SUCSESS_UPLOAD 4  //paket atau data sampai ke gateway tetapi sudah berhasil di forward ke internet
#define PACKAGE_TRANSMITTED_WAITING_EXECUTION 5 //package setup atau perintah sampai ke target, tetapi sedang eksekusi (contoh odsou sedang mengukur)
#define PACKAGE_TRANSMITTED_FAILED_EXECUTION 6 //package setup atau perintah sampai ke target, tetapi gagal di eksekusi oleh target
#define PACKAGE_TRANSMITTED_SUCSESS_EXECUTION 7 //package setup atau perintah sampai ke target, tetapi gagal di eksekusi oleh target

struct SpiRamAllocator : ArduinoJson::Allocator {
  void* allocate(size_t size) override {
    return heap_caps_malloc(size, MALLOC_CAP_SPIRAM);
  }

  void deallocate(void* pointer) override {
    heap_caps_free(pointer);
  }

  void* reallocate(void* ptr, size_t new_size) override {
    return heap_caps_realloc(ptr, new_size, MALLOC_CAP_SPIRAM);
  }
};

extern SpiRamAllocator allocator;

//Debug ON-OF
#if CI_USE_PSRAM
#define CREATE_JSON_DOCUMENT(X) JsonDocument X(&allocator)
#else
#define CREATE_JSON_DOCUMENT(X) JsonDocument X
#endif

#endif