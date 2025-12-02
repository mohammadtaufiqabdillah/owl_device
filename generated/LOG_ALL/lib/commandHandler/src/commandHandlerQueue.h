#ifndef COMMANDHANDLERQUEUE_H
#define COMMANDHANDLERQUEUE_H
#include <Arduino.h>

extern SemaphoreHandle_t mutex_handler;

// Queue Declarations
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

struct mqtt_struct {
    char host[125];
    uint16_t port;
    char username[25];
    char password[25];
};

struct use_key_struct {
    // no fields defined for use_key
};

struct stb_bandul_setup_struct {
    // no fields defined for stb_bandul_setup
};

struct stb_mekanikal_setup_struct {
    // no fields defined for stb_mekanikal_setup
};

struct restart_struct {
    // no fields defined for restart
};

struct interval_struct {
    // no fields defined for interval
};

struct utcoffset_struct {
    // no fields defined for utcoffset
};

struct last_epoch_struct {
    // no fields defined for last_epoch
};

struct key_struct {
    // no fields defined for key
};

struct sn_struct {
    // no fields defined for sn
};

struct otaurl_struct {
    char otaurl[125];
};

struct api_ver1_struct {
    // no fields defined for api_ver1
};

struct api_ver2_struct {
    // no fields defined for api_ver2
};

struct adv_ble_struct {
    // no fields defined for adv_ble
};

struct radio_mode_struct {
    // no fields defined for radio_mode
};

struct radio_param_1_struct {
    // no fields defined for radio_param_1
};

struct radio_param_2_struct {
    // no fields defined for radio_param_2
};

struct radio_param_3_struct {
    // no fields defined for radio_param_3
};

struct radio_param_4_struct {
    // no fields defined for radio_param_4
};

struct license_struct {
    // no fields defined for license
};

struct suspend_struct {
    // no fields defined for suspend
};

struct hierarchy_setup_struct {
    // no fields defined for hierarchy_setup
};

struct fingerprint_setup_struct {
    // no fields defined for fingerprint_setup
};

struct pom_setup_struct {
    // no fields defined for pom_setup
};

struct stb_controller_setup_struct {
    // no fields defined for stb_controller_setup
};

struct tracker_setup_struct {
    // no fields defined for tracker_setup
};

struct stb_controller_sub_setup_struct {
    // no fields defined for stb_controller_sub_setup
};

struct touch_calibration_struct {
    // no fields defined for touch_calibration
};

struct esp_now_struct {
    // no fields defined for esp_now
};

struct functional_flag_struct {
    // no fields defined for functional_flag
};

struct pom_data_struct {
    uint32_t transaksi_id;
    float liter;
    char plat[15];
    uint32_t karyawan_id;
    float stock;
    char date[23];
};

struct radio_pooling_data_struct {
    // no fields defined for radio_pooling_data
};

struct bandul_data_struct {
    // no fields defined for bandul_data
};

struct logger_gps_data_struct {
    // no fields defined for logger_gps_data
};

struct logger_vehicle_gps_data_struct {
    // no fields defined for logger_vehicle_gps_data
};

struct radio_broadcast_data_struct {
    // no fields defined for radio_broadcast_data
};

struct radio_join_data_struct {
    // no fields defined for radio_join_data
};

struct radio_response_data_struct {
    // no fields defined for radio_response_data
};

struct stb_notification_struct {
    // no fields defined for stb_notification
};

struct stb_sounding_data_struct {
    float tinggi_minyak_dari_atas;
    float tinggi_minyak_dari_bawah;
    float tinggi_udara_dari_atas;
    float tinggi_udara_dari_bawah;
    float suhu;
    float baterai_bandul;
    bool status;
    uint32_t error_code;
    float average_1;
    float average_2;
    int16_t speed_naik;
    int16_t speed_turun;
    float mejaukur;
    float minyak;
    float air;
    float lumpur;
    float tinggi_meja;
    float deviasi;
    float v_charge;
    float suhu_rtc;
    float suhu_bandul;
    float suhu_probe;
    float suhu_mek;
    uint32_t recorded_at;
};

struct soil_data_struct {
    // no fields defined for soil_data
};

struct file_read_struct {
    // no fields defined for file_read
};

struct file_write_struct {
    // no fields defined for file_write
};

struct file_append_struct {
    // no fields defined for file_append
};

struct file_delete_struct {
    // no fields defined for file_delete
};

struct pom_receive_struct {
    // no fields defined for pom_receive
};

struct query_sql_struct {
    // no fields defined for query_sql
};

struct fingerprint_register_struct {
    // no fields defined for fingerprint_register
};

struct backup_template_fp_struct {
    // no fields defined for backup_template_fp
};

struct restore_template_fp_struct {
    // no fields defined for restore_template_fp
};

struct delete_template_fp_struct {
    // no fields defined for delete_template_fp
};

struct admin_fp_struct {
    // no fields defined for admin_fp
};

struct get_info_struct {
    // no fields defined for get_info
};

struct factory_reset_struct {
    // no fields defined for factory_reset
};

struct fingerprint_scan_struct {
    // no fields defined for fingerprint_scan
};

struct stb_move_to_struct {
    // no fields defined for stb_move_to
};

struct stb_move_detection_struct {
    // no fields defined for stb_move_detection
};

struct stb_sounding_struct {
    // no fields defined for stb_sounding
};

struct stb_sounding_all_struct {
    // no fields defined for stb_sounding_all
};

struct save_ir_struct {
    // no fields defined for save_ir
};

struct execute_ir_struct {
    // no fields defined for execute_ir
};

struct get_ir_list_struct {
    // no fields defined for get_ir_list
};

struct schedule_daily_struct {
    // no fields defined for schedule_daily
};

struct schedule_monthly_struct {
    // no fields defined for schedule_monthly
};

struct file_list_struct {
    // no fields defined for file_list
};

struct get_schedule_struct {
    // no fields defined for get_schedule
};

struct command_to_bandul_struct {
    // no fields defined for command_to_bandul
};

struct soil_measure_struct {
    // no fields defined for soil_measure
};

struct message_from_gateway_struct {
    // no fields defined for message_from_gateway
};

struct message_from_node_struct {
    // no fields defined for message_from_node
};

struct command_radio_struct {
    // no fields defined for command_radio
};

struct command_to_mekanikal_struct {
    // no fields defined for command_to_mekanikal
};

struct get_bandul_data_struct {
    // no fields defined for get_bandul_data
};

struct get_data_struct {
    // no fields defined for get_data
};

struct update_mekanikal_struct {
    // no fields defined for update_mekanikal
};

struct upload_data_struct {
    // no fields defined for upload_data
};

struct check_data_struct {
    // no fields defined for check_data
};

struct clear_schedule_struct {
    // no fields defined for clear_schedule
};

struct stb_check_air_struct {
    // no fields defined for stb_check_air
};



#pragma pack()
#endif