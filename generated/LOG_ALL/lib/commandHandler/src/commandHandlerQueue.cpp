#include "CommandHandler.h"

SemaphoreHandle_t mutex_handler;

// Queue Initializations
extern QueueHandle_t mqttQueue;
extern QueueHandle_t use_keyQueue;
extern QueueHandle_t stb_bandul_setupQueue;
extern QueueHandle_t stb_mekanikal_setupQueue;
extern QueueHandle_t intervalQueue;
extern QueueHandle_t utcoffsetQueue;
extern QueueHandle_t last_epochQueue;
extern QueueHandle_t keyQueue;
extern QueueHandle_t snQueue;
extern QueueHandle_t otaurlQueue;
extern QueueHandle_t api_ver1Queue;
extern QueueHandle_t api_ver2Queue;
extern QueueHandle_t adv_bleQueue;
extern QueueHandle_t radio_modeQueue;
extern QueueHandle_t radio_param_1Queue;
extern QueueHandle_t radio_param_2Queue;
extern QueueHandle_t radio_param_3Queue;
extern QueueHandle_t radio_param_4Queue;
extern QueueHandle_t licenseQueue;
extern QueueHandle_t suspendQueue;
extern QueueHandle_t hierarchy_setupQueue;
extern QueueHandle_t fingerprint_setupQueue;
extern QueueHandle_t pom_setupQueue;
extern QueueHandle_t stb_controller_setupQueue;
extern QueueHandle_t tracker_setupQueue;
extern QueueHandle_t stb_controller_sub_setupQueue;
extern QueueHandle_t touch_calibrationQueue;
extern QueueHandle_t esp_nowQueue;
extern QueueHandle_t functional_flagQueue;


// Initialize the queue
void CommandHandler::queueBegin(){
    mutex_handler = xSemaphoreCreateRecursiveMutex();
        mqttQueue = xQueueCreate(1, 1);
    use_keyQueue = xQueueCreate(1, 1);
    stb_bandul_setupQueue = xQueueCreate(1, 1);
    stb_mekanikal_setupQueue = xQueueCreate(1, 1);
    intervalQueue = xQueueCreate(1, 1);
    utcoffsetQueue = xQueueCreate(1, 1);
    last_epochQueue = xQueueCreate(1, 1);
    keyQueue = xQueueCreate(1, 1);
    snQueue = xQueueCreate(1, 1);
    otaurlQueue = xQueueCreate(1, 1);
    api_ver1Queue = xQueueCreate(1, 1);
    api_ver2Queue = xQueueCreate(1, 1);
    adv_bleQueue = xQueueCreate(1, 1);
    radio_modeQueue = xQueueCreate(1, 1);
    radio_param_1Queue = xQueueCreate(1, 1);
    radio_param_2Queue = xQueueCreate(1, 1);
    radio_param_3Queue = xQueueCreate(1, 1);
    radio_param_4Queue = xQueueCreate(1, 1);
    licenseQueue = xQueueCreate(1, 1);
    suspendQueue = xQueueCreate(1, 1);
    hierarchy_setupQueue = xQueueCreate(1, 1);
    fingerprint_setupQueue = xQueueCreate(1, 1);
    pom_setupQueue = xQueueCreate(1, 1);
    stb_controller_setupQueue = xQueueCreate(1, 1);
    tracker_setupQueue = xQueueCreate(1, 1);
    stb_controller_sub_setupQueue = xQueueCreate(1, 1);
    touch_calibrationQueue = xQueueCreate(1, 1);
    esp_nowQueue = xQueueCreate(1, 1);
    functional_flagQueue = xQueueCreate(1, 1);

}

// Send settings to the specified RTOS queue
void CommandHandler::sendToQueue(const char* queueName, bool status) {
    JsonDocument doc;
    bool newValues = status;

    if (strcmp(queueName, "mqttqueue") == 0) {
    if (mqttQueue != NULL) {
        xQueueOverwrite(mqttQueue, &newValues);
    }
}
else if (strcmp(queueName, "use_keyqueue") == 0) {
    if (use_keyQueue != NULL) {
        xQueueOverwrite(use_keyQueue, &newValues);
    }
}
else if (strcmp(queueName, "stb_bandul_setupqueue") == 0) {
    if (stb_bandul_setupQueue != NULL) {
        xQueueOverwrite(stb_bandul_setupQueue, &newValues);
    }
}
else if (strcmp(queueName, "stb_mekanikal_setupqueue") == 0) {
    if (stb_mekanikal_setupQueue != NULL) {
        xQueueOverwrite(stb_mekanikal_setupQueue, &newValues);
    }
}
else if (strcmp(queueName, "intervalqueue") == 0) {
    if (intervalQueue != NULL) {
        xQueueOverwrite(intervalQueue, &newValues);
    }
}
else if (strcmp(queueName, "utcoffsetqueue") == 0) {
    if (utcoffsetQueue != NULL) {
        xQueueOverwrite(utcoffsetQueue, &newValues);
    }
}
else if (strcmp(queueName, "last_epochqueue") == 0) {
    if (last_epochQueue != NULL) {
        xQueueOverwrite(last_epochQueue, &newValues);
    }
}
else if (strcmp(queueName, "keyqueue") == 0) {
    if (keyQueue != NULL) {
        xQueueOverwrite(keyQueue, &newValues);
    }
}
else if (strcmp(queueName, "snqueue") == 0) {
    if (snQueue != NULL) {
        xQueueOverwrite(snQueue, &newValues);
    }
}
else if (strcmp(queueName, "otaurlqueue") == 0) {
    if (otaurlQueue != NULL) {
        xQueueOverwrite(otaurlQueue, &newValues);
    }
}
else if (strcmp(queueName, "api_ver1queue") == 0) {
    if (api_ver1Queue != NULL) {
        xQueueOverwrite(api_ver1Queue, &newValues);
    }
}
else if (strcmp(queueName, "api_ver2queue") == 0) {
    if (api_ver2Queue != NULL) {
        xQueueOverwrite(api_ver2Queue, &newValues);
    }
}
else if (strcmp(queueName, "adv_blequeue") == 0) {
    if (adv_bleQueue != NULL) {
        xQueueOverwrite(adv_bleQueue, &newValues);
    }
}
else if (strcmp(queueName, "radio_modequeue") == 0) {
    if (radio_modeQueue != NULL) {
        xQueueOverwrite(radio_modeQueue, &newValues);
    }
}
else if (strcmp(queueName, "radio_param_1queue") == 0) {
    if (radio_param_1Queue != NULL) {
        xQueueOverwrite(radio_param_1Queue, &newValues);
    }
}
else if (strcmp(queueName, "radio_param_2queue") == 0) {
    if (radio_param_2Queue != NULL) {
        xQueueOverwrite(radio_param_2Queue, &newValues);
    }
}
else if (strcmp(queueName, "radio_param_3queue") == 0) {
    if (radio_param_3Queue != NULL) {
        xQueueOverwrite(radio_param_3Queue, &newValues);
    }
}
else if (strcmp(queueName, "radio_param_4queue") == 0) {
    if (radio_param_4Queue != NULL) {
        xQueueOverwrite(radio_param_4Queue, &newValues);
    }
}
else if (strcmp(queueName, "licensequeue") == 0) {
    if (licenseQueue != NULL) {
        xQueueOverwrite(licenseQueue, &newValues);
    }
}
else if (strcmp(queueName, "suspendqueue") == 0) {
    if (suspendQueue != NULL) {
        xQueueOverwrite(suspendQueue, &newValues);
    }
}
else if (strcmp(queueName, "hierarchy_setupqueue") == 0) {
    if (hierarchy_setupQueue != NULL) {
        xQueueOverwrite(hierarchy_setupQueue, &newValues);
    }
}
else if (strcmp(queueName, "fingerprint_setupqueue") == 0) {
    if (fingerprint_setupQueue != NULL) {
        xQueueOverwrite(fingerprint_setupQueue, &newValues);
    }
}
else if (strcmp(queueName, "pom_setupqueue") == 0) {
    if (pom_setupQueue != NULL) {
        xQueueOverwrite(pom_setupQueue, &newValues);
    }
}
else if (strcmp(queueName, "stb_controller_setupqueue") == 0) {
    if (stb_controller_setupQueue != NULL) {
        xQueueOverwrite(stb_controller_setupQueue, &newValues);
    }
}
else if (strcmp(queueName, "tracker_setupqueue") == 0) {
    if (tracker_setupQueue != NULL) {
        xQueueOverwrite(tracker_setupQueue, &newValues);
    }
}
else if (strcmp(queueName, "stb_controller_sub_setupqueue") == 0) {
    if (stb_controller_sub_setupQueue != NULL) {
        xQueueOverwrite(stb_controller_sub_setupQueue, &newValues);
    }
}
else if (strcmp(queueName, "touch_calibrationqueue") == 0) {
    if (touch_calibrationQueue != NULL) {
        xQueueOverwrite(touch_calibrationQueue, &newValues);
    }
}
else if (strcmp(queueName, "esp_nowqueue") == 0) {
    if (esp_nowQueue != NULL) {
        xQueueOverwrite(esp_nowQueue, &newValues);
    }
}
else if (strcmp(queueName, "functional_flagqueue") == 0) {
    if (functional_flagQueue != NULL) {
        xQueueOverwrite(functional_flagQueue, &newValues);
    }
}
}

