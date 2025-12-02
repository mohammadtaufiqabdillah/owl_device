const char* setupDatJson = R"(
{
    "pom_data": {
        "commandId": 5,
        "key": [
            "transaksi_id",
            "liter",
            "plat",
            "karyawan_id",
            "stock",
            "date"
        ],
        "data_type": [
            "uint32_t",
            "float",
            "string",
            "uint32_t",
            "float",
            "string"
        ],
        "data_len": [
            4,
            4,
            15,
            4,
            4,
            23
        ],
        "default": [
            0,
            0,
            "AA 4567 ZZZ",
            0,
            100,
            "2025 09 06 55:66:89"
        ],
        "queue": "pom_dataqueue",
        "path": "/pom_data"
    }
}
)";
