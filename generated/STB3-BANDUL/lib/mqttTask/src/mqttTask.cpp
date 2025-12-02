#include <mqttTask.h>
#include <esp_wifi.h>
static const char* TAGM = "mqttTask.h";
QueueHandle_t mqttMessageQueue;
SemaphoreHandle_t mutex_mqtt;
QueueHandle_t mqttToBeSend;
QueueHandle_t mqttSendStatus;

struct mqttMessage
{
  char topic[120];
  char message[3000];
};

QueueHandle_t statusMqttQueue;

void messageReceived(MQTTClient *client, char topic[], char bytes[], int length) {
    if(length>0){
    mqttMessage message;
    strlcpy(message.message,bytes,sizeof(message.message));
    strlcpy(message.topic,topic,sizeof(message.topic));
    xQueueSend(mqttMessageQueue,&message,0);
    }
  }

void generate_topic(char * topic, const char * description){
     sprintf(topic,"%s/%s/%u/%s",PRODUCT_TYPE,PRODUCT_NAME,getID32(),description);
}

bool send_mqtt_external(JsonDocument &doc,const char * description, int qos, int timeout){
  if(mutex_mqtt!=NULL && mqttMessageQueue!=NULL && mqttToBeSend!=NULL && mqttSendStatus!=NULL){    
    if(xSemaphoreTake(mutex_mqtt,100/portTICK_PERIOD_MS)==pdTRUE){
      mqtt_send_struct data;
      serializeJson(doc,data.payload);
      generate_topic(data.topic,description);
      data.qos=qos;      
      bool state=false;
      if(xQueueSend(mqttToBeSend,&data,100/portTICK_PERIOD_MS)==pdTRUE){
        xQueueReceive(mqttSendStatus,&state,timeout/portTICK_PERIOD_MS);
      }
      else
        state=false;
      xSemaphoreGive(mutex_mqtt);
      return state;
    }
    else
      return false;
}
return false;
}

bool send_mqtt(MQTTClient &client, JsonDocument &doc,const char * description, int qos,bool retained=false){   
        client.loop();
        char topic [120];
        generate_topic(topic,description);
        char buffer [3000];
        size_t size=serializeJson(doc,buffer);
        bool state=client.publish(topic,buffer,size,retained,qos);
        
        return state;
}

void connect_mqtt(MQTTClient &client, WiFiClient &net, const mqtt_struct mqttparam){
        //client.disconnect();
        char clientId[20];
        sprintf(clientId,"%u",getID32());
        client.connect(clientId,mqttparam.username,mqttparam.password);
        if(client.connected()){
            char topic[120];//command
            generate_topic(topic,"cmd");
            ESP_LOGI(TAGM,"connecting to mqtt %s\n",topic);
            if(client.subscribe(topic,1))
              ESP_LOGI(TAGM,"subsribe success\n");
            else
              ESP_LOGW(TAGM,"subscribe failed\n");
        }
        else  {
              ESP_LOGW(TAGM,"failed connect to mqtt\n");
        }
}

void begin_mqtt(MQTTClient &client, WiFiClient &net, mqtt_struct mqttparam){
    client.begin(mqttparam.host,mqttparam.port,net);
    client.setCleanSession(false);
    client.onMessageAdvanced(messageReceived);
    char topic [120];
    generate_topic(topic,"status");
    CREATE_JSON_DOCUMENT(doc);
    doc["notif_type"]="connection";
    doc["description"]="disconnect";
    doc["state"]=0;
    char payload [250];
    serializeJson(doc,payload);
    client.setWill(topic,payload,true,1);
    connect_mqtt(client,net,mqttparam);
}

#define BUILD_DATETIME __DATE__ " " __TIME__
#ifndef BUILD_USER
  #ifdef _WIN32
    #define BUILD_USER getenv("USERNAME")
  #else
    #define BUILD_USER getenv("riki")
  #endif
#endif