// Load struct overload function
bool CommandHandler::loadStruct(mqtt_struct &structData, bool internal, bool loadJson, JsonDocument *docResult) {
    JsonDocument doc;
    if (!loadJson) {
        bool hasValue = false;
        if (internal && loadSettingsInternal("mqtt", doc)) {
            strcpy(structData.host, doc["data"]["host"]);
            structData.port = doc["data"]["port"];
            strcpy(structData.username, doc["data"]["username"]);
            strcpy(structData.password, doc["data"]["password"]);
            return true;
        }
        if (!internal && loadSettings("mqtt", doc)) {
            strcpy(structData.host, doc["data"]["host"]);
            structData.port = doc["data"]["port"];
            strcpy(structData.username, doc["data"]["username"]);
            strcpy(structData.password, doc["data"]["password"]);
            return true;
        }
        if (!hasValue) {
            strcpy(structData.host, setupDoc["mqtt"]["default"][0]);
            structData.port = setupDoc["mqtt"]["default"][1];
            strcpy(structData.username, setupDoc["mqtt"]["default"][2]);
            strcpy(structData.password, setupDoc["mqtt"]["default"][3]);
            return true;
        }
        return false;
    }

    if (loadJson && docResult != nullptr) {
        (*docResult)["command"] = "mqtt";
        (*docResult)["data"]["host"] = structData.host;
        (*docResult)["data"]["port"] = structData.port;
        (*docResult)["data"]["username"] = structData.username;
        (*docResult)["data"]["password"] = structData.password;
        return false;
    }

    return false;
}

bool CommandHandler::loadStruct(use_key_struct &structData, bool internal, bool loadJson, JsonDocument *docResult) {
    JsonDocument doc;
    if (!loadJson) {
        bool hasValue = false;
        if (internal && loadSettingsInternal("use_key", doc)) {
            return true;
        }
        if (!internal && loadSettings("use_key", doc)) {
            return true;
        }
        if (!hasValue) {
            return true;
        }
        return false;
    }

    if (loadJson && docResult != nullptr) {
        (*docResult)["command"] = "use_key";
        return false;
    }

    return false;
}

bool CommandHandler::loadStruct(stb_bandul_setup_struct &structData, bool internal, bool loadJson, JsonDocument *docResult) {
    JsonDocument doc;
    if (!loadJson) {
        bool hasValue = false;
        if (internal && loadSettingsInternal("stb_bandul_setup", doc)) {
            return true;
        }
        if (!internal && loadSettings("stb_bandul_setup", doc)) {
            return true;
        }
        if (!hasValue) {
            return true;
        }
        return false;
    }

    if (loadJson && docResult != nullptr) {
        (*docResult)["command"] = "stb_bandul_setup";
        return false;
    }

    return false;
}

bool CommandHandler::loadStruct(stb_mekanikal_setup_struct &structData, bool internal, bool loadJson, JsonDocument *docResult) {
    JsonDocument doc;
    if (!loadJson) {
        bool hasValue = false;
        if (internal && loadSettingsInternal("stb_mekanikal_setup", doc)) {
            return true;
        }
        if (!internal && loadSettings("stb_mekanikal_setup", doc)) {
            return true;
        }
        if (!hasValue) {
            return true;
        }
        return false;
    }

    if (loadJson && docResult != nullptr) {
        (*docResult)["command"] = "stb_mekanikal_setup";
        return false;
    }

    return false;
}

bool CommandHandler::loadStruct(restart_struct &structData, bool internal, bool loadJson, JsonDocument *docResult) {
    JsonDocument doc;
    if (!loadJson) {
        bool hasValue = false;
        if (internal && loadSettingsInternal("restart", doc)) {
            return true;
        }
        if (!internal && loadSettings("restart", doc)) {
            return true;
        }
        if (!hasValue) {
            return true;
        }
        return false;
    }

    if (loadJson && docResult != nullptr) {
        (*docResult)["command"] = "restart";
        return false;
    }

    return false;
}

bool CommandHandler::loadStruct(interval_struct &structData, bool internal, bool loadJson, JsonDocument *docResult) {
    JsonDocument doc;
    if (!loadJson) {
        bool hasValue = false;
        if (internal && loadSettingsInternal("interval", doc)) {
            return true;
        }
        if (!internal && loadSettings("interval", doc)) {
            return true;
        }
        if (!hasValue) {
            return true;
        }
        return false;
    }

    if (loadJson && docResult != nullptr) {
        (*docResult)["command"] = "interval";
        return false;
    }

    return false;
}

bool CommandHandler::loadStruct(utcoffset_struct &structData, bool internal, bool loadJson, JsonDocument *docResult) {
    JsonDocument doc;
    if (!loadJson) {
        bool hasValue = false;
        if (internal && loadSettingsInternal("utcoffset", doc)) {
            return true;
        }
        if (!internal && loadSettings("utcoffset", doc)) {
            return true;
        }
        if (!hasValue) {
            return true;
        }
        return false;
    }

    if (loadJson && docResult != nullptr) {
        (*docResult)["command"] = "utcoffset";
        return false;
    }

    return false;
}

bool CommandHandler::loadStruct(last_epoch_struct &structData, bool internal, bool loadJson, JsonDocument *docResult) {
    JsonDocument doc;
    if (!loadJson) {
        bool hasValue = false;
        if (internal && loadSettingsInternal("last_epoch", doc)) {
            return true;
        }
        if (!internal && loadSettings("last_epoch", doc)) {
            return true;
        }
        if (!hasValue) {
            return true;
        }
        return false;
    }

    if (loadJson && docResult != nullptr) {
        (*docResult)["command"] = "last_epoch";
        return false;
    }

    return false;
}

bool CommandHandler::loadStruct(key_struct &structData, bool internal, bool loadJson, JsonDocument *docResult) {
    JsonDocument doc;
    if (!loadJson) {
        bool hasValue = false;
        if (internal && loadSettingsInternal("key", doc)) {
            return true;
        }
        if (!internal && loadSettings("key", doc)) {
            return true;
        }
        if (!hasValue) {
            return true;
        }
        return false;
    }

    if (loadJson && docResult != nullptr) {
        (*docResult)["command"] = "key";
        return false;
    }

    return false;
}

bool CommandHandler::loadStruct(sn_struct &structData, bool internal, bool loadJson, JsonDocument *docResult) {
    JsonDocument doc;
    if (!loadJson) {
        bool hasValue = false;
        if (internal && loadSettingsInternal("sn", doc)) {
            return true;
        }
        if (!internal && loadSettings("sn", doc)) {
            return true;
        }
        if (!hasValue) {
            return true;
        }
        return false;
    }

    if (loadJson && docResult != nullptr) {
        (*docResult)["command"] = "sn";
        return false;
    }

    return false;
}

