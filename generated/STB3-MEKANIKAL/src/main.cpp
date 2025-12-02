static const char* TAG = "MAIN";
#include <Arduino.h>
#include <commondef.h>
#include <ArduinoJson.h>
//#include <StreamString.h>
#include <commandHandler.h>
#include <commonvar.h>
#include <buttonpress.h>
#include <scheduleTask.h>
#include <mqttTask.h>
#include <bleTask.h>
#include <gpsApp.h>
#include <watchDog.h>
#include <tempNow.h>
#include <radioTask.h>
#if CI_USE_PSRAM
CommandHandler handler(FILE_SYSTEM,allocator);
#else
CommandHandler handler(FILE_SYSTEM);
#endif


radio_mode_struct radioParam;
void setup() {
  Serial.begin(115200);
  if(FILE_SYSTEM.begin()){
    ESP_LOGD(TAG,"Little FS Start");
  }

  else{
    FILE_SYSTEM.format();
    esp_restart();
    ESP_LOGE(TAG,"Little FS Failed");
  }

  if (handler.begin()){
    ESP_LOGI(TAG,"Command handler started");
    if(handler.getFileSystem().exists("/otaurl"))
      download_ota();
  }
  else{
    ESP_LOGE(TAG,"Command handler failed to start");
  }
  default_command();
  vTaskDelay(1000/portTICK_PERIOD_MS);
  //radio_task();
  vTaskDelay(1000/portTICK_PERIOD_MS);
  mqtt_task();
  ble_task();
  schedule_task();
  gps_task();
  watchdog_task();
  
  //tempnow_task();
  vTaskDelay(1000/portTICK_PERIOD_MS);
  //temnow_start_listen_bandul();
  uint8_t mac[6];
  char mac_string[35];
  // Baca MAC address Bluetooth
  esp_err_t ret = esp_read_mac(mac, ESP_MAC_BT);
  if(ret != ESP_OK) {
      ESP_LOGE("","GAGAL AMBIL MAC"); // Gagal membaca MAC
  }
  
  // Format MAC address ke dalam string
  snprintf(mac_string, sizeof(mac_string), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  
  ESP_LOGI("DEBUG","MAC BT %s, MAC WIFI %s\n",mac_string,WiFi.macAddress().c_str());
  handler.loadStruct(radioParam);
  

  vTaskDelay(10000/portTICK_PERIOD_MS);

  stb_sounding_data_struct soundingData;
  soundingData.tinggi_minyak_dari_atas=0.0;
  soundingData.tinggi_minyak_dari_bawah=0.0;
  soundingData.tinggi_udara_dari_atas=0.0;
  soundingData.tinggi_udara_dari_bawah=0.0;
  soundingData.suhu=0.0;
  soundingData.baterai_bandul=0.0;
  soundingData.status=false;
  soundingData.error_code=0;
  soundingData.average_1=0.0;
  soundingData.average_2=0.0;
  soundingData.speed_naik=0;
  soundingData.speed_turun=0;
  soundingData.mejaukur=0.0;
  soundingData.minyak=0.0;
  soundingData.air=0.0;
  soundingData.lumpur=0.0;
  soundingData.tinggi_meja=0.0;
  soundingData.deviasi=0.0;
  soundingData.v_charge=0.0;
  soundingData.suhu_rtc=0.0;
  soundingData.suhu_bandul=0.0;
  soundingData.suhu_probe=0.0;
  soundingData.suhu_mek=0.0;
  soundingData.recorded_at=1760901313;
  if(saveStructData("stb_sounding_data", &soundingData, sizeof(soundingData))){
    ESP_LOGI(TAG,"Stb sounding data saved");
  }
  else{
    ESP_LOGE(TAG,"Failed to save stb sounding data");
  }
  pom_data_struct pomData;
  pomData.transaksi_id=0;
  pomData.liter=0.0;
  strcpy(pomData.plat,"D 7777 XX");
  pomData.karyawan_id=0;
  pomData.stock=0.0;
  strcpy(pomData.date,"2023-01-01 00:00:00");
  if(saveStructData("pom_data", &pomData, sizeof(pomData))){
    ESP_LOGI(TAG,"Pom data saved");
  }
  else{
    ESP_LOGE(TAG,"Failed to save pom data");
  }
}

uint32_t last_millis=0;
void loop() {
  vTaskDelay(10/portTICK_PERIOD_MS);
  //Contoh untuk mengambil setup dari serial (jarang di gunakan)
  if(Serial.available()>0){
    CREATE_JSON_DOCUMENT(doc);
    DeserializationError err=deserializeJson(doc,Serial);
    while(Serial.available()>0&&Serial.read()!=10);//buang char lebihan
    if(err)
      ESP_LOGE(TAG,"Command from serial invalid %s",err.c_str());
    handler.processCommand(doc);
    serializeJsonPretty(doc,Serial);
    Serial.println("");
  }

  //Contoh event flag jika ada perubahan setup terhatap otaurl (sumber setup bisa dari mana saja (radio, ble, mqtt))
  if(uxQueueMessagesWaiting(otaurlQueue)>0){//there is update to ota url
    bool newValues=false;
    xQueueReceive(otaurlQueue,&newValues,0);
    esp_restart();
  }

  vTaskDelay(5000/portTICK_PERIOD_MS);
}