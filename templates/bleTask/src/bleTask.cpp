#include <bleTask.h>

static const char* TAGB = "BLE";
QueueHandle_t commandBleJson;
QueueHandle_t statusBleQueue;

struct DataJson
{
  uint8_t len;
  uint8_t data [515];
};

uint16_t connectionId;
uint32_t eplasedConnect;
bool deviceConnected = false;
bool oldDeviceConnected = false;
BLEServer *pServer = NULL;

BLECharacteristic * pCommand;
BLECharacteristic * pResponse;
BLECharacteristic * characteristic_notify_gps;
BLECharacteristic * characteristic_notify_fingerprint;
BLECharacteristic * characteristic_notify_vehicle;
BLECharacteristic * characteristic_notify_radio;

#define SERVICE_UUID                    (uint16_t)10000//"6E400001-B5A3-F393-E0A9-E50E24DCCA9E" // OWL BEACON UUID
#define CHARACTERISTIC_UUID_RESPONSE    (uint16_t)10001//"6E400002-B5A3-F393-E0A9-E50E24DCCA9E" // TO SENT/WRITE/EDIT ADVERTISING DATA PROPERTY WRITE/READ
#define CHARACTERISTIC_UUID_CMD         (uint16_t)10002//"6E400003-B5A3-F393-E0A9-E50E24DCCA9E" // TO SENT/WRITE/EDIT FULLDATA>>>> COMMAND FRAME (LEN|CMD|DATA IF ANY) MAX 20BYTE/CHAR> ENUM ATA SETTING.H (PROPERTY WRITE/READ) 

class ServerCallback: public BLEServerCallbacks {
 
  void onConnect(BLEServer* pServer, NimBLEConnInfo& connInfo) override{
      ESP_LOGI(TAGB,"device connected");
      bool event=true;
      xQueueOverwrite(bleEventQueue,&event);
      eplasedConnect=millis();
      deviceConnected = true;
    }

    void onDisconnect(BLEServer* pServer, NimBLEConnInfo& connInfo, int reason) override{
      ESP_LOGI(TAGB,"device disconnected");
      bool event=true;
      xQueueOverwrite(bleEventQueue,&event);
      deviceConnected = false;
    }
};

class CommandCallbackJson: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic, NimBLEConnInfo& connInfo) override{ 
    ESP_LOGD("","callback %s",pCharacteristic->getValue().c_str());
    std::string rxValue = pCharacteristic->getValue();
    if (rxValue.length() > 0) {
      DataJson data;
      data.len=rxValue.length();
      memcpy(data.data,rxValue.c_str(),data.len);
      xQueueSendFromISR(commandBleJson, &data, 0);
      bool event=true;
      xQueueOverwrite(bleEventQueue,&event);                                                  
      }
    }
};

void setting_advertisement(std::string custom_data) {
  BLEDevice::stopAdvertising();
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->setMinInterval(900);
  pAdvertising->setMaxInterval(1500);
  pAdvertising->enableScanResponse(false);
  BLEAdvertisementData oAdvertisementData = BLEAdvertisementData();
  std::string data;
  data += (char)((COMPANY_ID >> 8) & 0xFF);  // MSB dari COMPANY_ID (0x02)
  data += (char)(COMPANY_ID & 0xFF);         // LSB dari COMPANY_ID (0x9A)

  data += (char)((DEVICE_ID >> 8) & 0xFF);   // MSB dari DEVICE_ID (0x00)
  data += (char)(DEVICE_ID & 0xFF);          // LSB dari DEVICE_ID (0x11)
  data.append(custom_data);
  oAdvertisementData.setManufacturerData(data);

  if(oAdvertisementData.getPayload().size()<=31)
    pAdvertising->setAdvertisementData(oAdvertisementData);
  BLEDevice::startAdvertising();
}

void refresh_adv(ble_adv_struct &data){
  std::string custom_data;
  for(int i=0;i<data.len;i++){
    custom_data+=(char)data.data[i];
  }
  setting_advertisement(custom_data);
}

bool get_ble_status(status_ble_struct &result){
    if(statusBleQueue==NULL){
        ESP_LOGE("","No ble status queue, is ble task started?");
        return false;
    }

    if(xQueuePeek(statusBleQueue,&result,0)==pdTRUE){
        return true;
    }
    
    else{
        ESP_LOGE("","No ble Status data, is ble started?");
    }
    return false;
}

