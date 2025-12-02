#include <tempNow.h>
uint8_t broadcastAddress[] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
esp_now_peer_info_t peerInfo;
QueueHandle_t esp_now_queue;
QueueHandle_t esp_now_statussend_queue; 


void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
            headerRadioStruct radioData;
            uint8_t* data = reinterpret_cast<uint8_t*>(&radioData);
            memcpy(&radioData, incomingData, sizeof(radioData));
            xQueueSend(esp_now_queue, &radioData,0);
}

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  ////Serial.print("Last Packet Send Status: ");
  if (status == ESP_NOW_SEND_SUCCESS){
    ESP_LOGI("","Delivery success"); 
    bool deliveryStatus=true;
    xQueueOverwrite(esp_now_statussend_queue,&deliveryStatus);
  }
  else{
    ESP_LOGW("","Delivery fail"); 
    bool deliveryStatus=false;
    xQueueOverwrite(esp_now_statussend_queue,&deliveryStatus);
  }
}

void esp_now_task(void * pvParameters){
    bool status=false;
    WiFi.disconnect();
    vTaskDelay(1000/portTICK_PERIOD_MS);
    WiFi.mode(WIFI_STA);
    esp_now_struct setup_esp_now;
    handler.loadStruct(setup_esp_now);
    esp_wifi_set_channel(setup_esp_now.channel, WIFI_SECOND_CHAN_NONE);
    if(setup_esp_now.longrange)
        esp_wifi_set_protocol(WIFI_IF_STA,WIFI_PROTOCOL_LR);
    else
        esp_wifi_set_protocol(WIFI_IF_STA,WIFI_PROTOCOL_11B | WIFI_PROTOCOL_11G | WIFI_PROTOCOL_11N);    
    
    esp_now_init() == ESP_OK?status=true:status=false;
    
    if(!status){
        ESP_LOGE("","esp now init failed");
        vTaskDelete(NULL);
        return;
    }

    memcpy(peerInfo.peer_addr, broadcastAddress, 6);
    peerInfo.channel = 0;  
    peerInfo.encrypt = false;   
    if (esp_now_add_peer(&peerInfo) != ESP_OK){
        ESP_LOGE("","Failed to add peer");
        vTaskDelete(NULL);
        return;
    }

    esp_now_register_recv_cb(OnDataRecv);
    esp_now_register_send_cb(OnDataSent);
    
    ESP_LOGI("","esp now init, mac %s, channel %d, status %d\n",WiFi.macAddress(),WiFi.channel(),status);

    for(;;){
      headerRadioStruct message;
      if(xQueueReceive(esp_now_queue,&message,portMAX_DELAY)){
        if(message.to == 0 || message.to == getID32()){
            ESP_LOGI("","Received message with ID: %d", message.id_message);
            
#if ESP_NOW_ROLE_GATEWAY
            
            if(message.to==0){
                message.to = getID32();
            }

            CREATE_JSON_DOCUMENT(doc);
            if(handler.structCommandToJson(doc, message)){
                ESP_LOGI("","SUCCESS: Converted struct esp now to JSON");
                // Send the JSON document to MQTT
                if(message.type!= COMMAND_HANDLER_TYPE_DAT_WITHOUT_ACK){
                    bool status=send_mqtt_external(doc, doc["command"].as<const char*>(), 1);
                    //send acknowledgment to esp now
                    radio_response_data_struct dataResponse;
                    if(status){
                        dataResponse.status=true;
                        dataResponse.status_code=PACKAGE_TRANSMITTED_TO_GATEWAY_SUCSESS_UPLOAD;
                    }
                    else{
                        dataResponse.status=false;
                        dataResponse.status_code=PACKAGE_TRANSMITTED_TO_GATEWAY_FAILED_UPLOAD;
                    }
                    headerRadioStruct responseRadio;
                    responseRadio = handler.dataToStructCommand(message.from, "radio_response_data", COMMAND_HANDLER_TYPE_DAT_WITHOUT_ACK, dataResponse, message.id_message, getID32());
                    uint8_t* data = reinterpret_cast<uint8_t*>(&responseRadio);
                    if(!send_esp_now_external(responseRadio, 2000)){
                        ESP_LOGE("","Failed to send esp now external");
                    }
                    else{
                        ESP_LOGI("","Successfully sent esp now external");
                    }
                }
                else{
                    //do not send acknowledgment
                    send_mqtt_external(doc, doc["command"].as<const char*>(), 1);
                }
            }
            else{
                ESP_LOGE("","FAILED: Could not convert struct esp now to JSON");
            }
            
            #else
            CREATE_JSON_DOCUMENT(doc);
            bool status=handler.processCommand(message,&doc);
                if(status){
                    ESP_LOGI("","Command processed successfully");
                    radio_response_data_struct dataResponse;
                    dataResponse.status=true;
                    dataResponse.status_code=PACKAGE_TRANSMITTED_SUCSESS_EXECUTION;
                    headerRadioStruct responseRadio;
                    responseRadio = handler.dataToStructCommand(message.from, "radio_response_data", COMMAND_HANDLER_TYPE_DAT_WITHOUT_ACK, dataResponse, message.id_message);
                    if(message.type!= COMMAND_HANDLER_TYPE_DAT_WITHOUT_ACK)
                        send_esp_now_external(responseRadio);
                }
                else{
                    ESP_LOGE("","Failed to process command");
                    radio_response_data_struct dataResponse;
                    dataResponse.status=true;
                    dataResponse.status_code=PACKAGE_TRANSMITTED_FAILED_EXECUTION;
                    headerRadioStruct responseRadio;
                    responseRadio = handler.dataToStructCommand(message.from, "radio_response_data", COMMAND_HANDLER_TYPE_DAT_WITHOUT_ACK, dataResponse, message.id_message);
                    if(message.type!= COMMAND_HANDLER_TYPE_DAT_WITHOUT_ACK)
                        send_esp_now_external(responseRadio);
                }

            #endif
            // Handle the message as needed
            // You can add further processing here
        }
        else{
            ESP_LOGW("","Message not for me, ignoring");
        }
      }
      vTaskDelay(100/portTICK_PERIOD_MS);
    }
}

bool send_esp_now_external(headerRadioStruct &radioData,uint32_t timeout){
    if(esp_now_queue==NULL){
        ESP_LOGE("","No esp now queue, is esp now task started?");
        return false;
    }

    //send to esp now
    bool deliveryStatus=false;
    if(esp_now_send(broadcastAddress, (uint8_t *) &radioData, radioData.len) == ESP_OK){
        //wait for delivery status
        if(xQueueReceive(esp_now_statussend_queue,&deliveryStatus,timeout/portTICK_PERIOD_MS)){
            if(deliveryStatus){
                ESP_LOGI("","esp now send success");
                return true;
            }
            else{
                ESP_LOGW("","esp now send failed");
                return false;
            }
        }
        else{
            ESP_LOGE("","esp now send timeout");
            return false;
        }
    }
    else{
        ESP_LOGE("","esp now send failed");
        return false;
    }
    return false; // Should not reach here, but just in case
}

bool tempnow_task(){
    
    esp_now_queue=xQueueCreate(2, sizeof(headerRadioStruct));
    esp_now_statussend_queue=xQueueCreate(1, sizeof(bool));
    if(xTaskCreatePinnedToCore(esp_now_task,"esp_now_process",10000,NULL,1,NULL,1)==pdPASS){
        ESP_LOGI("","esp_now_ready");
    }
    return true;
   }