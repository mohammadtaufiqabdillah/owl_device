const char* setupSetJson = R"(
{
    "mqtt": {
        "commandId": 2,
        "key": [
            "host",
            "port",
            "username",
            "password"
        ],
        "data_type": [
            "string",
            "uint16_t",
            "string",
            "string"
        ],
        "data_len": [
            125,
            2,
            25,
            25
        ],
        "default": [
            "8.215.79.132",
            1883,
            "user_mqtt",
            "Pa55w0rd"
        ],
        "queue": "mqttqueue",
        "path": "/mqtt"
    },
    "use_key": {
        "commandId": 7,
        "key": [],
        "data_type": [],
        "data_len": [],
        "default": [],
        "queue": "use_keyqueue",
        "path": "/use_key"
    },
    "stb_bandul_setup": {
        "commandId": 22,
        "key": [],
        "data_type": [],
        "data_len": [],
        "default": [],
        "queue": "stb_bandul_setupqueue",
        "path": "/stb_bandul_setup"
    },
    "stb_mekanikal_setup": {
        "commandId": 25,
        "key": [],
        "data_type": [],
        "data_len": [],
        "default": [],
        "queue": "stb_mekanikal_setupqueue",
        "path": "/stb_mekanikal_setup"
    },
    "interval": {
        "commandId": 3,
        "key": [],
        "data_type": [],
        "data_len": [],
        "default": [],
        "queue": "intervalqueue",
        "path": "/interval"
    },
    "utcoffset": {
        "commandId": 4,
        "key": [],
        "data_type": [],
        "data_len": [],
        "default": [],
        "queue": "utcoffsetqueue",
        "path": "/utcoffset"
    },
    "last_epoch": {
        "commandId": 5,
        "key": [],
        "data_type": [],
        "data_len": [],
        "default": [],
        "queue": "last_epochqueue",
        "path": "/last_epoch"
    },
    "key": {
        "commandId": 6,
        "key": [],
        "data_type": [],
        "data_len": [],
        "default": [],
        "queue": "keyqueue",
        "path": "/key"
    },
    "sn": {
        "commandId": 8,
        "key": [],
        "data_type": [],
        "data_len": [],
        "default": [],
        "queue": "snqueue",
        "path": "/sn"
    },
    "otaurl": {
        "commandId": 9,
        "key": [
            "otaurl"
        ],
        "data_type": [
            "string"
        ],
        "data_len": [
            125
        ],
        "default": [
            "http://103.150.191.136/owl_inventory/produksi/firmware.php"
        ],
        "queue": "otaurlqueue",
        "path": "/otaurl"
    },
    "api_ver1": {
        "commandId": 10,
        "key": [],
        "data_type": [],
        "data_len": [],
        "default": [],
        "queue": "api_ver1queue",
        "path": "/api_ver1"
    },
    "api_ver2": {
        "commandId": 11,
        "key": [],
        "data_type": [],
        "data_len": [],
        "default": [],
        "queue": "api_ver2queue",
        "path": "/api_ver2"
    },
    "adv_ble": {
        "commandId": 12,
        "key": [],
        "data_type": [],
        "data_len": [],
        "default": [],
        "queue": "adv_blequeue",
        "path": "/adv_ble"
    },
    "radio_mode": {
        "commandId": 13,
        "key": [],
        "data_type": [],
        "data_len": [],
        "default": [],
        "queue": "radio_modequeue",
        "path": "/radio_mode"
    },
    "radio_param_1": {
        "commandId": 14,
        "key": [],
        "data_type": [],
        "data_len": [],
        "default": [],
        "queue": "radio_param_1queue",
        "path": "/radio_param_1"
    },
    "radio_param_2": {
        "commandId": 15,
        "key": [],
        "data_type": [],
        "data_len": [],
        "default": [],
        "queue": "radio_param_2queue",
        "path": "/radio_param_2"
    },
    "radio_param_3": {
        "commandId": 16,
        "key": [],
        "data_type": [],
        "data_len": [],
        "default": [],
        "queue": "radio_param_3queue",
        "path": "/radio_param_3"
    },
    "radio_param_4": {
        "commandId": 17,
        "key": [],
        "data_type": [],
        "data_len": [],
        "default": [],
        "queue": "radio_param_4queue",
        "path": "/radio_param_4"
    },
    "license": {
        "commandId": 18,
        "key": [],
        "data_type": [],
        "data_len": [],
        "default": [],
        "queue": "licensequeue",
        "path": "/license"
    },
    "suspend": {
        "commandId": 19,
        "key": [],
        "data_type": [],
        "data_len": [],
        "default": [],
        "queue": "suspendqueue",
        "path": "/suspend"
    },
    "hierarchy_setup": {
        "commandId": 20,
        "key": [],
        "data_type": [],
        "data_len": [],
        "default": [],
        "queue": "hierarchy_setupqueue",
        "path": "/hierarchy_setup"
    },
    "fingerprint_setup": {
        "commandId": 21,
        "key": [],
        "data_type": [],
        "data_len": [],
        "default": [],
        "queue": "fingerprint_setupqueue",
        "path": "/fingerprint_setup"
    },
    "pom_setup": {
        "commandId": 23,
        "key": [],
        "data_type": [],
        "data_len": [],
        "default": [],
        "queue": "pom_setupqueue",
        "path": "/pom_setup"
    },
    "stb_controller_setup": {
        "commandId": 24,
        "key": [],
        "data_type": [],
        "data_len": [],
        "default": [],
        "queue": "stb_controller_setupqueue",
        "path": "/stb_controller_setup"
    },
    "tracker_setup": {
        "commandId": 26,
        "key": [],
        "data_type": [],
        "data_len": [],
        "default": [],
        "queue": "tracker_setupqueue",
        "path": "/tracker_setup"
    },
    "stb_controller_sub_setup": {
        "commandId": 27,
        "key": [],
        "data_type": [],
        "data_len": [],
        "default": [],
        "queue": "stb_controller_sub_setupqueue",
        "path": "/stb_controller_sub_setup"
    },
    "touch_calibration": {
        "commandId": 28,
        "key": [],
        "data_type": [],
        "data_len": [],
        "default": [],
        "queue": "touch_calibrationqueue",
        "path": "/touch_calibration"
    },
    "esp_now": {
        "commandId": 29,
        "key": [],
        "data_type": [],
        "data_len": [],
        "default": [],
        "queue": "esp_nowqueue",
        "path": "/esp_now"
    },
    "functional_flag": {
        "commandId": 30,
        "key": [],
        "data_type": [],
        "data_len": [],
        "default": [],
        "queue": "functional_flagqueue",
        "path": "/functional_flag"
    }
}
)";