bool CommandHandler::loadStruct(otaurl_struct &structData, bool internal, bool loadJson, JsonDocument *docResult) {
    JsonDocument doc;
    if (!loadJson) {
        bool hasValue = false;
        if (internal && loadSettingsInternal("otaurl", doc)) {
            strcpy(structData.otaurl, doc["data"]["otaurl"]);
            return true;
        }
        if (!internal && loadSettings("otaurl", doc)) {
            strcpy(structData.otaurl, doc["data"]["otaurl"]);
            return true;
        }
        if (!hasValue) {
            strcpy(structData.otaurl, setupDoc["otaurl"]["default"][0]);
            return true;
        }
        return false;
    }

    if (loadJson && docResult != nullptr) {
        (*docResult)["command"] = "otaurl";
        (*docResult)["data"]["otaurl"] = structData.otaurl;
        return false;
    }

    return false;
}

bool CommandHandler::loadStruct(api_ver1_struct &structData, bool internal, bool loadJson, JsonDocument *docResult) {
    JsonDocument doc;
    if (!loadJson) {
        bool hasValue = false;
        if (internal && loadSettingsInternal("api_ver1", doc)) {
            return true;
        }
        if (!internal && loadSettings("api_ver1", doc)) {
            return true;
        }
        if (!hasValue) {
            return true;
        }
        return false;
    }

    if (loadJson && docResult != nullptr) {
        (*docResult)["command"] = "api_ver1";
        return false;
    }

    return false;
}

bool CommandHandler::loadStruct(api_ver2_struct &structData, bool internal, bool loadJson, JsonDocument *docResult) {
    JsonDocument doc;
    if (!loadJson) {
        bool hasValue = false;
        if (internal && loadSettingsInternal("api_ver2", doc)) {
            return true;
        }
        if (!internal && loadSettings("api_ver2", doc)) {
            return true;
        }
        if (!hasValue) {
            return true;
        }
        return false;
    }

    if (loadJson && docResult != nullptr) {
        (*docResult)["command"] = "api_ver2";
        return false;
    }

    return false;
}

bool CommandHandler::loadStruct(adv_ble_struct &structData, bool internal, bool loadJson, JsonDocument *docResult) {
    JsonDocument doc;
    if (!loadJson) {
        bool hasValue = false;
        if (internal && loadSettingsInternal("adv_ble", doc)) {
            return true;
        }
        if (!internal && loadSettings("adv_ble", doc)) {
            return true;
        }
        if (!hasValue) {
            return true;
        }
        return false;
    }

    if (loadJson && docResult != nullptr) {
        (*docResult)["command"] = "adv_ble";
        return false;
    }

    return false;
}

bool CommandHandler::loadStruct(radio_mode_struct &structData, bool internal, bool loadJson, JsonDocument *docResult) {
    JsonDocument doc;
    if (!loadJson) {
        bool hasValue = false;
        if (internal && loadSettingsInternal("radio_mode", doc)) {
            return true;
        }
        if (!internal && loadSettings("radio_mode", doc)) {
            return true;
        }
        if (!hasValue) {
            return true;
        }
        return false;
    }

    if (loadJson && docResult != nullptr) {
        (*docResult)["command"] = "radio_mode";
        return false;
    }

    return false;
}

bool CommandHandler::loadStruct(radio_param_1_struct &structData, bool internal, bool loadJson, JsonDocument *docResult) {
    JsonDocument doc;
    if (!loadJson) {
        bool hasValue = false;
        if (internal && loadSettingsInternal("radio_param_1", doc)) {
            return true;
        }
        if (!internal && loadSettings("radio_param_1", doc)) {
            return true;
        }
        if (!hasValue) {
            return true;
        }
        return false;
    }

    if (loadJson && docResult != nullptr) {
        (*docResult)["command"] = "radio_param_1";
        return false;
    }

    return false;
}

bool CommandHandler::loadStruct(radio_param_2_struct &structData, bool internal, bool loadJson, JsonDocument *docResult) {
    JsonDocument doc;
    if (!loadJson) {
        bool hasValue = false;
        if (internal && loadSettingsInternal("radio_param_2", doc)) {
            return true;
        }
        if (!internal && loadSettings("radio_param_2", doc)) {
            return true;
        }
        if (!hasValue) {
            return true;
        }
        return false;
    }

    if (loadJson && docResult != nullptr) {
        (*docResult)["command"] = "radio_param_2";
        return false;
    }

    return false;
}

bool CommandHandler::loadStruct(radio_param_3_struct &structData, bool internal, bool loadJson, JsonDocument *docResult) {
    JsonDocument doc;
    if (!loadJson) {
        bool hasValue = false;
        if (internal && loadSettingsInternal("radio_param_3", doc)) {
            return true;
        }
        if (!internal && loadSettings("radio_param_3", doc)) {
            return true;
        }
        if (!hasValue) {
            return true;
        }
        return false;
    }

    if (loadJson && docResult != nullptr) {
        (*docResult)["command"] = "radio_param_3";
        return false;
    }

    return false;
}

bool CommandHandler::loadStruct(radio_param_4_struct &structData, bool internal, bool loadJson, JsonDocument *docResult) {
    JsonDocument doc;
    if (!loadJson) {
        bool hasValue = false;
        if (internal && loadSettingsInternal("radio_param_4", doc)) {
            return true;
        }
        if (!internal && loadSettings("radio_param_4", doc)) {
            return true;
        }
        if (!hasValue) {
            return true;
        }
        return false;
    }

    if (loadJson && docResult != nullptr) {
        (*docResult)["command"] = "radio_param_4";
        return false;
    }

    return false;
}

bool CommandHandler::loadStruct(license_struct &structData, bool internal, bool loadJson, JsonDocument *docResult) {
    JsonDocument doc;
    if (!loadJson) {
        bool hasValue = false;
        if (internal && loadSettingsInternal("license", doc)) {
            return true;
        }
        if (!internal && loadSettings("license", doc)) {
            return true;
        }
        if (!hasValue) {
            return true;
        }
        return false;
    }

    if (loadJson && docResult != nullptr) {
        (*docResult)["command"] = "license";
        return false;
    }

    return false;
}

bool CommandHandler::loadStruct(suspend_struct &structData, bool internal, bool loadJson, JsonDocument *docResult) {
    JsonDocument doc;
    if (!loadJson) {
        bool hasValue = false;
        if (internal && loadSettingsInternal("suspend", doc)) {
            return true;
        }
        if (!internal && loadSettings("suspend", doc)) {
            return true;
        }
        if (!hasValue) {
            return true;
        }
        return false;
    }

    if (loadJson && docResult != nullptr) {
        (*docResult)["command"] = "suspend";
        return false;
    }

    return false;
}

bool CommandHandler::loadStruct(hierarchy_setup_struct &structData, bool internal, bool loadJson, JsonDocument *docResult) {
    JsonDocument doc;
    if (!loadJson) {
        bool hasValue = false;
        if (internal && loadSettingsInternal("hierarchy_setup", doc)) {
            return true;
        }
        if (!internal && loadSettings("hierarchy_setup", doc)) {
            return true;
        }
        if (!hasValue) {
            return true;
        }
        return false;
    }

    if (loadJson && docResult != nullptr) {
        (*docResult)["command"] = "hierarchy_setup";
        return false;
    }

    return false;
}

bool CommandHandler::loadStruct(fingerprint_setup_struct &structData, bool internal, bool loadJson, JsonDocument *docResult) {
    JsonDocument doc;
    if (!loadJson) {
        bool hasValue = false;
        if (internal && loadSettingsInternal("fingerprint_setup", doc)) {
            return true;
        }
        if (!internal && loadSettings("fingerprint_setup", doc)) {
            return true;
        }
        if (!hasValue) {
            return true;
        }
        return false;
    }

    if (loadJson && docResult != nullptr) {
        (*docResult)["command"] = "fingerprint_setup";
        return false;
    }

    return false;
}