void send_status(MQTTClient &client){
        client.loop();
        CREATE_JSON_DOCUMENT(doc);
        use_key_struct use_key;
        handler.loadStruct(use_key);


        doc["device"]["hardware"]=HARDWARE_VER;
        doc["device"]["firmware"]=SOFTWARE_VER;
        doc["device"]["ci_core"]=CO_INVENTOR_CORE;
        doc["device"]["build"]=BUILD_DATETIME;
        doc["device"]["secure"]=use_key.state;
        doc["device"]["last_online"]=getDateTime().timestamp();

        doc["cpu"]["free_ram"]=ESP.getFreeHeap()/1024;
        doc["cpu"]["min_ram"]=ESP.getMinFreeHeap()/1024;
        doc["cpu"]["free_psram"]=ESP.getFreePsram()/1024;
        doc["cpu"]["min_psram"]=ESP.getMinFreePsram()/1024;
        doc["cpu"]["temp"]=get_rtc_temp();
        
        doc["disk"]["sytem"]=GET_FS_TYPE;
        doc["disk"]["total"]=GET_TOTAL_SPACE;
        doc["disk"]["used"]=GET_USED_SPACE;

        doc["wifi"]["ssid"]=WiFi.SSID();
        doc["wifi"]["ip"]=WiFi.localIP().toString();
        doc["wifi"]["rssi"]=WiFi.RSSI();
        
        power_status_struct power=getPowerStatus();
        doc["power"]["bat"]=power.bat;
        doc["power"]["v_bat"]=power.vBat;
        doc["power"]["v_charge"]=power.vCharge;
        doc["power"]["is_charge"]=power.isCharge;
        
        //serializeJson(doc,Serial);
        send_mqtt(client, doc,"heartbeat",0,true);
}

bool get_mqtt_status(status_mqtt_struct &result){
    if(statusMqttQueue==NULL){
        ESP_LOGE("","No mqtt status queue, is mqtt task started?");
        return false;
    }

    if(xQueuePeek(statusMqttQueue,&result,0)==pdTRUE){
        return true;
    }
    
    else{
        ESP_LOGE("","No mqtt status data, is mqtt task started?");
    }
    return false;
}

