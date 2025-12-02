const char* setupSetJson = R"(
{
    "wifi": {
        "commandId": 6,
        "key": [
            "ssid",
            "password"
        ],
        "data_type": [
            "string",
            "string"
        ],
        "data_len": [
            25,
            25
        ],
        "default": [
            "87654321",
            "Devices"
        ],
        "queue": "wifiqueue",
        "path": "/wifi"
    },
    "interval": {
        "commandId": 8,
        "key": [
            "get_data",
            "upload_data",
            "standby"
        ],
        "data_type": [
            "uint32_t",
            "uint32_t",
            "uint32_t"
        ],
        "data_len": [
            4,
            4,
            4
        ],
        "default": [
            60,
            600,
            60
        ],
        "queue": "intervalqueue",
        "path": "/interval"
    }
}
)";