bool CommandHandler::loadStruct(pom_setup_struct &structData, bool internal, bool loadJson, JsonDocument *docResult) {
    JsonDocument doc;
    if (!loadJson) {
        bool hasValue = false;
        if (internal && loadSettingsInternal("pom_setup", doc)) {
            return true;
        }
        if (!internal && loadSettings("pom_setup", doc)) {
            return true;
        }
        if (!hasValue) {
            return true;
        }
        return false;
    }

    if (loadJson && docResult != nullptr) {
        (*docResult)["command"] = "pom_setup";
        return false;
    }

    return false;
}

bool CommandHandler::loadStruct(stb_controller_setup_struct &structData, bool internal, bool loadJson, JsonDocument *docResult) {
    JsonDocument doc;
    if (!loadJson) {
        bool hasValue = false;
        if (internal && loadSettingsInternal("stb_controller_setup", doc)) {
            return true;
        }
        if (!internal && loadSettings("stb_controller_setup", doc)) {
            return true;
        }
        if (!hasValue) {
            return true;
        }
        return false;
    }

    if (loadJson && docResult != nullptr) {
        (*docResult)["command"] = "stb_controller_setup";
        return false;
    }

    return false;
}

bool CommandHandler::loadStruct(tracker_setup_struct &structData, bool internal, bool loadJson, JsonDocument *docResult) {
    JsonDocument doc;
    if (!loadJson) {
        bool hasValue = false;
        if (internal && loadSettingsInternal("tracker_setup", doc)) {
            return true;
        }
        if (!internal && loadSettings("tracker_setup", doc)) {
            return true;
        }
        if (!hasValue) {
            return true;
        }
        return false;
    }

    if (loadJson && docResult != nullptr) {
        (*docResult)["command"] = "tracker_setup";
        return false;
    }

    return false;
}

bool CommandHandler::loadStruct(stb_controller_sub_setup_struct &structData, bool internal, bool loadJson, JsonDocument *docResult) {
    JsonDocument doc;
    if (!loadJson) {
        bool hasValue = false;
        if (internal && loadSettingsInternal("stb_controller_sub_setup", doc)) {
            return true;
        }
        if (!internal && loadSettings("stb_controller_sub_setup", doc)) {
            return true;
        }
        if (!hasValue) {
            return true;
        }
        return false;
    }

    if (loadJson && docResult != nullptr) {
        (*docResult)["command"] = "stb_controller_sub_setup";
        return false;
    }

    return false;
}

bool CommandHandler::loadStruct(touch_calibration_struct &structData, bool internal, bool loadJson, JsonDocument *docResult) {
    JsonDocument doc;
    if (!loadJson) {
        bool hasValue = false;
        if (internal && loadSettingsInternal("touch_calibration", doc)) {
            return true;
        }
        if (!internal && loadSettings("touch_calibration", doc)) {
            return true;
        }
        if (!hasValue) {
            return true;
        }
        return false;
    }

    if (loadJson && docResult != nullptr) {
        (*docResult)["command"] = "touch_calibration";
        return false;
    }

    return false;
}

bool CommandHandler::loadStruct(esp_now_struct &structData, bool internal, bool loadJson, JsonDocument *docResult) {
    JsonDocument doc;
    if (!loadJson) {
        bool hasValue = false;
        if (internal && loadSettingsInternal("esp_now", doc)) {
            return true;
        }
        if (!internal && loadSettings("esp_now", doc)) {
            return true;
        }
        if (!hasValue) {
            return true;
        }
        return false;
    }

    if (loadJson && docResult != nullptr) {
        (*docResult)["command"] = "esp_now";
        return false;
    }

    return false;
}

bool CommandHandler::loadStruct(functional_flag_struct &structData, bool internal, bool loadJson, JsonDocument *docResult) {
    JsonDocument doc;
    if (!loadJson) {
        bool hasValue = false;
        if (internal && loadSettingsInternal("functional_flag", doc)) {
            return true;
        }
        if (!internal && loadSettings("functional_flag", doc)) {
            return true;
        }
        if (!hasValue) {
            return true;
        }
        return false;
    }

    if (loadJson && docResult != nullptr) {
        (*docResult)["command"] = "functional_flag";
        return false;
    }

    return false;
}

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

bool CommandHandler::loadStruct(radio_pooling_data_struct &structData, bool internal, bool loadJson, JsonDocument *docResult) {
    JsonDocument doc;
    if (!loadJson) {
        bool hasValue = false;
        if (internal && loadSettingsInternal("radio_pooling_data", doc)) {
            return true;
        }
        if (!internal && loadSettings("radio_pooling_data", doc)) {
            return true;
        }
        if (!hasValue) {
            return true;
        }
        return false;
    }

    if (loadJson && docResult != nullptr) {
        (*docResult)["command"] = "radio_pooling_data";
        return false;
    }

    return false;
}

bool CommandHandler::loadStruct(bandul_data_struct &structData, bool internal, bool loadJson, JsonDocument *docResult) {
    JsonDocument doc;
    if (!loadJson) {
        bool hasValue = false;
        if (internal && loadSettingsInternal("bandul_data", doc)) {
            return true;
        }
        if (!internal && loadSettings("bandul_data", doc)) {
            return true;
        }
        if (!hasValue) {
            return true;
        }
        return false;
    }

    if (loadJson && docResult != nullptr) {
        (*docResult)["command"] = "bandul_data";
        return false;
    }

    return false;
}

bool CommandHandler::loadStruct(logger_gps_data_struct &structData, bool internal, bool loadJson, JsonDocument *docResult) {
    JsonDocument doc;
    if (!loadJson) {
        bool hasValue = false;
        if (internal && loadSettingsInternal("logger_gps_data", doc)) {
            return true;
        }
        if (!internal && loadSettings("logger_gps_data", doc)) {
            return true;
        }
        if (!hasValue) {
            return true;
        }
        return false;
    }

    if (loadJson && docResult != nullptr) {
        (*docResult)["command"] = "logger_gps_data";
        return false;
    }

    return false;
}

bool CommandHandler::loadStruct(logger_vehicle_gps_data_struct &structData, bool internal, bool loadJson, JsonDocument *docResult) {
    JsonDocument doc;
    if (!loadJson) {
        bool hasValue = false;
        if (internal && loadSettingsInternal("logger_vehicle_gps_data", doc)) {
            return true;
        }
        if (!internal && loadSettings("logger_vehicle_gps_data", doc)) {
            return true;
        }
        if (!hasValue) {
            return true;
        }
        return false;
    }

    if (loadJson && docResult != nullptr) {
        (*docResult)["command"] = "logger_vehicle_gps_data";
        return false;
    }

    return false;
}

bool CommandHandler::loadStruct(radio_broadcast_data_struct &structData, bool internal, bool loadJson, JsonDocument *docResult) {
    JsonDocument doc;
    if (!loadJson) {
        bool hasValue = false;
        if (internal && loadSettingsInternal("radio_broadcast_data", doc)) {
            return true;
        }
        if (!internal && loadSettings("radio_broadcast_data", doc)) {
            return true;
        }
        if (!hasValue) {
            return true;
        }
        return false;
    }

    if (loadJson && docResult != nullptr) {
        (*docResult)["command"] = "radio_broadcast_data";
        return false;
    }

    return false;
}

bool CommandHandler::loadStruct(radio_join_data_struct &structData, bool internal, bool loadJson, JsonDocument *docResult) {
    JsonDocument doc;
    if (!loadJson) {
        bool hasValue = false;
        if (internal && loadSettingsInternal("radio_join_data", doc)) {
            return true;
        }
        if (!internal && loadSettings("radio_join_data", doc)) {
            return true;
        }
        if (!hasValue) {
            return true;
        }
        return false;
    }

    if (loadJson && docResult != nullptr) {
        (*docResult)["command"] = "radio_join_data";
        return false;
    }

    return false;
}

