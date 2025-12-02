#include <commonvar.h>
#include <Update.h>
#include <esp_wifi.h>

QueueHandle_t gpsStatusQueue;

QueueHandle_t powerStatusQueue;

QueueHandle_t unixTimeStatusQueue;

QueueHandle_t bleEventQueue;

QueueHandle_t bleAdvertismentQueue;

QueueHandle_t bleDataQueue;

xTaskHandle mqttTaskHandler;

xTaskHandle bleTaskHandler;

xTaskHandle scheduleTaskHandler;

xTaskHandle gpsTaskHandler;

xTaskHandle radioTaskHandler;

xTaskHandle watchDogTaskHandler;

xTaskHandle tempNowTaskHandler;

//MALLOC_CAP_SPIRAM 
SpiRamAllocator allocator;

DateTime getDateTime(){
  uint32_t unixtime=0;
  DateTime now;
  if(unixTimeStatusQueue!=NULL && uxQueueMessagesWaiting(unixTimeStatusQueue)>0){
    xQueuePeek(unixTimeStatusQueue,&unixtime,0);
    now=DateTime(unixtime);
  }
  return now;
}

gps_status_struct getGpsStatus(){
  gps_status_struct gpsStatus;
  if(gpsStatusQueue!=NULL){
  xQueuePeek(gpsStatusQueue,&gpsStatus,0);
  }
  return gpsStatus;
}

power_status_struct getPowerStatus(){
  power_status_struct powerStatus;
  if(powerStatusQueue!=NULL){
    xQueuePeek(powerStatusQueue,&powerStatus,0);
    }
    return powerStatus;
}