void mqtt_process(void * pvParameters){
    WiFiClient net;
    MQTTClient client (5024);
    status_mqtt_struct status_mqtt;
    status_mqtt.mqtt_connected=false;
    status_mqtt.mqtt_reconnect=0;
    status_mqtt.wifi_connected=false;
    status_mqtt.wifi_reconnect=0;
    bool wifiReconnect=false;
    bool notifReconnect=false;
    uint32_t tryMqttReconnect=0;
    //---------------
    interval_struct interval;
    handler.loadStruct(interval);
    ESP_LOGD(TAGM,"standby:%d,upload:%d,pooling:%d\n",interval.standby,interval.upload_data,interval.get_data);

    wifi_struct wifiparam;
    handler.loadStruct(wifiparam);
    ESP_LOGD(TAGM,"ssid:%s,password:%s\n",wifiparam.ssid, wifiparam.password);  
    const uint8_t protocol = WIFI_PROTOCOL_11B | WIFI_PROTOCOL_11G || WIFI_PROTOCOL_11N;
    esp_wifi_set_protocol(WIFI_IF_STA,protocol);  
    WiFi.mode(WIFI_STA);
    char hostname[50];
    sprintf(hostname,"%s-%u",HOSTNAME,getID32());
    WiFi.setHostname(hostname);
    WiFi.begin(wifiparam.ssid,wifiparam.password);

    mqtt_struct mqttparam;
    handler.loadStruct(mqttparam);
    ESP_LOGD(TAGM,"host:%s,user:%s,password:%s,port:%d\n",mqttparam.host,mqttparam.username,mqttparam.password,mqttparam.port);
    //----------------
    client.dropOverflow(true);
    begin_mqtt(client,net,mqttparam);

    uint32_t lastUpload=0;

    bool wifiLastState=false;
    uint32_t lastConnectWifi=millis();

    bool  mqttLastState=false;
    uint32_t lastConnectMqtt=millis();
    bool firstTime=true;
    uint32_t last_send_highwater=millis();
    bool isNotifConnectedSend=false;
    for(;;){
      print_high_water("MQTT TASK",&last_send_highwater);
      
      //If MQTT Connected
      if(client.connected()){
        
        //Do something (LED Indicator or LCD)
        lastConnectMqtt=millis();
        if(firstTime||!mqttLastState){//Just reconnected
          //Do something send notif
          status_mqtt.mqtt_connected=true;
          status_mqtt.mqtt_reconnect=0;
          xQueueOverwrite(statusMqttQueue,&status_mqtt);
          ESP_LOGI(TAGM,"mqtt reconnected");
          tryMqttReconnect=0;
          firstTime=false;
          mqttLastState=true;
          CREATE_JSON_DOCUMENT(doc);
          doc["notif_type"]="connection";
          doc["description"]="connected";
          doc["state"]=1;
          isNotifConnectedSend=send_mqtt(client, doc,"status",2,true);
          
          //Connected to internet turn off bluetooth advertising
          adv_ble_struct advertising_state;
          advertising_state.state=false;
          int i=0;
          while (!handler.processCommand("adv_ble",COMMAND_HANDLER_TYPE_SET,advertising_state,getID16()) && i<5) {
            vTaskDelay(10/portTICK_PERIOD_MS);
            i++;
          }
        }        
      
        if(!isNotifConnectedSend){//gagal kirim notif connected resend sampai berhasil
          CREATE_JSON_DOCUMENT(doc);
          doc["notif_type"]="connection";
          doc["description"]="connected";
          doc["state"]=1;
          isNotifConnectedSend=send_mqtt(client, doc,"status",2,true);
        }
      }

      //If MQTT Disconnected
      else{
        status_mqtt.mqtt_connected=false;
        status_mqtt.mqtt_reconnect=((millis()-lastConnectMqtt)/1000)-interval.mqttreconnect;
        xQueueOverwrite(statusMqttQueue,&status_mqtt);
        //Do something (LED Indicator or LCD)
        if(mqttLastState){
          ESP_LOGW(TAGM,"mqtt disconnected");
          mqttLastState=false;

          //turn on bluetooth advertising
          adv_ble_struct advertising_state;
          advertising_state.state=true;

          int i=0;
          while (!handler.processCommand("adv_ble",COMMAND_HANDLER_TYPE_SET,advertising_state,getID16()) && i<5) {
            vTaskDelay(10/portTICK_PERIOD_MS);
            i++;
          }  
        }
        
        if(//Wifi Connected and mqtt has disconnected too long trying to reconnect
          (WiFi.status()==WL_CONNECTED)&&
          (((millis()-lastConnectMqtt)/1000)>interval.mqttreconnect)
          )
        {
          lastConnectMqtt=millis();
          ESP_LOGI(TAGM,"trying reconnect mqtt");
          tryMqttReconnect++;
          if(tryMqttReconnect%10==10){
            WiFi.disconnect();
          }
          connect_mqtt(client,net,mqttparam);
        }
      }

      //If Wifi Connected
      if(WiFi.status()==WL_CONNECTED){
        //Do something (LED Indicator or What)
        
        lastConnectWifi=millis();
        if(!wifiLastState){
          status_mqtt.wifi_connected=true;
          status_mqtt.wifi_reconnect=0;
          xQueueOverwrite(statusMqttQueue,&status_mqtt);
          wifiLastState=true;
          ESP_LOGD(TAGM,"Wifi Reconnected");
          ESP_LOGD(TAGM,"trying Reconnect to MQTT");
          connect_mqtt(client,net,mqttparam);//reconnect to mqtt
        }
      }

      //If Wifi Disconnected
      else{
        //Do Something (LED Indicator or LCD or What)
        wifiLastState=false; 
        status_mqtt.wifi_connected=false;
        status_mqtt.wifi_reconnect=((millis()-lastConnectWifi)/1000)-interval.wifireconnect;
        xQueueOverwrite(statusMqttQueue,&status_mqtt);

        if(millis()-lastConnectWifi>(interval.wifireconnect*1000)){
          lastConnectWifi=millis();
          esp_wifi_set_protocol(WIFI_IF_STA,protocol);  
          WiFi.begin(wifiparam.ssid,wifiparam.password);   
          ESP_LOGI(TAGM,"wifi connection lost reconnecting"); 
        }
      }


//-------------------------EVENT HANDLER WHEN NEW SETUP------------------------
      if(uxQueueMessagesWaiting(mqttQueue)>0){//there is update to mqtt param
        bool newValues=false;
        xQueueReceive(mqttQueue,&newValues,0);
        handler.loadStruct(mqttparam);
        client.disconnect();
        begin_mqtt(client,net,mqttparam);
        connect_mqtt(client,net,mqttparam);
      }

      if(uxQueueMessagesWaiting(intervalQueue)>0){//there is update to interval param
        bool newValues=false;
        xQueueReceive(intervalQueue,&newValues,0);
        handler.loadStruct(interval);
      }

      if(uxQueueMessagesWaiting(wifiQueue)>0){//there is update to wifi param
        bool newValues=false;
        xQueueReceive(wifiQueue,&newValues,0);
        //ESP_LOGI(TAG,"---------Load New Wifi Param----%s--\n",wifiparam.ssid);
        handler.loadStruct(wifiparam);
        esp_wifi_set_protocol(WIFI_IF_STA,protocol); 
        WiFi.begin(wifiparam.ssid,wifiparam.password);
      }
//------------------------------------------------END OF EVENT HANDLER-----------------------

      if(client.connected()&&((millis()-lastUpload)/1000)>=interval.heartbeat){//Time for upload/update status
        lastUpload=millis();
        send_status(client);
      }
      
      if(uxQueueMessagesWaiting(mqttToBeSend)>0){//there is data to uploaded to mqtt
        ESP_LOGI("","There is call to send mqtt data");
        mqtt_send_struct data;
        if(xQueueReceive(mqttToBeSend,&data,0)==pdTRUE){
          bool state=false;
          if(WiFi.status()!=WL_CONNECTED){
            esp_wifi_set_protocol(WIFI_IF_STA,protocol); 
            WiFi.begin(wifiparam.ssid,wifiparam.password);
            vTaskDelay(1000/portTICK_PERIOD_MS);
            connect_mqtt(client,net,mqttparam);
          }
          if(!client.connected())
            connect_mqtt(client,net,mqttparam);
            
          state=client.publish(data.topic,data.payload,strlen(data.payload),false,data.qos);
          xQueueOverwrite(mqttSendStatus,&state);
        }
      }
      client.loop();
      //Serial.println(".");
      vTaskDelay(10/portTICK_PERIOD_MS);
    }
}