bool CommandHandler::loadStruct(radio_response_data_struct &structData, bool internal, bool loadJson, JsonDocument *docResult) {
    JsonDocument doc;
    if (!loadJson) {
        bool hasValue = false;
        if (internal && loadSettingsInternal("radio_response_data", doc)) {
            return true;
        }
        if (!internal && loadSettings("radio_response_data", doc)) {
            return true;
        }
        if (!hasValue) {
            return true;
        }
        return false;
    }

    if (loadJson && docResult != nullptr) {
        (*docResult)["command"] = "radio_response_data";
        return false;
    }

    return false;
}

bool CommandHandler::loadStruct(stb_notification_struct &structData, bool internal, bool loadJson, JsonDocument *docResult) {
    JsonDocument doc;
    if (!loadJson) {
        bool hasValue = false;
        if (internal && loadSettingsInternal("stb_notification", doc)) {
            return true;
        }
        if (!internal && loadSettings("stb_notification", doc)) {
            return true;
        }
        if (!hasValue) {
            return true;
        }
        return false;
    }

    if (loadJson && docResult != nullptr) {
        (*docResult)["command"] = "stb_notification";
        return false;
    }

    return false;
}

bool CommandHandler::loadStruct(stb_sounding_data_struct &structData, bool internal, bool loadJson, JsonDocument *docResult) {
    JsonDocument doc;
    if (!loadJson) {
        bool hasValue = false;
        if (internal && loadSettingsInternal("stb_sounding_data", doc)) {
            structData.tinggi_minyak_dari_atas = doc["data"]["tinggi_minyak_dari_atas"];
            structData.tinggi_minyak_dari_bawah = doc["data"]["tinggi_minyak_dari_bawah"];
            structData.tinggi_udara_dari_atas = doc["data"]["tinggi_udara_dari_atas"];
            structData.tinggi_udara_dari_bawah = doc["data"]["tinggi_udara_dari_bawah"];
            structData.suhu = doc["data"]["suhu"];
            structData.baterai_bandul = doc["data"]["baterai_bandul"];
            structData.status = doc["data"]["status"];
            structData.error_code = doc["data"]["error_code"];
            structData.average_1 = doc["data"]["average_1"];
            structData.average_2 = doc["data"]["average_2"];
            structData.speed_naik = doc["data"]["speed_naik"];
            structData.speed_turun = doc["data"]["speed_turun"];
            structData.mejaukur = doc["data"]["mejaukur"];
            structData.minyak = doc["data"]["minyak"];
            structData.air = doc["data"]["air"];
            structData.lumpur = doc["data"]["lumpur"];
            structData.tinggi_meja = doc["data"]["tinggi_meja"];
            structData.deviasi = doc["data"]["deviasi"];
            structData.v_charge = doc["data"]["v_charge"];
            structData.suhu_rtc = doc["data"]["suhu_rtc"];
            structData.suhu_bandul = doc["data"]["suhu_bandul"];
            structData.suhu_probe = doc["data"]["suhu_probe"];
            structData.suhu_mek = doc["data"]["suhu_mek"];
            structData.recorded_at = doc["data"]["recorded_at"];
            return true;
        }
        if (!internal && loadSettings("stb_sounding_data", doc)) {
            structData.tinggi_minyak_dari_atas = doc["data"]["tinggi_minyak_dari_atas"];
            structData.tinggi_minyak_dari_bawah = doc["data"]["tinggi_minyak_dari_bawah"];
            structData.tinggi_udara_dari_atas = doc["data"]["tinggi_udara_dari_atas"];
            structData.tinggi_udara_dari_bawah = doc["data"]["tinggi_udara_dari_bawah"];
            structData.suhu = doc["data"]["suhu"];
            structData.baterai_bandul = doc["data"]["baterai_bandul"];
            structData.status = doc["data"]["status"];
            structData.error_code = doc["data"]["error_code"];
            structData.average_1 = doc["data"]["average_1"];
            structData.average_2 = doc["data"]["average_2"];
            structData.speed_naik = doc["data"]["speed_naik"];
            structData.speed_turun = doc["data"]["speed_turun"];
            structData.mejaukur = doc["data"]["mejaukur"];
            structData.minyak = doc["data"]["minyak"];
            structData.air = doc["data"]["air"];
            structData.lumpur = doc["data"]["lumpur"];
            structData.tinggi_meja = doc["data"]["tinggi_meja"];
            structData.deviasi = doc["data"]["deviasi"];
            structData.v_charge = doc["data"]["v_charge"];
            structData.suhu_rtc = doc["data"]["suhu_rtc"];
            structData.suhu_bandul = doc["data"]["suhu_bandul"];
            structData.suhu_probe = doc["data"]["suhu_probe"];
            structData.suhu_mek = doc["data"]["suhu_mek"];
            structData.recorded_at = doc["data"]["recorded_at"];
            return true;
        }
        if (!hasValue) {
            structData.tinggi_minyak_dari_atas = setupDoc["stb_sounding_data"]["default"][0];
            structData.tinggi_minyak_dari_bawah = setupDoc["stb_sounding_data"]["default"][1];
            structData.tinggi_udara_dari_atas = setupDoc["stb_sounding_data"]["default"][2];
            structData.tinggi_udara_dari_bawah = setupDoc["stb_sounding_data"]["default"][3];
            structData.suhu = setupDoc["stb_sounding_data"]["default"][4];
            structData.baterai_bandul = setupDoc["stb_sounding_data"]["default"][5];
            structData.status = setupDoc["stb_sounding_data"]["default"][6];
            structData.error_code = setupDoc["stb_sounding_data"]["default"][7];
            structData.average_1 = setupDoc["stb_sounding_data"]["default"][8];
            structData.average_2 = setupDoc["stb_sounding_data"]["default"][9];
            structData.speed_naik = setupDoc["stb_sounding_data"]["default"][10];
            structData.speed_turun = setupDoc["stb_sounding_data"]["default"][11];
            structData.mejaukur = setupDoc["stb_sounding_data"]["default"][12];
            structData.minyak = setupDoc["stb_sounding_data"]["default"][13];
            structData.air = setupDoc["stb_sounding_data"]["default"][14];
            structData.lumpur = setupDoc["stb_sounding_data"]["default"][15];
            structData.tinggi_meja = setupDoc["stb_sounding_data"]["default"][16];
            structData.deviasi = setupDoc["stb_sounding_data"]["default"][17];
            structData.v_charge = setupDoc["stb_sounding_data"]["default"][18];
            structData.suhu_rtc = setupDoc["stb_sounding_data"]["default"][19];
            structData.suhu_bandul = setupDoc["stb_sounding_data"]["default"][20];
            structData.suhu_probe = setupDoc["stb_sounding_data"]["default"][21];
            structData.suhu_mek = setupDoc["stb_sounding_data"]["default"][22];
            structData.recorded_at = setupDoc["stb_sounding_data"]["default"][23];
            return true;
        }
        return false;
    }

    if (loadJson && docResult != nullptr) {
        (*docResult)["command"] = "stb_sounding_data";
        (*docResult)["data"]["tinggi_minyak_dari_atas"] = structData.tinggi_minyak_dari_atas;
        (*docResult)["data"]["tinggi_minyak_dari_bawah"] = structData.tinggi_minyak_dari_bawah;
        (*docResult)["data"]["tinggi_udara_dari_atas"] = structData.tinggi_udara_dari_atas;
        (*docResult)["data"]["tinggi_udara_dari_bawah"] = structData.tinggi_udara_dari_bawah;
        (*docResult)["data"]["suhu"] = structData.suhu;
        (*docResult)["data"]["baterai_bandul"] = structData.baterai_bandul;
        (*docResult)["data"]["status"] = structData.status;
        (*docResult)["data"]["error_code"] = structData.error_code;
        (*docResult)["data"]["average_1"] = structData.average_1;
        (*docResult)["data"]["average_2"] = structData.average_2;
        (*docResult)["data"]["speed_naik"] = structData.speed_naik;
        (*docResult)["data"]["speed_turun"] = structData.speed_turun;
        (*docResult)["data"]["mejaukur"] = structData.mejaukur;
        (*docResult)["data"]["minyak"] = structData.minyak;
        (*docResult)["data"]["air"] = structData.air;
        (*docResult)["data"]["lumpur"] = structData.lumpur;
        (*docResult)["data"]["tinggi_meja"] = structData.tinggi_meja;
        (*docResult)["data"]["deviasi"] = structData.deviasi;
        (*docResult)["data"]["v_charge"] = structData.v_charge;
        (*docResult)["data"]["suhu_rtc"] = structData.suhu_rtc;
        (*docResult)["data"]["suhu_bandul"] = structData.suhu_bandul;
        (*docResult)["data"]["suhu_probe"] = structData.suhu_probe;
        (*docResult)["data"]["suhu_mek"] = structData.suhu_mek;
        (*docResult)["data"]["recorded_at"] = structData.recorded_at;
        return false;
    }

    return false;
}

