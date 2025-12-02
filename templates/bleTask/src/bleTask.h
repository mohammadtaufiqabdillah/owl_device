#ifndef BLETASK_H
#define BLETASK_H

#include <ArduinoJson.h>
#include <commonvar.h>
#include <commondef.h>
#include <NimBLEDevice.h>
extern BLECharacteristic * pResponse;
extern BLECharacteristic * characteristic_notify_gps;
extern BLECharacteristic * characteristic_notify_fingerprint;
extern BLECharacteristic * characteristic_notify_vehicle;
extern BLECharacteristic * characteristic_notify_radio;

bool ble_task();

struct status_ble_struct
{
    bool advertising=false;
    bool connected=false;
};

extern QueueHandle_t statusBleQueue;

bool get_ble_status(status_ble_struct &result);

#endif