void ble_process(void * pvParameters){
  adv_ble_struct advertising_state;
  ble_adv_struct adv_data;
  status_ble_struct status_ble;
  adv_data.len=2;
  adv_data.data[0]='c';
  adv_data.data[1]='i';
  interval_struct interval;
  advertising_state.state=true;
  status_ble.advertising=true;
  xQueueOverwrite(statusBleQueue,&status_ble);
  int loop=0;
  while(!handler.processCommand("adv_ble",COMMAND_HANDLER_TYPE_SET,advertising_state)&& loop<5){
    vTaskDelay(1000/portTICK_PERIOD_MS);
    loop++;
  }
  handler.loadStruct(interval);
  uint32_t last_adv=millis();
  uint32_t last_highwater=millis();
  refresh_adv(adv_data);
  for(;;){
    bool event=false;
    xQueueReceive(bleEventQueue,&event,interval.standby/portTICK_PERIOD_MS);
    print_high_water("BLE TASK",&last_highwater);  
      if (!deviceConnected && oldDeviceConnected) {
          vTaskDelay(1000/portTICK_PERIOD_MS); // give the bluetooth stack the chance to get things ready
          ESP_LOGI(TAGB,"device disconnected");
          status_ble.connected=false;
          xQueueOverwrite(statusBleQueue,&status_ble);
          oldDeviceConnected = deviceConnected;
          if(advertising_state.state)
            refresh_adv(adv_data);
        }
  
        if (deviceConnected && !oldDeviceConnected) {
            ESP_LOGI(TAGB,"device connected");
            oldDeviceConnected = deviceConnected;
            status_ble.connected=true;
            xQueueOverwrite(statusBleQueue,&status_ble);
        }
  
        if(uxQueueMessagesWaiting(commandBleJson)>0){
          DataJson data;
          xQueueReceive(commandBleJson,&data,portMAX_DELAY);
          char response [513];
          handler.processCommand((char*)data.data,response,sizeof(response));
          pResponse->setValue((uint8_t*)response,strlen(response));
          pResponse->notify();
          ESP_LOGD(TAGB,"%s",response);
          //refresh_adv();
        }

        if(uxQueueMessagesWaiting(bleDataQueue)>0){
          ble_send_struct data;
          xQueueReceive(bleDataQueue,&data,portMAX_DELAY);
          data.characteristic->setValue((uint8_t*)data.data,data.len);
          pResponse->notify();
          //refresh_adv();
        }

        // on first standby interval advertising ble regardles of internet connection or setup ble        
        if(uxQueueMessagesWaiting(adv_bleQueue)>0  && (millis()-last_adv)>interval.standby*1000){
          bool state=false;
          xQueueReceive(adv_bleQueue,&state,0);
          int i=0;
          while (!handler.loadStruct(advertising_state) && i<5) {
            vTaskDelay(1000/portTICK_PERIOD_MS);
            i++;
          }
          status_ble.advertising=advertising_state.state;
          xQueueOverwrite(statusBleQueue,&status_ble);
          if(advertising_state.state){
            refresh_adv(adv_data);
          }
          
          else{
            ESP_LOGI(TAGB,"advertising state is false, stop advertising");
            BLEDevice::stopAdvertising();
          }      
        } 
      }
    }

bool ble_task(){
    commandBleJson=xQueueCreate(2,sizeof(DataJson));
    BLEDevice::init("");
    BLEDevice::setMTU(512);
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new ServerCallback());
    BLEService *pService = pServer->createService(SERVICE_UUID);
    pResponse = pService->createCharacteristic(
                    CHARACTERISTIC_UUID_RESPONSE,
                    NIMBLE_PROPERTY::NOTIFY  | NIMBLE_PROPERTY::READ
    );

    BLECharacteristic * pCommand = pService->createCharacteristic(
                    CHARACTERISTIC_UUID_CMD,
                    NIMBLE_PROPERTY::WRITE
    );
    
    pCommand->setCallbacks(new CommandCallbackJson());

    pService->start();
    
    //refresh_adv();
  
    bleEventQueue=xQueueCreate(1,sizeof(bool));

    bleAdvertismentQueue=xQueueCreate(2,sizeof(ble_adv_struct));

    bleDataQueue=xQueueCreate(2,sizeof(ble_send_struct));

    statusBleQueue=xQueueCreate(1,sizeof(status_ble_struct));

    if(xTaskCreatePinnedToCore(ble_process,"ble_process",10240,NULL,1,&bleTaskHandler,1)==pdPASS){
    ESP_LOGD(TAGB,"Ble process ready");
    return true;
   }
   return false;
}