bool CommandHandler::loadStruct(soil_data_struct &structData, bool internal, bool loadJson, JsonDocument *docResult) {
    JsonDocument doc;
    if (!loadJson) {
        bool hasValue = false;
        if (internal && loadSettingsInternal("soil_data", doc)) {
            return true;
        }
        if (!internal && loadSettings("soil_data", doc)) {
            return true;
        }
        if (!hasValue) {
            return true;
        }
        return false;
    }

    if (loadJson && docResult != nullptr) {
        (*docResult)["command"] = "soil_data";
        return false;
    }

    return false;
}

bool CommandHandler::loadStruct(file_read_struct &structData, bool internal, bool loadJson, JsonDocument *docResult) {
    JsonDocument doc;
    if (!loadJson) {
        bool hasValue = false;
        if (internal && loadSettingsInternal("file_read", doc)) {
            return true;
        }
        if (!internal && loadSettings("file_read", doc)) {
            return true;
        }
        if (!hasValue) {
            return true;
        }
        return false;
    }

    if (loadJson && docResult != nullptr) {
        (*docResult)["command"] = "file_read";
        return false;
    }

    return false;
}

bool CommandHandler::loadStruct(file_write_struct &structData, bool internal, bool loadJson, JsonDocument *docResult) {
    JsonDocument doc;
    if (!loadJson) {
        bool hasValue = false;
        if (internal && loadSettingsInternal("file_write", doc)) {
            return true;
        }
        if (!internal && loadSettings("file_write", doc)) {
            return true;
        }
        if (!hasValue) {
            return true;
        }
        return false;
    }

    if (loadJson && docResult != nullptr) {
        (*docResult)["command"] = "file_write";
        return false;
    }

    return false;
}

bool CommandHandler::loadStruct(file_append_struct &structData, bool internal, bool loadJson, JsonDocument *docResult) {
    JsonDocument doc;
    if (!loadJson) {
        bool hasValue = false;
        if (internal && loadSettingsInternal("file_append", doc)) {
            return true;
        }
        if (!internal && loadSettings("file_append", doc)) {
            return true;
        }
        if (!hasValue) {
            return true;
        }
        return false;
    }

    if (loadJson && docResult != nullptr) {
        (*docResult)["command"] = "file_append";
        return false;
    }

    return false;
}

bool CommandHandler::loadStruct(file_delete_struct &structData, bool internal, bool loadJson, JsonDocument *docResult) {
    JsonDocument doc;
    if (!loadJson) {
        bool hasValue = false;
        if (internal && loadSettingsInternal("file_delete", doc)) {
            return true;
        }
        if (!internal && loadSettings("file_delete", doc)) {
            return true;
        }
        if (!hasValue) {
            return true;
        }
        return false;
    }

    if (loadJson && docResult != nullptr) {
        (*docResult)["command"] = "file_delete";
        return false;
    }

    return false;
}

bool CommandHandler::loadStruct(pom_receive_struct &structData, bool internal, bool loadJson, JsonDocument *docResult) {
    JsonDocument doc;
    if (!loadJson) {
        bool hasValue = false;
        if (internal && loadSettingsInternal("pom_receive", doc)) {
            return true;
        }
        if (!internal && loadSettings("pom_receive", doc)) {
            return true;
        }
        if (!hasValue) {
            return true;
        }
        return false;
    }

    if (loadJson && docResult != nullptr) {
        (*docResult)["command"] = "pom_receive";
        return false;
    }

    return false;
}

bool CommandHandler::loadStruct(query_sql_struct &structData, bool internal, bool loadJson, JsonDocument *docResult) {
    JsonDocument doc;
    if (!loadJson) {
        bool hasValue = false;
        if (internal && loadSettingsInternal("query_sql", doc)) {
            return true;
        }
        if (!internal && loadSettings("query_sql", doc)) {
            return true;
        }
        if (!hasValue) {
            return true;
        }
        return false;
    }

    if (loadJson && docResult != nullptr) {
        (*docResult)["command"] = "query_sql";
        return false;
    }

    return false;
}

bool CommandHandler::loadStruct(fingerprint_register_struct &structData, bool internal, bool loadJson, JsonDocument *docResult) {
    JsonDocument doc;
    if (!loadJson) {
        bool hasValue = false;
        if (internal && loadSettingsInternal("fingerprint_register", doc)) {
            return true;
        }
        if (!internal && loadSettings("fingerprint_register", doc)) {
            return true;
        }
        if (!hasValue) {
            return true;
        }
        return false;
    }

    if (loadJson && docResult != nullptr) {
        (*docResult)["command"] = "fingerprint_register";
        return false;
    }

    return false;
}

bool CommandHandler::loadStruct(backup_template_fp_struct &structData, bool internal, bool loadJson, JsonDocument *docResult) {
    JsonDocument doc;
    if (!loadJson) {
        bool hasValue = false;
        if (internal && loadSettingsInternal("backup_template_fp", doc)) {
            return true;
        }
        if (!internal && loadSettings("backup_template_fp", doc)) {
            return true;
        }
        if (!hasValue) {
            return true;
        }
        return false;
    }

    if (loadJson && docResult != nullptr) {
        (*docResult)["command"] = "backup_template_fp";
        return false;
    }

    return false;
}

bool CommandHandler::loadStruct(restore_template_fp_struct &structData, bool internal, bool loadJson, JsonDocument *docResult) {
    JsonDocument doc;
    if (!loadJson) {
        bool hasValue = false;
        if (internal && loadSettingsInternal("restore_template_fp", doc)) {
            return true;
        }
        if (!internal && loadSettings("restore_template_fp", doc)) {
            return true;
        }
        if (!hasValue) {
            return true;
        }
        return false;
    }

    if (loadJson && docResult != nullptr) {
        (*docResult)["command"] = "restore_template_fp";
        return false;
    }

    return false;
}

bool CommandHandler::loadStruct(delete_template_fp_struct &structData, bool internal, bool loadJson, JsonDocument *docResult) {
    JsonDocument doc;
    if (!loadJson) {
        bool hasValue = false;
        if (internal && loadSettingsInternal("delete_template_fp", doc)) {
            return true;
        }
        if (!internal && loadSettings("delete_template_fp", doc)) {
            return true;
        }
        if (!hasValue) {
            return true;
        }
        return false;
    }

    if (loadJson && docResult != nullptr) {
        (*docResult)["command"] = "delete_template_fp";
        return false;
    }

    return false;
}

bool CommandHandler::loadStruct(admin_fp_struct &structData, bool internal, bool loadJson, JsonDocument *docResult) {
    JsonDocument doc;
    if (!loadJson) {
        bool hasValue = false;
        if (internal && loadSettingsInternal("admin_fp", doc)) {
            return true;
        }
        if (!internal && loadSettings("admin_fp", doc)) {
            return true;
        }
        if (!hasValue) {
            return true;
        }
        return false;
    }

    if (loadJson && docResult != nullptr) {
        (*docResult)["command"] = "admin_fp";
        return false;
    }

    return false;
}