void mqtt_command(void * pvParameters){
    for(;;){
      mqttMessage message;
      statusMqttQueue=xQueueCreate(1,sizeof(status_mqtt_struct));
      if(xQueueReceive(mqttMessageQueue,&message,portMAX_DELAY)){
        CREATE_JSON_DOCUMENT(doc);
        handler.processCommand(message.message,doc);  
        //ESP_LOGD("","Received command %s from topic %s",message.message,message.topic);
        send_mqtt_external(doc,"recmd",0);
      }
      vTaskDelay(100/portTICK_PERIOD_MS);
    }
}

bool mqtt_task(){
    mqttMessageQueue=xQueueCreate(2,sizeof(mqttMessage));
    mutex_mqtt=xSemaphoreCreateMutex();
    mqttToBeSend=xQueueCreate(1,sizeof(mqtt_send_struct));
    mqttSendStatus=xQueueCreate(1,sizeof(uint32_t));
    if(xTaskCreatePinnedToCore(mqtt_process,"mqtt_process",16000,NULL,1,&mqttTaskHandler,1)==pdPASS && xTaskCreatePinnedToCore(mqtt_command,"mqtt_command",10000,NULL,1,NULL,1)){
    ESP_LOGD(TAGM,"mqtt process ready");
    return true;
   }

   return false;
}
