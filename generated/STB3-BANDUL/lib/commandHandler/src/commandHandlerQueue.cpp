#include "CommandHandler.h"

SemaphoreHandle_t mutex_handler;

// Queue Initializations
extern QueueHandle_t wifiQueue;
extern QueueHandle_t intervalQueue;


// Initialize the queue
void CommandHandler::queueBegin(){
    mutex_handler = xSemaphoreCreateRecursiveMutex();
        wifiQueue = xQueueCreate(1, 1);
    intervalQueue = xQueueCreate(1, 1);

}

// Send settings to the specified RTOS queue
void CommandHandler::sendToQueue(const char* queueName, bool status) {
    JsonDocument doc;
    bool newValues = status;

    if (strcmp(queueName, "wifiqueue") == 0) {
    if (wifiQueue != NULL) {
        xQueueOverwrite(wifiQueue, &newValues);
    }
}
else if (strcmp(queueName, "intervalqueue") == 0) {
    if (intervalQueue != NULL) {
        xQueueOverwrite(intervalQueue, &newValues);
    }
}
}

// Load struct overload function
bool CommandHandler::loadStruct(pom_data_struct &structData, bool internal, bool loadJson, JsonDocument *docResult) {
    JsonDocument doc;
    if (!loadJson) {
        bool hasValue = false;
        if (internal && loadSettingsInternal("pom_data", doc)) {
            structData.transaksi_id = doc["data"]["transaksi_id"];
            structData.liter = doc["data"]["liter"];
            strcpy(structData.plat, doc["data"]["plat"]);
            structData.karyawan_id = doc["data"]["karyawan_id"];
            structData.stock = doc["data"]["stock"];
            strcpy(structData.date, doc["data"]["date"]);
            return true;
        }
        if (!internal && loadSettings("pom_data", doc)) {
            structData.transaksi_id = doc["data"]["transaksi_id"];
            structData.liter = doc["data"]["liter"];
            strcpy(structData.plat, doc["data"]["plat"]);
            structData.karyawan_id = doc["data"]["karyawan_id"];
            structData.stock = doc["data"]["stock"];
            strcpy(structData.date, doc["data"]["date"]);
            return true;
        }
        if (!hasValue) {
            structData.transaksi_id = setupDoc["pom_data"]["default"][0];
            structData.liter = setupDoc["pom_data"]["default"][1];
            strcpy(structData.plat, setupDoc["pom_data"]["default"][2]);
            structData.karyawan_id = setupDoc["pom_data"]["default"][3];
            structData.stock = setupDoc["pom_data"]["default"][4];
            strcpy(structData.date, setupDoc["pom_data"]["default"][5]);
            return true;
        }
        return false;
    }

    if (loadJson && docResult != nullptr) {
        (*docResult)["command"] = "pom_data";
        (*docResult)["data"]["transaksi_id"] = structData.transaksi_id;
        (*docResult)["data"]["liter"] = structData.liter;
        (*docResult)["data"]["plat"] = structData.plat;
        (*docResult)["data"]["karyawan_id"] = structData.karyawan_id;
        (*docResult)["data"]["stock"] = structData.stock;
        (*docResult)["data"]["date"] = structData.date;
        return false;
    }

    return false;
}

bool CommandHandler::loadStruct(wifi_struct &structData, bool internal, bool loadJson, JsonDocument *docResult) {
    JsonDocument doc;
    if (!loadJson) {
        bool hasValue = false;
        if (internal && loadSettingsInternal("wifi", doc)) {
            strcpy(structData.ssid, doc["data"]["ssid"]);
            strcpy(structData.password, doc["data"]["password"]);
            return true;
        }
        if (!internal && loadSettings("wifi", doc)) {
            strcpy(structData.ssid, doc["data"]["ssid"]);
            strcpy(structData.password, doc["data"]["password"]);
            return true;
        }
        if (!hasValue) {
            strcpy(structData.ssid, setupDoc["wifi"]["default"][0]);
            strcpy(structData.password, setupDoc["wifi"]["default"][1]);
            return true;
        }
        return false;
    }

    if (loadJson && docResult != nullptr) {
        (*docResult)["command"] = "wifi";
        (*docResult)["data"]["ssid"] = structData.ssid;
        (*docResult)["data"]["password"] = structData.password;
        return false;
    }

    return false;
}

bool CommandHandler::loadStruct(interval_struct &structData, bool internal, bool loadJson, JsonDocument *docResult) {
    JsonDocument doc;
    if (!loadJson) {
        bool hasValue = false;
        if (internal && loadSettingsInternal("interval", doc)) {
            structData.get_data = doc["data"]["get_data"];
            structData.upload_data = doc["data"]["upload_data"];
            structData.standby = doc["data"]["standby"];
            return true;
        }
        if (!internal && loadSettings("interval", doc)) {
            structData.get_data = doc["data"]["get_data"];
            structData.upload_data = doc["data"]["upload_data"];
            structData.standby = doc["data"]["standby"];
            return true;
        }
        if (!hasValue) {
            structData.get_data = setupDoc["interval"]["default"][0];
            structData.upload_data = setupDoc["interval"]["default"][1];
            structData.standby = setupDoc["interval"]["default"][2];
            return true;
        }
        return false;
    }

    if (loadJson && docResult != nullptr) {
        (*docResult)["command"] = "interval";
        (*docResult)["data"]["get_data"] = structData.get_data;
        (*docResult)["data"]["upload_data"] = structData.upload_data;
        (*docResult)["data"]["standby"] = structData.standby;
        return false;
    }

    return false;
}