bool CommandHandler::loadStruct(get_info_struct &structData, bool internal, bool loadJson, JsonDocument *docResult) {
    JsonDocument doc;
    if (!loadJson) {
        bool hasValue = false;
        if (internal && loadSettingsInternal("get_info", doc)) {
            return true;
        }
        if (!internal && loadSettings("get_info", doc)) {
            return true;
        }
        if (!hasValue) {
            return true;
        }
        return false;
    }

    if (loadJson && docResult != nullptr) {
        (*docResult)["command"] = "get_info";
        return false;
    }

    return false;
}

bool CommandHandler::loadStruct(factory_reset_struct &structData, bool internal, bool loadJson, JsonDocument *docResult) {
    JsonDocument doc;
    if (!loadJson) {
        bool hasValue = false;
        if (internal && loadSettingsInternal("factory_reset", doc)) {
            return true;
        }
        if (!internal && loadSettings("factory_reset", doc)) {
            return true;
        }
        if (!hasValue) {
            return true;
        }
        return false;
    }

    if (loadJson && docResult != nullptr) {
        (*docResult)["command"] = "factory_reset";
        return false;
    }

    return false;
}

bool CommandHandler::loadStruct(fingerprint_scan_struct &structData, bool internal, bool loadJson, JsonDocument *docResult) {
    JsonDocument doc;
    if (!loadJson) {
        bool hasValue = false;
        if (internal && loadSettingsInternal("fingerprint_scan", doc)) {
            return true;
        }
        if (!internal && loadSettings("fingerprint_scan", doc)) {
            return true;
        }
        if (!hasValue) {
            return true;
        }
        return false;
    }

    if (loadJson && docResult != nullptr) {
        (*docResult)["command"] = "fingerprint_scan";
        return false;
    }

    return false;
}

bool CommandHandler::loadStruct(stb_move_to_struct &structData, bool internal, bool loadJson, JsonDocument *docResult) {
    JsonDocument doc;
    if (!loadJson) {
        bool hasValue = false;
        if (internal && loadSettingsInternal("stb_move_to", doc)) {
            return true;
        }
        if (!internal && loadSettings("stb_move_to", doc)) {
            return true;
        }
        if (!hasValue) {
            return true;
        }
        return false;
    }

    if (loadJson && docResult != nullptr) {
        (*docResult)["command"] = "stb_move_to";
        return false;
    }

    return false;
}

bool CommandHandler::loadStruct(stb_move_detection_struct &structData, bool internal, bool loadJson, JsonDocument *docResult) {
    JsonDocument doc;
    if (!loadJson) {
        bool hasValue = false;
        if (internal && loadSettingsInternal("stb_move_detection", doc)) {
            return true;
        }
        if (!internal && loadSettings("stb_move_detection", doc)) {
            return true;
        }
        if (!hasValue) {
            return true;
        }
        return false;
    }

    if (loadJson && docResult != nullptr) {
        (*docResult)["command"] = "stb_move_detection";
        return false;
    }

    return false;
}

bool CommandHandler::loadStruct(stb_sounding_struct &structData, bool internal, bool loadJson, JsonDocument *docResult) {
    JsonDocument doc;
    if (!loadJson) {
        bool hasValue = false;
        if (internal && loadSettingsInternal("stb_sounding", doc)) {
            return true;
        }
        if (!internal && loadSettings("stb_sounding", doc)) {
            return true;
        }
        if (!hasValue) {
            return true;
        }
        return false;
    }

    if (loadJson && docResult != nullptr) {
        (*docResult)["command"] = "stb_sounding";
        return false;
    }

    return false;
}

bool CommandHandler::loadStruct(stb_sounding_all_struct &structData, bool internal, bool loadJson, JsonDocument *docResult) {
    JsonDocument doc;
    if (!loadJson) {
        bool hasValue = false;
        if (internal && loadSettingsInternal("stb_sounding_all", doc)) {
            return true;
        }
        if (!internal && loadSettings("stb_sounding_all", doc)) {
            return true;
        }
        if (!hasValue) {
            return true;
        }
        return false;
    }

    if (loadJson && docResult != nullptr) {
        (*docResult)["command"] = "stb_sounding_all";
        return false;
    }

    return false;
}

bool CommandHandler::loadStruct(save_ir_struct &structData, bool internal, bool loadJson, JsonDocument *docResult) {
    JsonDocument doc;
    if (!loadJson) {
        bool hasValue = false;
        if (internal && loadSettingsInternal("save_ir", doc)) {
            return true;
        }
        if (!internal && loadSettings("save_ir", doc)) {
            return true;
        }
        if (!hasValue) {
            return true;
        }
        return false;
    }

    if (loadJson && docResult != nullptr) {
        (*docResult)["command"] = "save_ir";
        return false;
    }

    return false;
}

bool CommandHandler::loadStruct(execute_ir_struct &structData, bool internal, bool loadJson, JsonDocument *docResult) {
    JsonDocument doc;
    if (!loadJson) {
        bool hasValue = false;
        if (internal && loadSettingsInternal("execute_ir", doc)) {
            return true;
        }
        if (!internal && loadSettings("execute_ir", doc)) {
            return true;
        }
        if (!hasValue) {
            return true;
        }
        return false;
    }

    if (loadJson && docResult != nullptr) {
        (*docResult)["command"] = "execute_ir";
        return false;
    }

    return false;
}

bool CommandHandler::loadStruct(get_ir_list_struct &structData, bool internal, bool loadJson, JsonDocument *docResult) {
    JsonDocument doc;
    if (!loadJson) {
        bool hasValue = false;
        if (internal && loadSettingsInternal("get_ir_list", doc)) {
            return true;
        }
        if (!internal && loadSettings("get_ir_list", doc)) {
            return true;
        }
        if (!hasValue) {
            return true;
        }
        return false;
    }

    if (loadJson && docResult != nullptr) {
        (*docResult)["command"] = "get_ir_list";
        return false;
    }

    return false;
}

bool CommandHandler::loadStruct(schedule_daily_struct &structData, bool internal, bool loadJson, JsonDocument *docResult) {
    JsonDocument doc;
    if (!loadJson) {
        bool hasValue = false;
        if (internal && loadSettingsInternal("schedule_daily", doc)) {
            return true;
        }
        if (!internal && loadSettings("schedule_daily", doc)) {
            return true;
        }
        if (!hasValue) {
            return true;
        }
        return false;
    }

    if (loadJson && docResult != nullptr) {
        (*docResult)["command"] = "schedule_daily";
        return false;
    }

    return false;
}

bool CommandHandler::loadStruct(schedule_monthly_struct &structData, bool internal, bool loadJson, JsonDocument *docResult) {
    JsonDocument doc;
    if (!loadJson) {
        bool hasValue = false;
        if (internal && loadSettingsInternal("schedule_monthly", doc)) {
            return true;
        }
        if (!internal && loadSettings("schedule_monthly", doc)) {
            return true;
        }
        if (!hasValue) {
            return true;
        }
        return false;
    }

    if (loadJson && docResult != nullptr) {
        (*docResult)["command"] = "schedule_monthly";
        return false;
    }

    return false;
}

bool CommandHandler::loadStruct(file_list_struct &structData, bool internal, bool loadJson, JsonDocument *docResult) {
    JsonDocument doc;
    if (!loadJson) {
        bool hasValue = false;
        if (internal && loadSettingsInternal("file_list", doc)) {
            return true;
        }
        if (!internal && loadSettings("file_list", doc)) {
            return true;
        }
        if (!hasValue) {
            return true;
        }
        return false;
    }

    if (loadJson && docResult != nullptr) {
        (*docResult)["command"] = "file_list";
        return false;
    }

    return false;
}