bool get_bt_mac_string(char *mac_str, size_t buffer_size) {
  uint8_t mac[6];
  
  // Baca MAC address Bluetooth
  esp_err_t ret = esp_read_mac(mac, ESP_MAC_BT);
  if(ret != ESP_OK) {
      return false; // Gagal membaca MAC
  }
  
  // Pastikan buffer cukup besar (17 karakter + null terminator)
  if(buffer_size < 18) {
      return false; // Buffer terlalu kecil
  }
  
  // Format MAC address ke dalam string
  snprintf(mac_str, buffer_size, "%02x:%02x:%02x:%02x:%02x:%02x",
           mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  
  return true;
}

uint16_t mac_to_chip_id_16(const char* mac){
  char mac_wifi [25];
  strcpy(mac_wifi,mac);
  uint8_t bssid[6];
  sscanf(mac_wifi, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx", &bssid[0], &bssid[1], &bssid[2], &bssid[3], &bssid[4], &bssid[5]);
  bssid[5]-2;
  uint32_t high     = bssid[4]|bssid[5]<<8&0xFFFFFFFF;
  uint32_t low    = bssid[0]|bssid[1]<<8|bssid[2]<<16|bssid[3]<<24;
  return low+high;
}

uint16_t getID16(){
          char mac [35];
          get_bt_mac_string(mac,sizeof(mac));
          return mac_to_chip_id_16(mac);
}

uint32_t getID32(){
  char mac [35];
  get_bt_mac_string(mac,sizeof(mac));

  return mac_to_chip_id(mac);
}

uint32_t mac_to_chip_id(const char* mac){
  char mac_wifi [25];
  strcpy(mac_wifi,mac);
  uint8_t bssid[6];
  sscanf(mac_wifi, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx", &bssid[0], &bssid[1], &bssid[2], &bssid[3], &bssid[4], &bssid[5]);
  uint32_t high     = bssid[4]|bssid[5]<<8&0xFFFFFFFF;
  uint32_t low    = bssid[0]|bssid[1]<<8|bssid[2]<<16|bssid[3]<<24;
  return low+high;
}

void print_high_water(const char *task, uint32_t *last_time){
  if(millis()-*last_time>60000){
    *last_time=millis();
    uint32_t highwater=uxTaskGetStackHighWaterMark(NULL);
    ESP_LOGI("HIGHWATER","-----%s-----%u-----bytes",task,highwater);
  }
}

float mapFloat(float Input, float batasBawah, float batasAtas, float valueBawah, float valueAtas){
  float a=batasAtas-batasBawah;
  float b=valueAtas-valueBawah;
  float c=Input-batasBawah;
  return (c*b/a)-valueBawah;
}

void download_ota(){
  otaurl_struct ota;
  if(!handler.loadStruct(ota)){
    ESP_LOGE("","Failed to load ota url");
    return;
  }
  wifi_struct wifi;
  if(!handler.loadStruct(wifi)){
    ESP_LOGE("","Failed to load wifi config");
    return;
  }
  handler.getFileSystem().remove("/otaurl");
  WiFi.mode(WIFI_STA);
  esp_wifi_set_protocol(WIFI_IF_STA,WIFI_PROTOCOL_11B | WIFI_PROTOCOL_11G | WIFI_PROTOCOL_11N);  
  WiFi.begin(wifi.ssid,wifi.password);
  int retry=0;
  while(WiFi.status()!=WL_CONNECTED&&retry<6){
    retry++;
    ESP_LOGI("","Connecting to WiFi... %d",retry);
    vTaskDelay(20000/portTICK_PERIOD_MS);
  }

  if(WiFi.status()==WL_CONNECTED){
    HTTPClient http;
    http.begin(ota.otaurl);
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
    ESP_LOGI("","[HTTP] GET...\n");
    int httpCode = http.GET();
        ESP_LOGI("","[HTTP] GET... code: %d\n", httpCode);
        if(httpCode == HTTP_CODE_OK) {
          int len = http.getSize();
          int max_len=len;
          int persen;
          int percentage;
          uint8_t buff[128] = { 0 };
          WiFiClient * stream = http.getStreamPtr();
          bool firstTime=true;
          while(http.connected() && (len > 0 || len == -1)) {
            if(firstTime){//mulai update
              firstTime=false;
              if(!Update.begin(len))
                break;
            }
            
            size_t size = stream->available();
            if (size) {
              int c = stream->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));
              Update.write(buff, c);
              if(len > 0) {
                len -= c;
              }
            }
            
            persen=map(len,0,max_len,100,0);
            if(percentage!=persen){
              percentage=persen;
              ESP_LOGI("","Update %d %\n",percentage);
            }
            vTaskDelay(1/portTICK_PERIOD_MS);
          }
          
          if(percentage==100){//start updating
            if (Update.end()) {
              ESP_LOGI("","OTA done!\n");
              if (Update.isFinished()) {
                  ESP_LOGI("","Update successfully completed. Rebooting.");
                  esp_restart();
              }
              else {
                  ESP_LOGE("","Update not finished? Something went wrong!\n");
              }
            }
          }
        }
        
        else {
                ESP_LOGE("","[HTTP] GET... failed, error: %s\n");
                ESP_LOGE("","%s\n",http.errorToString(httpCode).c_str());
        }

    http.end();
  }    
}

bool update_advertisment_data (headerCommandStruct &data){
  if(bleEventQueue!=NULL && bleAdvertismentQueue!=NULL){
    bool event=true;
    ble_adv_struct newData;
    newData.len=data.len;
    memcpy(newData.data,data.data,sizeof(data.data));
    if(xQueueOverwrite(bleEventQueue,&event)==pdTRUE && xQueueSend(bleAdvertismentQueue,&newData,0)==pdTRUE)
      return true;
    else
      return false;
  }
  else
    return false;
}

bool send_ble_data (BLECharacteristic * characteristic, headerCommandStruct &data){
  if(bleEventQueue!=NULL && bleAdvertismentQueue!=NULL){
    bool event=true;
    ble_send_struct newData;
    newData.len=data.len;
    newData.characteristic=characteristic;
    memcpy(newData.data,data.data,sizeof(data.data));
    if(xQueueOverwrite(bleEventQueue,&event)==pdTRUE && xQueueSend(bleDataQueue,&newData,0)==pdTRUE)
      return true;
    else
      return false;
  }
  else
    return false;
}

bool send_ble_data (BLECharacteristic * characteristic, JsonDocument &doc){
  if(bleEventQueue!=NULL && bleAdvertismentQueue!=NULL){
    bool event=true;
    ble_send_struct newData;
    newData.len=serializeJson(doc,newData.data);
    newData.characteristic=characteristic;
    if(xQueueOverwrite(bleEventQueue,&event)==pdTRUE && xQueueSend(bleDataQueue,&newData,0)==pdTRUE)
      return true;
    else
      return false;
  }
  else
    return false;
}

bool send_radio_external(headerRadioStruct &radioData,uint32_t timeout){
     uint8_t* data = reinterpret_cast<uint8_t*>(&radioData);
     return send_radio_external(radioData.commandId,data,radioData.len,timeout);   
}

void file_read(JsonDocument &doc) {
  CREATE_JSON_DOCUMENT(buffer);
  if(handler.readFile(doc["data"]["path"].as<const char*>(),buffer)){
    doc["data"]=buffer.as<JsonObject>();
    doc["status"]="success";
    doc["error"]="none";
    return;
  }      
    
  doc["status"]="error";
  doc["error"]="failed to write file";

}    

void file_write(JsonDocument &doc) {
  if(handler.writeFile(doc["data"]["path"].as<const char*>(),doc)){
    doc["status"]="success";
    doc["error"]="none";
    return;
  }      
    
  doc["status"]="error";
  doc["error"]="failed to write file";
  
}    

void file_append(JsonDocument &doc) {
  if(handler.appendFile(doc["data"]["path"].as<const char*>(),doc)){
    doc["status"]="success";
    doc["error"]="none";
    return;
  }      
    
  doc["status"]="error";
  doc["error"]="failed to append file";
}    

void file_delete(JsonDocument &doc) {
  if(handler.deleteFile(doc["data"]["path"].as<const char*>())){
    doc["status"]="success";
    doc["error"]="none";
    return;
  }      
    
  doc["status"]="error";
  doc["error"]="failed to delete file";
}    

void file_list(JsonDocument &doc) {
  CREATE_JSON_DOCUMENT(buffer);
    if(handler.listFile(doc["data"]["path"].as<const char*>(),buffer)){
    doc["status"]="success";
    doc["error"]="none";
    doc.remove("data");
    doc["data"]=buffer.as<JsonArray>();
    return;
  }      
    
  doc["status"]="error";
  doc["error"]="failed to list file";
}    

void restart_device(JsonDocument &doc) {
        doc["status"]="success";
        doc["error"]="none";
        doc["data"]["description"]="Device will restart in 5 seconds";
        send_mqtt_external(doc,"recmd",1);
        vTaskDelay(5000 / portTICK_PERIOD_MS); // Wait for 5 seconds before restarting
        esp_restart(); // Restart the device
}


#if ROLE_GATEWAY

void command_radio(JsonDocument &doc) {
        CREATE_JSON_DOCUMENT(doc_buffer);
        doc_buffer = doc["data"]["command"].as<JsonObject>();
        //validate command to be executed is valid?
        //ESP_LOGD("COMMAND_RADIO", "Received command: %s", doc_buffer["data"]["command"]["command"].as<const char*>());
        serializeJsonPretty(doc_buffer,Serial);
        Serial.println("");
        if(!handler.validateFormat(doc_buffer)){
            doc["status"] = doc_buffer["status"];
            doc["error"] = doc_buffer["error"];
            if(!doc_buffer["status_code"].isNull()){
                doc["status_code"] = doc_buffer["status_code"];
            }
            //ESP_LOGD("COMMAND_RADIO", "status: error, error: invalid command format");
            return;
        }

        doc_buffer["to"] = doc["data"]["to"].as<uint32_t>();
        doc_buffer["from"] = getID32();
        doc.remove("data");
        headerRadioStruct structCommand;
        if(!handler.jsonToStructCommand(doc_buffer, structCommand)){
            doc["status"] = "error";
            doc["error"] = "failed to convert command to radio structure";
            //ESP_LOGD("COMMAND_RADIO", "status: error, error: failed to convert command to radio structure");
            return;
        }
        
        uint8_t* data = reinterpret_cast<uint8_t*>(&structCommand);
        if(!send_radio_external(structCommand.id_message, data, structCommand.len)){
            doc["status"] = "error";
            doc["error"] = "failed to send radio command";
            ///ESP_LOGD("COMMAND_RADIO", "status: error, error: failed to send radio command");
            return;
        }
        else {
            doc["status"] = "success";
            doc["error"] = "none";
            doc["description"] = "Command sent successfully, please wait for response";
            //ESP_LOGD("COMMAND_RADIO", "status: success, error: none, description: Command sent successfully, please wait for response");
            return;
        }
}

void message_from_gateway(JsonDocument &doc) {
        headerRadioStruct structCommand;
        doc["to"]=doc["data"]["to"];
        doc["from"]=getID32();
        if(!handler.jsonToStructCommand(doc, structCommand)){
            doc["status"] = "error";
            doc["error"] = "failed to convert command to radio structure";
            //ESP_LOGD("COMMAND_RADIO", "status: error, error: failed to convert command to radio structure");
            return;
        }
        //serializeJsonPretty(doc,Serial);
        //Serial.println(structCommand.to);
        //Serial.println(structCommand.from);
        uint8_t* data = reinterpret_cast<uint8_t*>(&structCommand);
        if(!send_radio_external(structCommand.id_message, data, structCommand.len)){
            doc["status"] = "error";
            doc["error"] = "failed to send radio command";
            ///ESP_LOGD("COMMAND_RADIO", "status: error, error: failed to send radio command");
            return;
        }
        else {
            doc["status"] = "success";
            doc["error"] = "none";
            doc["description"] = "Command sent successfully, please wait for response";
            //ESP_LOGD("COMMAND_RADIO", "status: success, error: none, description: Command sent successfully, please wait for response");
            return;
        }
}

#endif

  // 1. Fungsi untuk mengecek konsistensi data dan mendapatkan jumlah data
  bool checkDataConsistency(const char* dataName, FileHeader &header, uint32_t structSize, uint32_t* recordCount,bool is_backup) {
      String path = "/" + String(dataName);
      if(is_backup)
        path=path + "2.bin";
      else
        path=path + ".bin";
      
      fs::FS& fileSystem=handler.getFileSystem();

      if (!fileSystem.exists(path.c_str())) {
          ESP_LOGE("","File tidak ditemukan!");
          return false;
      }
      
      File file = fileSystem.open(path.c_str(), "rb");
      if (!file) {
          ESP_LOGE("","Gagal membuka file!");
          file.close();
          return false;
      }
      
      // Baca header
      if (file.readBytes((char*)&header, sizeof(FileHeader)) != sizeof(FileHeader)) {
          ESP_LOGE("","Gagal membaca header!");
          file.close();
          return false;
      }
      
      // Validasi nama data dan ukuran struktur
      if (strncmp(header.dataName, dataName, sizeof(header.dataName)) != 0 || 
          header.dataSize != structSize) {
          ESP_LOGE("","Struktur data tidak konsisten!");
          file.close();
          return false;
      }
      
      // Hitung jumlah record dan validasi konsistensi
      size_t fileSize = file.size();
      size_t dataSectionSize = fileSize - sizeof(FileHeader);
      
      if (dataSectionSize % structSize != 0) {
          ESP_LOGE("","Ukuran file tidak konsisten!");
          file.close();
          return false;
      }
      
      *recordCount = dataSectionSize / structSize;
      file.close();
      
      ESP_LOGI("","Jumlah data: %u",*recordCount);
      return true;
  }

  // 2. Fungsi untuk save structure data
  bool saveStructData(const char* dataName, void* structData, uint32_t structSize) {
    // Setup paths
    String mainPath = "/" + String(dataName) + ".bin";
    String backupPath = "/" + String(dataName) + "2.bin";
    
    FileHeader header;
    strncpy(header.dataName, dataName, sizeof(header.dataName));
    header.dataSize = structSize;
    
    fs::FS& fileSystem=handler.getFileSystem();

    if (!fileSystem.exists(mainPath.c_str())) {
        File file = fileSystem.open(mainPath.c_str(), "wb");
        if (!file) {
            ESP_LOGE("","Gagal membuat file baru!");
            file.close();
            return false;
        }
        
        // Tulis header
        if (file.write((uint8_t*)&header, sizeof(FileHeader)) != sizeof(FileHeader)) {
            ESP_LOGE("","Gagal menulis header!");
            file.close();
            return false;
        }
        
        // Tulis data pertama
        if (file.write((uint8_t*)structData, structSize) != structSize) {
            ESP_LOGE("","Gagal menulis data!");
            file.close();
            return false;
        }
        
        file.close();
        ESP_LOGI("","File baru dibuat dan data ditulis.");
        return true;
    }
    
    // Jika file sudah ada
    File file = fileSystem.open(mainPath.c_str(), "r+b");
    if (!file) {
        ESP_LOGE("","Gagal membuka file!");
        return false;
    }
    
    // Baca header existing
    FileHeader existingHeader;
    if (file.readBytes((char*)&existingHeader, sizeof(FileHeader)) != sizeof(FileHeader)) {
        ESP_LOGE("","Gagal membaca header!");
        file.close();
        return false;
    }
    
    // Validasi konsistensi
    if (strncmp(existingHeader.dataName, dataName, sizeof(existingHeader.dataName)) != 0 || 
        existingHeader.dataSize != structSize) {
        ESP_LOGE("","Struktur data tidak konsisten! Membuat file baru.");
        file.close();
        
        // Rotasi: hapus backup lama, rename main ke backup
        if (fileSystem.exists(backupPath.c_str())) {
            fileSystem.remove(backupPath.c_str());
        }
        fileSystem.rename(mainPath.c_str(), backupPath.c_str());
        
        // Buat file baru
        return saveStructData(dataName, structData, structSize);
    }
    
    // Validasi konsistensi ukuran file
    size_t fileSize = file.size();
    size_t dataSectionSize = fileSize - sizeof(FileHeader);
    
    if (dataSectionSize % structSize != 0) {
        ESP_LOGE("","Ukuran file tidak konsisten! Melakukan rotasi.");
        file.close();
        
        // Rotasi: hapus backup lama, rename main ke backup
        if (fileSystem.exists(backupPath.c_str())) {
            fileSystem.remove(backupPath.c_str());
        }
        fileSystem.rename(mainPath.c_str(), backupPath.c_str());
        
        // Buat file baru
        return saveStructData(dataName, structData, structSize);
    }
    
    // Cek ukuran file (maksimal 50KB)
    size_t maxDataSize = 50*1024; // 50KB untuk data saja
    if(CI_USE_PSRAM)
      maxDataSize=350*1024; // 200KB untuk data saja jika menggunakan PSRAM

    if (fileSize >= maxDataSize + sizeof(FileHeader)) {
        ESP_LOGE("","File penuh, melakukan rotasi...");
        file.close();
        
        // Rotasi: hapus backup lama, rename main ke backup
        if (fileSystem.exists(backupPath.c_str())) {
            fileSystem.remove(backupPath.c_str());
        }
        fileSystem.rename(mainPath.c_str(), backupPath.c_str());
        
        // Buat file baru
        return saveStructData(dataName, structData, structSize);
    }
    
    // Jika masih ada space, append data
    if (!file.seek(0, SeekEnd)) {
        ESP_LOGE("","Gagal seek ke akhir file!");
        file.close();
        return false;
    }
    
    if (file.write((uint8_t*)structData, structSize) != structSize) {
        ESP_LOGE("","Gagal menulis data!");
        file.close();
        return false;
    }
    
    file.close();
    ESP_LOGI("","Data berhasil ditambahkan.");
    return true;
}


// enum OperatorType { OP_EQ, OP_NEQ, OP_GT, OP_GTE, OP_LT, OP_LTE };
// // Fungsi pembanding timestamp
// bool compareTimestamp(uint32_t data, uint32_t target, OperatorType op) {
//     switch (op) {
//         case OP_EQ:  return data == target;
//         case OP_NEQ: return data != target;
//         case OP_GT:  return data >  target;
//         case OP_GTE: return data >= target;
//         case OP_LT:  return data <  target;
//         case OP_LTE: return data <= target;
//     }
//     return false;
// }

// // Fungsi generik untuk filter data binary
// void filterFileByTimestamp(const char* structName, 
//                            void* buffer, 
//                            size_t structSize, 
//                            uint32_t target, 
//                            OperatorType op) {
//     // nama file: misalnya "/MyStructName.bin"
//     String filename = "/";
//     filename += structName;
//     filename += ".bin";

//     File file = SPIFFS.open(filename, "r");
//     if (!file) {
//         Serial.println("Gagal buka file!");
//         return;
//     }

//     while (file.available() >= (int)structSize) {
//         file.readBytes((char*)buffer, structSize);

//         // Ambil 4 byte terakhir sebagai timestamp
//         uint32_t recorded_at;
//         memcpy(&recorded_at, (uint8_t*)buffer + structSize - sizeof(uint32_t), sizeof(uint32_t));

//         if (compareTimestamp(recorded_at, target, op)) {
//             // print timestamp + size
//             Serial.printf("Match ts=%u structSize=%u\n", recorded_at, structSize);
//         }
//     }

//     file.close();
// }



// 2. Fungsi untuk membaca data dari posisi tertentu
uint32_t readData(const char* dataName, void* buffer, uint32_t bufferSize, uint32_t startRecord, bool is_backup) {
    String path = "/" + String(dataName);
    if(is_backup)
      path=path + "2.bin";
    else
      path=path + ".bin";

    fs::FS& fileSystem=handler.getFileSystem();
  
    if (!fileSystem.exists(path.c_str())) {
        ESP_LOGE("","File tidak ditemukan!");
        return false;
    }
    
    File file = fileSystem.open(path.c_str(), "rb");
    if (!file) {
        ESP_LOGE("","Gagal membuka file!");
        return false;
    }
    
    // Baca header
    FileHeader header;
    if (file.readBytes((char*)&header, sizeof(FileHeader)) != sizeof(FileHeader)) {
        ESP_LOGE("","Gagal membaca header!");
        file.close();
        return false;
    }
    
    // Hitung jumlah record
    size_t fileSize = file.size();
    size_t dataSectionSize = fileSize - sizeof(FileHeader);
    uint32_t totalRecords = dataSectionSize / header.dataSize;
    
    // Validasi startRecord
    if (startRecord >= totalRecords) {
        ESP_LOGE("","Start record melebihi jumlah data yang ada!");
        file.close();
        return false;
    }
    
    // Pindah ke posisi data yang diminta
    uint32_t startPosition = sizeof(FileHeader) + (startRecord * header.dataSize);
    if (!file.seek(startPosition)) {
        ESP_LOGE("","Gagal seek ke posisi data!");
        file.close();
        return false;
    }
    
    // Hitung berapa banyak data yang bisa dibaca
    uint32_t maxRecordsToRead = (bufferSize / header.dataSize);
    uint32_t availableRecords = totalRecords - startRecord;
    uint32_t recordsToRead = min(maxRecordsToRead, availableRecords);
    
    // Baca data
    uint32_t bytesToRead = recordsToRead * header.dataSize;
    uint32_t bytesRead = file.readBytes((char*)buffer, bytesToRead);
    
    file.close();
    
    ESP_LOGI("","Berhasil membaca %u records.", bytesRead / header.dataSize);
    
    return bytesRead / header.dataSize; // Return jumlah record yang dibaca
}

// 3. Fungsi untuk mengupdate data pada posisi tertentu
bool updateStructData(const char* dataName, void* structData, uint32_t structSize, uint32_t recordPosition, bool is_backup) {
    String path = "/" + String(dataName);
    if(is_backup)
      path=path + "2.bin";
    else
      path=path + ".bin";
    
    fs::FS& fileSystem=handler.getFileSystem();
    
    if (!fileSystem.exists(path.c_str())) {
        ESP_LOGE("","File tidak ditemukan!");
        return false;
    }
    
    File file = fileSystem.open(path.c_str(), "r+b");
    if (!file) {
        ESP_LOGE("","Gagal membuka file!");
        file.close();
        return false;
    }
    
    // Baca header
    FileHeader header;
    if (file.readBytes((char*)&header, sizeof(FileHeader)) != sizeof(FileHeader)) {
        ESP_LOGE("","Gagal membaca header!");
        file.close();
        return false;
    }
    
    // Validasi ukuran struktur
    if (header.dataSize != structSize) {
        ESP_LOGE("","Ukuran struktur tidak sesuai!");
        file.close();
        return false;
    }
    
    // Hitung jumlah record
    size_t fileSize = file.size();
    size_t dataSectionSize = fileSize - sizeof(FileHeader);
    uint32_t totalRecords = dataSectionSize / header.dataSize;
    
    // Validasi recordPosition
    if (recordPosition >= totalRecords) {
        ESP_LOGE("","Posisi record melebihi jumlah data yang ada!");
        file.close();
        return false;
    }
    
    // Pindah ke posisi data yang akan diupdate
    uint32_t updatePosition = sizeof(FileHeader) + (recordPosition * header.dataSize);
    if (!file.seek(updatePosition)) {
        ESP_LOGE("","Gagal seek ke posisi data!");
        file.close();
        return false;
    }
    
    // Update data
    if (file.write((uint8_t*)structData, structSize) != structSize) {
        ESP_LOGE("","Gagal mengupdate data!");
        file.close();
        return false;
    }
    
    file.close();
    
    ESP_LOGI("","Berhasil mengupdate data pada posisi %u.", recordPosition);
    return true;
}

// 4. F
bool updateFlagData(const char* dataName, uint32_t updated_row, bool is_backup) {
    String path = "/" + String(dataName);
    if(is_backup)
      path=path + "2.bin";
    else
      path=path + ".bin";

    fs::FS& fileSystem=handler.getFileSystem();
  
    if (!fileSystem.exists(path.c_str())) {
        ESP_LOGE("","File tidak ditemukan!");
        return false;
    }
    
    File file = fileSystem.open(path.c_str(), "r+b");
    if (!file) {
        ESP_LOGE("","Gagal membuka file!");
        return false;
    }
    
    // Baca header
    FileHeader header;
    if (file.readBytes((char*)&header, sizeof(FileHeader)) != sizeof(FileHeader)) {
        ESP_LOGE("","Gagal membaca header!");
        file.close();
        return false;
    }
    header.uploaded_row=updated_row;
        // Jika masih ada space, append data
    if (!file.seek(0)) {
        ESP_LOGE("","Gagal seek ke awal file!");
        file.close();
        return false;
    }
    
    if (file.write((uint8_t*)&header, sizeof(header)) != sizeof(header)) {
        ESP_LOGE("","Gagal update header!");
        file.close();
        return false;
    }
    
    file.close();
    ESP_LOGI("","Data header diupdate.");
    return true;
}


void check_data(JsonDocument &doc){
      doc["data"]["data_name"].as<const char*>();
      uint32_t recordCount=0;
      uint32_t structSize=0;
      structSize=handler.getDataSize(doc["data"]["data_name"].as<const char*>(),COMMAND_HANDLER_TYPE_DAT);
      if(structSize==0){
        doc["status"]="error";
        doc["error"]="invalid data_name";
        return;
      }

      FileHeader header;
      if (checkDataConsistency(doc["data"]["data_name"].as<const char*>(), header, structSize, &recordCount)) {
        doc["data"]["record_count"]=recordCount;
        doc["data"]["data_size"]=header.dataSize;
        doc["data"]["uploaded_row"]=header.uploaded_row;
        doc["status"]="success";
        doc["error"]="none";
      }
      else{
        doc["status"]="error";
        doc["error"]="data not consistent or not found";
        return;
      }
}

void upload_data(JsonDocument &doc) {
  /*
  data_name
  is_backup
  send_to (mqtt,http,ble,serial,radio,espnow)
  }
  */
  
  uint8_t buffer_struct[512];
  size_t data_size=handler.getDataSize(doc["data"]["data_name"].as<const char*>(),COMMAND_HANDLER_TYPE_DAT);
  bool is_backup=doc["data"]["is_backup"].as<bool>();
  uint32_t recordCount=0;
  if (data_size==0){
      doc["status"]="error";
      doc["error"]="invalid or unsuported data";
      return;
  }

  if (data_size>512){
      doc["status"]="error";
      doc["error"]="data size too large";
      return;
  }

  FileHeader header;
  if (checkDataConsistency(doc["data"]["data_name"].as<const char*>(),header, data_size,&recordCount,is_backup)) {
        ESP_LOGI("","Data konsisten. Jumlah record: %u", recordCount);
  }

  else{
      doc["status"]="error";
      doc["error"]="data not consistent";
      return;
  }

  uint32_t index=header.uploaded_row;
  uint32_t recordReaded=index;

  uint32_t commandId=handler.getCommandId(doc["data"]["data_name"].as<const char*>(),COMMAND_HANDLER_TYPE_DAT);
      
  if(commandId==0){
      doc["status"]="error";
      doc["error"]="invalid no command id found";
      return;
  }

  while(recordReaded<recordCount){
      memset(buffer_struct,0,sizeof(buffer_struct));
      uint32_t readed_data=readData(doc["data"]["data_name"].as<const char*>(), &buffer_struct, data_size, recordReaded,is_backup);
      if(readed_data==0){
        doc["status"]="error";
        doc["error"]="failed to read data";
        return;
      }

      CREATE_JSON_DOCUMENT(docBuffer);
      
      docBuffer["part"]=(recordReaded+1)-index;
      
      uint32_t max_part=recordCount-index;

      docBuffer["max"]=max_part;
      
      if(!handler.structDataToJson(docBuffer,buffer_struct,doc["data"]["data_name"].as<const char*>())){
        doc["status"]="error";
        doc["error"]="failed to convert struct to json";
        return;
      }

      if(strcmp(doc["data"]["send_to"].as<const char*>(),"mqtt")==0){
        serializeJsonPretty(docBuffer,Serial);
        Serial.println("");
        bool status=send_mqtt_external(docBuffer,doc["data"]["data_name"].as<const char*>(),2);
        if(!status){
          doc["status"]="error";
          doc["error"]="failed to send mqtt message";
          header.uploaded_row=recordReaded;
          updateFlagData(doc["data"]["data_name"].as<const char*>(),header.uploaded_row,is_backup);
          return;
        }
      }

      else{
        doc["status"]="error";
        doc["error"]="invalid send_to value";
        return;
      }
      recordReaded+=readed_data;
      header.uploaded_row=recordReaded;
    }
    bool status=updateFlagData(doc["data"]["data_name"].as<const char*>(),header.uploaded_row,is_backup);
    doc.remove("data");
    if(!status){
        doc["status"]="error";
        doc["error"]="data uploaded but failed to update flag data";
        return;
    }
    doc["status"]="success";
    doc["error"]="none";
  }

void get_data(JsonDocument &doc) {
  /*
  data_name
  index
  limit
  is_backup
  send_to (mqtt,http,ble,serial,radio,espnow)
  }
  */
  
  uint8_t buffer_struct[512];
  size_t data_size=handler.getDataSize(doc["data"]["data_name"].as<const char*>(),COMMAND_HANDLER_TYPE_DAT);
  bool is_backup=doc["data"]["is_backup"].as<bool>();
  uint32_t index=doc["data"]["index"].as<uint32_t>();
  uint32_t limit=doc["data"]["limit"].as<uint32_t>();
  uint32_t recordCount=0;
  if (data_size==0){
      doc["status"]="error";
      doc["error"]="invalid or unsuported data";
      return;
  }

  if (data_size>512){
      doc["status"]="error";
      doc["error"]="data size too large";
      return;
  }

  FileHeader header;
  if (checkDataConsistency(doc["data"]["data_name"].as<const char*>(),header, data_size,&recordCount,is_backup)) {
        ESP_LOGI("","Data konsisten. Jumlah record: %u", recordCount);
  }

  else{
      doc["status"]="error";
      doc["error"]="data not consistent";
      return;
  }

  uint32_t recordReaded=index;

  uint32_t commandId=handler.getCommandId(doc["data"]["data_name"].as<const char*>(),COMMAND_HANDLER_TYPE_DAT);
      
  if(commandId==0){
      doc["status"]="error";
      doc["error"]="invalid no command id found";
      return;
  }

  while(recordReaded<recordCount && recordReaded<index+limit){
      memset(buffer_struct,0,sizeof(buffer_struct));
    uint32_t readed_data=readData(doc["data"]["data_name"].as<const char*>(), &buffer_struct, data_size, recordReaded,is_backup);
      if(readed_data==0){
        doc["status"]="error";
        doc["error"]="failed to read data";
        return;
      }

      CREATE_JSON_DOCUMENT(docBuffer);
      
      docBuffer["part"]=(recordReaded+1);
      docBuffer["max"]=recordCount;
      if(recordCount<index+limit)
        docBuffer["limit"]=recordCount;
      else
        docBuffer["limit"]=index+limit;
      //serializeJsonPretty(docBuffer,Serial);

      if(!handler.structDataToJson(docBuffer,buffer_struct,doc["data"]["data_name"].as<const char*>())){
        doc["status"]="error";
        doc["error"]="failed to convert struct to json";
        return;
      }

      if(strcmp(doc["data"]["send_to"].as<const char*>(),"mqtt")==0){
        serializeJsonPretty(docBuffer,Serial);
        Serial.println("");
        send_mqtt_external(docBuffer,doc["data"]["data_name"].as<const char*>(),0);
      }

      else{
        doc["status"]="error";
        doc["error"]="invalid send_to value";
        return;
      }
      recordReaded+=readed_data;
    }
    doc.remove("data");
    doc["status"]="success";
    doc["error"]="none";
  }

void factory_reset(JsonDocument &doc){
    bool state=FILE_SYSTEM.format();
    doc.remove("data");
    doc["status"]=state;
    doc["data"]["description"]="Device will restart in 5 seconds";
    send_mqtt_external(doc,"recmd",1);
    vTaskDelay(5000 / portTICK_PERIOD_MS); // Wait for 5 seconds before restarting
    esp_restart(); // Restart the device
  }

void default_command(){
  handler.registerCommand("restart",restart_device);
  handler.registerCommand("file_list",file_list);
  handler.registerCommand("file_write",file_write);
  handler.registerCommand("file_append",file_append);
  handler.registerCommand("file_delete",file_delete);
  handler.registerCommand("file_read",file_read);
  handler.registerCommand("get_data",get_data);
  handler.registerCommand("check_data",check_data);
  handler.registerCommand("upload_data",upload_data);
  handler.registerCommand("factory_reset",factory_reset);
  #if ROLE_GATEWAY
    handler.registerCommand("command_radio",command_radio);
    handler.registerCommand("message_from_gateway",message_from_gateway);
  #endif
  }