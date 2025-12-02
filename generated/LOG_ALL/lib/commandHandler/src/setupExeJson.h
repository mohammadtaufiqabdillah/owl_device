const char* setupExeJson = R"(
{
    "restart": {
        "commandId": 1,
        "key": [],
        "data_type": [],
        "data_len": [],
        "default": [],
        "queue": "restartqueue",
        "path": "/restart"
    },
    "file_read": {
        "commandId": 2,
        "key": [],
        "data_type": [],
        "data_len": [],
        "default": [],
        "queue": "file_readqueue",
        "path": "/file_read"
    },
    "file_write": {
        "commandId": 3,
        "key": [],
        "data_type": [],
        "data_len": [],
        "default": [],
        "queue": "file_writequeue",
        "path": "/file_write"
    },
    "file_append": {
        "commandId": 4,
        "key": [],
        "data_type": [],
        "data_len": [],
        "default": [],
        "queue": "file_appendqueue",
        "path": "/file_append"
    },
    "file_delete": {
        "commandId": 5,
        "key": [],
        "data_type": [],
        "data_len": [],
        "default": [],
        "queue": "file_deletequeue",
        "path": "/file_delete"
    },
    "pom_receive": {
        "commandId": 6,
        "key": [],
        "data_type": [],
        "data_len": [],
        "default": [],
        "queue": "pom_receivequeue",
        "path": "/pom_receive"
    },
    "query_sql": {
        "commandId": 7,
        "key": [],
        "data_type": [],
        "data_len": [],
        "default": [],
        "queue": "query_sqlqueue",
        "path": "/query_sql"
    },
    "fingerprint_register": {
        "commandId": 8,
        "key": [],
        "data_type": [],
        "data_len": [],
        "default": [],
        "queue": "fingerprint_registerqueue",
        "path": "/fingerprint_register"
    },
    "backup_template_fp": {
        "commandId": 9,
        "key": [],
        "data_type": [],
        "data_len": [],
        "default": [],
        "queue": "backup_template_fpqueue",
        "path": "/backup_template_fp"
    },
    "restore_template_fp": {
        "commandId": 10,
        "key": [],
        "data_type": [],
        "data_len": [],
        "default": [],
        "queue": "restore_template_fpqueue",
        "path": "/restore_template_fp"
    },
    "delete_template_fp": {
        "commandId": 11,
        "key": [],
        "data_type": [],
        "data_len": [],
        "default": [],
        "queue": "delete_template_fpqueue",
        "path": "/delete_template_fp"
    },
    "admin_fp": {
        "commandId": 12,
        "key": [],
        "data_type": [],
        "data_len": [],
        "default": [],
        "queue": "admin_fpqueue",
        "path": "/admin_fp"
    },
    "get_info": {
        "commandId": 13,
        "key": [],
        "data_type": [],
        "data_len": [],
        "default": [],
        "queue": "get_infoqueue",
        "path": "/get_info"
    },
    "factory_reset": {
        "commandId": 14,
        "key": [],
        "data_type": [],
        "data_len": [],
        "default": [],
        "queue": "factory_resetqueue",
        "path": "/factory_reset"
    },
    "fingerprint_scan": {
        "commandId": 15,
        "key": [],
        "data_type": [],
        "data_len": [],
        "default": [],
        "queue": "fingerprint_scanqueue",
        "path": "/fingerprint_scan"
    },
    "stb_move_to": {
        "commandId": 16,
        "key": [],
        "data_type": [],
        "data_len": [],
        "default": [],
        "queue": "stb_move_toqueue",
        "path": "/stb_move_to"
    },
    "stb_move_detection": {
        "commandId": 17,
        "key": [],
        "data_type": [],
        "data_len": [],
        "default": [],
        "queue": "stb_move_detectionqueue",
        "path": "/stb_move_detection"
    },
    "stb_sounding": {
        "commandId": 18,
        "key": [],
        "data_type": [],
        "data_len": [],
        "default": [],
        "queue": "stb_soundingqueue",
        "path": "/stb_sounding"
    },
    "stb_sounding_all": {
        "commandId": 19,
        "key": [],
        "data_type": [],
        "data_len": [],
        "default": [],
        "queue": "stb_sounding_allqueue",
        "path": "/stb_sounding_all"
    },
    "save_ir": {
        "commandId": 20,
        "key": [],
        "data_type": [],
        "data_len": [],
        "default": [],
        "queue": "save_irqueue",
        "path": "/save_ir"
    },
    "execute_ir": {
        "commandId": 21,
        "key": [],
        "data_type": [],
        "data_len": [],
        "default": [],
        "queue": "execute_irqueue",
        "path": "/execute_ir"
    },
    "get_ir_list": {
        "commandId": 22,
        "key": [],
        "data_type": [],
        "data_len": [],
        "default": [],
        "queue": "get_ir_listqueue",
        "path": "/get_ir_list"
    },
    "schedule_daily": {
        "commandId": 23,
        "key": [],
        "data_type": [],
        "data_len": [],
        "default": [],
        "queue": "schedule_dailyqueue",
        "path": "/schedule_daily"
    },
    "schedule_monthly": {
        "commandId": 24,
        "key": [],
        "data_type": [],
        "data_len": [],
        "default": [],
        "queue": "schedule_monthlyqueue",
        "path": "/schedule_monthly"
    },
    "file_list": {
        "commandId": 25,
        "key": [],
        "data_type": [],
        "data_len": [],
        "default": [],
        "queue": "file_listqueue",
        "path": "/file_list"
    },
    "get_schedule": {
        "commandId": 26,
        "key": [],
        "data_type": [],
        "data_len": [],
        "default": [],
        "queue": "get_schedulequeue",
        "path": "/get_schedule"
    },
    "command_to_bandul": {
        "commandId": 27,
        "key": [],
        "data_type": [],
        "data_len": [],
        "default": [],
        "queue": "command_to_bandulqueue",
        "path": "/command_to_bandul"
    },
    "soil_measure": {
        "commandId": 28,
        "key": [],
        "data_type": [],
        "data_len": [],
        "default": [],
        "queue": "soil_measurequeue",
        "path": "/soil_measure"
    },
    "message_from_gateway": {
        "commandId": 29,
        "key": [],
        "data_type": [],
        "data_len": [],
        "default": [],
        "queue": "message_from_gatewayqueue",
        "path": "/message_from_gateway"
    },
    "message_from_node": {
        "commandId": 30,
        "key": [],
        "data_type": [],
        "data_len": [],
        "default": [],
        "queue": "message_from_nodequeue",
        "path": "/message_from_node"
    },
    "command_radio": {
        "commandId": 31,
        "key": [],
        "data_type": [],
        "data_len": [],
        "default": [],
        "queue": "command_radioqueue",
        "path": "/command_radio"
    },
    "command_to_mekanikal": {
        "commandId": 32,
        "key": [],
        "data_type": [],
        "data_len": [],
        "default": [],
        "queue": "command_to_mekanikalqueue",
        "path": "/command_to_mekanikal"
    },
    "get_bandul_data": {
        "commandId": 33,
        "key": [],
        "data_type": [],
        "data_len": [],
        "default": [],
        "queue": "get_bandul_dataqueue",
        "path": "/get_bandul_data"
    },
    "get_data": {
        "commandId": 34,
        "key": [],
        "data_type": [],
        "data_len": [],
        "default": [],
        "queue": "get_dataqueue",
        "path": "/get_data"
    },
    "update_mekanikal": {
        "commandId": 35,
        "key": [],
        "data_type": [],
        "data_len": [],
        "default": [],
        "queue": "update_mekanikalqueue",
        "path": "/update_mekanikal"
    },
    "upload_data": {
        "commandId": 36,
        "key": [],
        "data_type": [],
        "data_len": [],
        "default": [],
        "queue": "upload_dataqueue",
        "path": "/upload_data"
    },
    "check_data": {
        "commandId": 37,
        "key": [],
        "data_type": [],
        "data_len": [],
        "default": [],
        "queue": "check_dataqueue",
        "path": "/check_data"
    },
    "clear_schedule": {
        "commandId": 38,
        "key": [],
        "data_type": [],
        "data_len": [],
        "default": [],
        "queue": "clear_schedulequeue",
        "path": "/clear_schedule"
    },
    "stb_check_air": {
        "commandId": 39,
        "key": [],
        "data_type": [],
        "data_len": [],
        "default": [],
        "queue": "stb_check_airqueue",
        "path": "/stb_check_air"
    }
}
)";