bool CommandHandler::loadStruct(get_schedule_struct &structData, bool internal, bool loadJson, JsonDocument *docResult) {
    JsonDocument doc;
    if (!loadJson) {
        bool hasValue = false;
        if (internal && loadSettingsInternal("get_schedule", doc)) {
            return true;
        }
        if (!internal && loadSettings("get_schedule", doc)) {
            return true;
        }
        if (!hasValue) {
            return true;
        }
        return false;
    }

    if (loadJson && docResult != nullptr) {
        (*docResult)["command"] = "get_schedule";
        return false;
    }

    return false;
}

bool CommandHandler::loadStruct(command_to_bandul_struct &structData, bool internal, bool loadJson, JsonDocument *docResult) {
    JsonDocument doc;
    if (!loadJson) {
        bool hasValue = false;
        if (internal && loadSettingsInternal("command_to_bandul", doc)) {
            return true;
        }
        if (!internal && loadSettings("command_to_bandul", doc)) {
            return true;
        }
        if (!hasValue) {
            return true;
        }
        return false;
    }

    if (loadJson && docResult != nullptr) {
        (*docResult)["command"] = "command_to_bandul";
        return false;
    }

    return false;
}

bool CommandHandler::loadStruct(soil_measure_struct &structData, bool internal, bool loadJson, JsonDocument *docResult) {
    JsonDocument doc;
    if (!loadJson) {
        bool hasValue = false;
        if (internal && loadSettingsInternal("soil_measure", doc)) {
            return true;
        }
        if (!internal && loadSettings("soil_measure", doc)) {
            return true;
        }
        if (!hasValue) {
            return true;
        }
        return false;
    }

    if (loadJson && docResult != nullptr) {
        (*docResult)["command"] = "soil_measure";
        return false;
    }

    return false;
}

bool CommandHandler::loadStruct(message_from_gateway_struct &structData, bool internal, bool loadJson, JsonDocument *docResult) {
    JsonDocument doc;
    if (!loadJson) {
        bool hasValue = false;
        if (internal && loadSettingsInternal("message_from_gateway", doc)) {
            return true;
        }
        if (!internal && loadSettings("message_from_gateway", doc)) {
            return true;
        }
        if (!hasValue) {
            return true;
        }
        return false;
    }

    if (loadJson && docResult != nullptr) {
        (*docResult)["command"] = "message_from_gateway";
        return false;
    }

    return false;
}

bool CommandHandler::loadStruct(message_from_node_struct &structData, bool internal, bool loadJson, JsonDocument *docResult) {
    JsonDocument doc;
    if (!loadJson) {
        bool hasValue = false;
        if (internal && loadSettingsInternal("message_from_node", doc)) {
            return true;
        }
        if (!internal && loadSettings("message_from_node", doc)) {
            return true;
        }
        if (!hasValue) {
            return true;
        }
        return false;
    }

    if (loadJson && docResult != nullptr) {
        (*docResult)["command"] = "message_from_node";
        return false;
    }

    return false;
}

bool CommandHandler::loadStruct(command_radio_struct &structData, bool internal, bool loadJson, JsonDocument *docResult) {
    JsonDocument doc;
    if (!loadJson) {
        bool hasValue = false;
        if (internal && loadSettingsInternal("command_radio", doc)) {
            return true;
        }
        if (!internal && loadSettings("command_radio", doc)) {
            return true;
        }
        if (!hasValue) {
            return true;
        }
        return false;
    }

    if (loadJson && docResult != nullptr) {
        (*docResult)["command"] = "command_radio";
        return false;
    }

    return false;
}

bool CommandHandler::loadStruct(command_to_mekanikal_struct &structData, bool internal, bool loadJson, JsonDocument *docResult) {
    JsonDocument doc;
    if (!loadJson) {
        bool hasValue = false;
        if (internal && loadSettingsInternal("command_to_mekanikal", doc)) {
            return true;
        }
        if (!internal && loadSettings("command_to_mekanikal", doc)) {
            return true;
        }
        if (!hasValue) {
            return true;
        }
        return false;
    }

    if (loadJson && docResult != nullptr) {
        (*docResult)["command"] = "command_to_mekanikal";
        return false;
    }

    return false;
}

bool CommandHandler::loadStruct(get_bandul_data_struct &structData, bool internal, bool loadJson, JsonDocument *docResult) {
    JsonDocument doc;
    if (!loadJson) {
        bool hasValue = false;
        if (internal && loadSettingsInternal("get_bandul_data", doc)) {
            return true;
        }
        if (!internal && loadSettings("get_bandul_data", doc)) {
            return true;
        }
        if (!hasValue) {
            return true;
        }
        return false;
    }

    if (loadJson && docResult != nullptr) {
        (*docResult)["command"] = "get_bandul_data";
        return false;
    }

    return false;
}

bool CommandHandler::loadStruct(get_data_struct &structData, bool internal, bool loadJson, JsonDocument *docResult) {
    JsonDocument doc;
    if (!loadJson) {
        bool hasValue = false;
        if (internal && loadSettingsInternal("get_data", doc)) {
            return true;
        }
        if (!internal && loadSettings("get_data", doc)) {
            return true;
        }
        if (!hasValue) {
            return true;
        }
        return false;
    }

    if (loadJson && docResult != nullptr) {
        (*docResult)["command"] = "get_data";
        return false;
    }

    return false;
}

bool CommandHandler::loadStruct(update_mekanikal_struct &structData, bool internal, bool loadJson, JsonDocument *docResult) {
    JsonDocument doc;
    if (!loadJson) {
        bool hasValue = false;
        if (internal && loadSettingsInternal("update_mekanikal", doc)) {
            return true;
        }
        if (!internal && loadSettings("update_mekanikal", doc)) {
            return true;
        }
        if (!hasValue) {
            return true;
        }
        return false;
    }

    if (loadJson && docResult != nullptr) {
        (*docResult)["command"] = "update_mekanikal";
        return false;
    }

    return false;
}

bool CommandHandler::loadStruct(upload_data_struct &structData, bool internal, bool loadJson, JsonDocument *docResult) {
    JsonDocument doc;
    if (!loadJson) {
        bool hasValue = false;
        if (internal && loadSettingsInternal("upload_data", doc)) {
            return true;
        }
        if (!internal && loadSettings("upload_data", doc)) {
            return true;
        }
        if (!hasValue) {
            return true;
        }
        return false;
    }

    if (loadJson && docResult != nullptr) {
        (*docResult)["command"] = "upload_data";
        return false;
    }

    return false;
}

bool CommandHandler::loadStruct(check_data_struct &structData, bool internal, bool loadJson, JsonDocument *docResult) {
    JsonDocument doc;
    if (!loadJson) {
        bool hasValue = false;
        if (internal && loadSettingsInternal("check_data", doc)) {
            return true;
        }
        if (!internal && loadSettings("check_data", doc)) {
            return true;
        }
        if (!hasValue) {
            return true;
        }
        return false;
    }

    if (loadJson && docResult != nullptr) {
        (*docResult)["command"] = "check_data";
        return false;
    }

    return false;
}

bool CommandHandler::loadStruct(clear_schedule_struct &structData, bool internal, bool loadJson, JsonDocument *docResult) {
    JsonDocument doc;
    if (!loadJson) {
        bool hasValue = false;
        if (internal && loadSettingsInternal("clear_schedule", doc)) {
            return true;
        }
        if (!internal && loadSettings("clear_schedule", doc)) {
            return true;
        }
        if (!hasValue) {
            return true;
        }
        return false;
    }

    if (loadJson && docResult != nullptr) {
        (*docResult)["command"] = "clear_schedule";
        return false;
    }

    return false;
}

bool CommandHandler::loadStruct(stb_check_air_struct &structData, bool internal, bool loadJson, JsonDocument *docResult) {
    JsonDocument doc;
    if (!loadJson) {
        bool hasValue = false;
        if (internal && loadSettingsInternal("stb_check_air", doc)) {
            return true;
        }
        if (!internal && loadSettings("stb_check_air", doc)) {
            return true;
        }
        if (!hasValue) {
            return true;
        }
        return false;
    }

    if (loadJson && docResult != nullptr) {
        (*docResult)["command"] = "stb_check_air";
        return false;
    }

    return false;
}

