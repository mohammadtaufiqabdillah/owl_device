#include <radioTask.h>

#if CONFIG_IDF_TARGET_ESP32
    #define E22_RXEN_PIN 25
    #define E22_DIO1_PIN 2
    #define E22_BUSY_PIN 15
    #define E22_RST_PIN 4
    #define E22_CS_PIN 5    
#endif

#if CONFIG_IDF_TARGET_ESP32S3
    #define E22_RXEN_PIN 17
    #define E22_DIO1_PIN 48
    #define E22_BUSY_PIN 47
    #define E22_RST_PIN 38
    #define E22_CS_PIN 10
#endif

QueueHandle_t radioMessageQueue;
SemaphoreHandle_t mutex_radio;
QueueHandle_t flagQueue;
QueueHandle_t RxQueue; //for interupt
QueueHandle_t TxQueue; //for transmit task 2 that non blocking
QueueHandle_t TxAcknowledgeQueue; //for package status query (wethear the package has been acknowledge or not) max last 100 message
QueueHandle_t radioStatusQueue; //for package status query (wethear the package has been acknowledge or not) max last 100 message
QueueHandle_t numberQueue; //for number of message in queue

volatile bool receivedFlag = false;
bool receiveState=false;

SX1262 radio = new Module(E22_CS_PIN,E22_DIO1_PIN,E22_RST_PIN,E22_BUSY_PIN);
radio_mode_struct radioModeParam;
radio_param_1_struct radioChannel[MAX_CHANNEL_RADIO];
RadioAckBuffer ackBuffer(MAX_PACKAGE_STATUS_HOLD); 

struct radio_package_struct
{
    uint8_t data[255];
    uint8_t len;
    uint32_t message_id;
};

bool get_radio_status(radio_status_struct &result){
    if(radioStatusQueue==NULL){
        ESP_LOGE("","No Radio Status queue, is radio task started?");
        return false;
    }
    if(xQueuePeek(radioStatusQueue,&result,0)==pdTRUE){
        return true;
    }
        else{
            ESP_LOGE("","No Radio Status data, is radio task started?");
    }
    
    return false;
}

bool get_radio_acknowledge_status (const uint32_t message_id, radio_acknowledge_struct &data){//Ring buffer check for message_id if find return true & return the struct
    return ackBuffer.get(message_id, data);
}

bool get_radio_acknowledge_status (JsonDocument &doc){//Ring buffer check for message_id if find return true & return the struct
     return ackBuffer.get(doc);
}

bool update_radio_acknowledge_status (const uint32_t message_id, radio_acknowledge_struct &update, bool insert=false){//Ring buffer check for message_id if find return true & return the struct
    if(TxAcknowledgeQueue==NULL){
        ESP_LOGE("","No Tx Acknowledge queue, is radio task started?");
        return false;
    }
    int number_of_message=uxQueueMessagesWaiting(TxAcknowledgeQueue);
    radio_acknowledge_struct buffer[number_of_message];
    ESP_LOGI("","create queue acknowledge buffer %d",number_of_message);
    int len=0;
    bool getQueueStatus=false;
    bool match=false;
    do
    {
        if(xQueueReceive(TxAcknowledgeQueue,&buffer[len],5/portTICK_PERIOD_MS)==pdTRUE){
            //ESP_LOGI("","message id %u, message id source %u, indext %d",buffer[len].message_id,message_id,len);
            getQueueStatus=true;
            len++;
        }
        else
            getQueueStatus=false;
        /* code */
        if(buffer[len-1].message_id==message_id){
            buffer[len-1].rssi=update.rssi;
            buffer[len-1].status_send=update.status_send;
            buffer[len-1].message_type=update.message_type;
            buffer[len-1].snr=update.snr;
            ESP_LOGI("","match indext ------------------------------------------------------------- %d",len);
            match=true;
            break;
        }
    } while (getQueueStatus);

    while(len>0){//kembalikan ke queue asli
        xQueueSendToFront(TxAcknowledgeQueue,&buffer[len-1],0);
        len--;
    }
    return match;
}

bool send_radio_external(uint32_t message_id,const uint8_t *array,int len,uint32_t timeout){
    if(TxQueue==NULL){
        ESP_LOGE("","No Tx Queue, is radio task started?");
        return false;
    }

    if(len>255){
        ESP_LOGE("","Cannot transmit data len >255");
        return false;
    }

    radio_package_struct newData;
    newData.len=len;
    newData.message_id=message_id;
    memcpy(newData.data,array,len);

    if(xQueueSend(TxQueue,&newData,timeout/portTICK_PERIOD_MS)==pdTRUE)    
        return true;
    else{
        ESP_LOGE("","Cannot send to queue transmit queue full?");
        return false;
    }
    return true;
    //vTaskDelay(timeout/portTICK_PERIOD_MS);
  }

void setFlag(void) {
    // we got a packet, set the flag
    if(receiveState){
      receivedFlag = true;
      bool flags=true;
      xQueueSendFromISR(flagQueue, &flags, NULL );
    }
  }

template <typename T1, typename T2>
void copy_radio_params(T1& dest, const T2& src) {
        dest.bandwidth = src.bandwidth;
        dest.tx = src.tx;
        dest.rx = src.rx;
        dest.cr = src.cr;
        dest.sf = src.sf;
        dest.db = src.db;
        dest.bitrate = src.bitrate;
    }
  
void load_all_channel_setting(radio_param_1_struct (&channel)[MAX_CHANNEL_RADIO]) {
        radio_param_1_struct channel1;
        radio_param_2_struct channel2;
        radio_param_3_struct channel3;
        radio_param_4_struct channel4;    
        handler.loadStruct(channel1);
        handler.loadStruct(channel2);
        handler.loadStruct(channel3);
        handler.loadStruct(channel4);
        copy_radio_params(channel[0],channel1);
        copy_radio_params(channel[1],channel2);
        copy_radio_params(channel[2],channel3);
        copy_radio_params(channel[3],channel4);
    }

void listenMode(float freq){
        radio.setFrequency(freq);
        receiveState=true;
        int state = radio.startReceive();
        if (state == RADIOLIB_ERR_NONE) {  
        } 
        else {
            ESP_LOGE("","RADIO FAIL TO SET FREQ IN LISTEN MODE");
        }
      }

void setup_radio(radio_param_1_struct (&radioChannel)[MAX_CHANNEL_RADIO],uint8_t channel_num){
    int state = radio.begin(radioChannel[channel_num].rx);
    if (state == RADIOLIB_ERR_NONE) {
        ESP_LOGI("","RADIO HAS BEEN STARTED SUCSESSFULL");
    } 
    else {
        ESP_LOGE("","RADIO FAIL TO START ERROR IN HARDWARE?");
        vTaskDelete(NULL);
    }
 
    if (radio.setDio2AsRfSwitch() != RADIOLIB_ERR_NONE) {
        ESP_LOGE("","RADIO FAIL TO SET DIO2 AS RF SWITCH?");
        vTaskDelete(NULL);
    }

    if (radio.setRxBoostedGainMode(true)!= RADIOLIB_ERR_NONE) {
        ESP_LOGE("","RADIO FAIL TO BOOSTED GAIN MODE");
        vTaskDelete(NULL);
    }

    if (radio.setCurrentLimit(140) == RADIOLIB_ERR_INVALID_CURRENT_LIMIT) {
        ESP_LOGE("","RADIO FAIL TO SET CURRENT LIMIT");
        vTaskDelete(NULL);
    }

    radio.setRfSwitchPins(E22_RXEN_PIN, RADIOLIB_NC);
    
    radio.setDio1Action(setFlag);


    if (radio.setOutputPower(radioChannel[channel_num].db) == RADIOLIB_ERR_INVALID_OUTPUT_POWER) {
        ESP_LOGE("","RADIO FAIL TO SET DB");
        vTaskDelete(NULL);
    }
    
    if (radio.setCodingRate(radioChannel[channel_num].cr) == RADIOLIB_ERR_INVALID_CODING_RATE) {
        ESP_LOGE("","RADIO FAIL TO SET CR");
        vTaskDelete(NULL);
    }
    
    if (radio.setSpreadingFactor(radioChannel[channel_num].sf) == RADIOLIB_ERR_INVALID_SPREADING_FACTOR) {
        ESP_LOGE("","RADIO FAIL TO SET spread factor");
        vTaskDelete(NULL);
    }

    if (radio.setBandwidth(radioChannel[channel_num].bandwidth) == RADIOLIB_ERR_INVALID_BANDWIDTH) {
        ESP_LOGE("","RADIO FAIL TO SET bandwidth");
        vTaskDelete(NULL);
    }

    if (radio.setBitRate(radioChannel[channel_num].bitrate) == RADIOLIB_ERR_INVALID_BIT_RANGE) {
        ESP_LOGE("","RADIO FAIL TO SET bitrate");
        //vTaskDelete(NULL);
    }

    
}

void radio_isr( void * pvParameters ){
    bool flag;
    uint32_t last_send_highwater=millis(); 
    while(true){
        print_high_water("RECEIVE RADIO ISR",&last_send_highwater);
        if(xQueueReceive(flagQueue, &flag, portMAX_DELAY)==pdTRUE){
            headerRadioStruct radioData;
            uint8_t* data = reinterpret_cast<uint8_t*>(&radioData);
            int state = radio.readData(data,radio.getPacketLength());
            if (state == RADIOLIB_ERR_NONE) {
              xQueueSend(RxQueue, &radioData, portMAX_DELAY);
            } 
          }
    }
  }
  
bool create_radio_isr_task(){
    if(xTaskCreatePinnedToCore(radio_isr,"risr_task",3548,NULL,3/*priority*/,NULL,1/*core*/)==pdPASS)
        return true;
    else
      return false;
  }

void transmit_func( void * pvParameters ){
    radio_package_struct msgPack;
    uint32_t last_send_highwater=millis();
    while(true){
        print_high_water("TRANSMIT RADIO TASK",&last_send_highwater);
        //format sama dengan message_frame 
      if(xQueueReceive(TxQueue, &msgPack, portMAX_DELAY)){
        //Serial.print("get queue delay before send ");
        //vTaskDelay((sqrt(esp_random())/(90))/portTICK_PERIOD_MS);
        ESP_LOGD("","TRANSMIT RADIO TASK ----------- %u",msgPack.message_id);
        radio.setFrequency(radioChannel[radioModeParam.channel].tx);
        receiveState=false;
        int state=-99;

//     struct headerRadioStruct {
//     uint32_t to;
//     uint32_t from;
//     uint16_t id_message;
//     uint8_t type;
//     uint16_t key;
//     uint16_t commandId;
//     uint8_t data [512];
//     uint16_t len; 
//   };
  
        if(!radioModeParam.is_fsk){//bukan FSK 
            state = radio.scanChannel();
            while (state != RADIOLIB_CHANNEL_FREE){
                listenMode(radioChannel[radioModeParam.channel].rx);
                receiveState=true;
                randomSeed(esp_random());
                uint32_t random_numer=random(100,radioModeParam.randomness);
                ESP_LOGD("","random waiting %u",random_numer);
                vTaskDelay(random_numer/portTICK_PERIOD_MS);
                receiveState=false;
                radio.setFrequency(radioChannel[radioModeParam.channel].tx);
                state = radio.scanChannel();
            };
        }
        state = radio.transmit(msgPack.data,msgPack.len);
        if (state == RADIOLIB_ERR_NONE) {
          ESP_LOGD("","TRANSMIT DONE");
        } 
        else if (state == RADIOLIB_ERR_PACKET_TOO_LONG) {
          ESP_LOGW("","PACKET TOO LONG");
        } 
        else if (state == RADIOLIB_ERR_TX_TIMEOUT) {
          ESP_LOGW("","RADIOLIB TIMEOUT");
        } 
        else {
          ESP_LOGW("","TRANSMIT FAILD??");
        }

        receiveState=true;
        radio.finishTransmit(); 
        listenMode(radioChannel[radioModeParam.channel].rx);     
    
        radio_acknowledge_struct status;
        status.message_id=msgPack.message_id;
        bool is_ack=ackBuffer.get(msgPack.message_id,status);
        status.timeonair=radio.getTimeOnAir(sizeof(msgPack))/1000;
        status.message_type=msgPack.data[10]; //data ke 11
        
        if(status.message_type!=COMMAND_HANDLER_TYPE_DAT_WITHOUT_ACK&&!is_ack){
            status.status_send=PACKAGE_TRANSMITTED;
            ackBuffer.insert(status);
        }
        else if(status.message_type!=COMMAND_HANDLER_TYPE_DAT_WITHOUT_ACK&&is_ack){
            ackBuffer.update(msgPack.message_id,status);
        }
      }
    }
  }
  
bool createTransmitTask(){
    if(xTaskCreatePinnedToCore(transmit_func,"tx_task",3548,NULL,1/*priority*/,NULL,1/*core*/)==pdPASS)
        return true;
    else
      return false;
  }
  

void radio_process(void * pvParameters){
    load_all_channel_setting(radioChannel);       
    setup_radio(radioChannel,radioModeParam.channel);

    TxAcknowledgeQueue=xQueueCreate(MAX_PACKAGE_STATUS_HOLD,sizeof(radio_acknowledge_struct)); 
    radioStatusQueue=xQueueCreate(1,sizeof(radio_status_struct));
    TxQueue=xQueueCreate(10,sizeof(radio_package_struct));
    flagQueue=xQueueCreate(1,sizeof(bool));
    RxQueue=xQueueCreate(10,sizeof(headerRadioStruct));
    interval_struct interval;
    handler.loadStruct(interval);
    if(create_radio_isr_task())
        ESP_LOGI("","RADIO ISR INITIALIZED");
    else{
        ESP_LOGE("","RADIO ISR FAILD");
        vTaskDelete(NULL);
    }
    if(createTransmitTask())
        ESP_LOGI("","RADIO TRANSMIT INITIALIZED");
    else{
        ESP_LOGE("","RADIO TRANSMIT FAILD");    
        vTaskDelete(NULL);
    }
    headerRadioStruct dataReceived;
    uint32_t last_send_highwater=millis();
    uint32_t last_broadcast_message=millis();
    listenMode(radioChannel[radioModeParam.channel].rx);
    for(;;){
        print_high_water("RADIO PROCESS TASK",&last_send_highwater);
        if(xQueueReceive(RxQueue, &dataReceived,interval.radioreconnect/portTICK_PERIOD_MS)==pdTRUE){
            #if ROLE_NODE
                last_broadcast_message=millis();
            #endif

            ESP_LOGD("","RECEIVE DATA FROM RADIO");

            if(dataReceived.to==getID32() || dataReceived.to==0){//for this device or broadcast
                ESP_LOGD("","NEW MESSAGE FOR THIS DEVICE or Broadcast message_ID %u",dataReceived.id_message);// FOR ME.....
                radio_acknowledge_struct is_acknowledge;
                bool old_message=ackBuffer.get(dataReceived.id_message,is_acknowledge);
                is_acknowledge.rssi=radio.getRSSI();
                is_acknowledge.snr=radio.getSNR();
                is_acknowledge.message_id=dataReceived.id_message;
                is_acknowledge.message_type=dataReceived.type;
                if(old_message){//old message arrived and acknowledge
                        radio_status_struct status_radio;
                        status_radio.connected=true;
                        status_radio.channel=radioModeParam.channel;
                        xQueueOverwrite(radioStatusQueue,&status_radio);
                        ESP_LOGD("","ACKNOWLEDGE MESSAGE FOR MESSAGE ID %u",dataReceived.id_message);
                        CREATE_JSON_DOCUMENT(docFromRadio);
                        if(handler.structCommandToJson(docFromRadio,dataReceived)){
                            #if ROLE_GATEWAY
                            docFromRadio["rssi"]= is_acknowledge.rssi;
                            docFromRadio["snr"]= is_acknowledge.snr;
                            is_acknowledge.status_send=docFromRadio["data"]["status_code"].as<uint32_t>();
                            bool upload_status=send_mqtt_external(docFromRadio,docFromRadio["command"].as<const char*>(),1,2000);
                            ackBuffer.update(dataReceived.id_message,is_acknowledge);
                            #endif

                            #if ROLE_NODE
                            is_acknowledge.status_send=docFromRadio["data"]["status_code"].as<uint32_t>();
                            ackBuffer.update(dataReceived.id_message,is_acknowledge);
                            //TODO DO SOMETHING WITH THE DATA
                            #endif

                            #if ROLE_REPEATER
                             //TODO DO SOMETHING WITH THE DATA
                            #endif
                        }
                }

                else{ /// new message arrived
                    ESP_LOGD("","NEW MESSAGE SENDING ACKNOWLEDGE TO SENDER"); 
                    CREATE_JSON_DOCUMENT(docFromRadio);
                    if(handler.structCommandToJson(docFromRadio,dataReceived)){
                        radio_status_struct status_radio;
                        status_radio.connected=true;
                        status_radio.channel=radioModeParam.channel;
                        xQueueOverwrite(radioStatusQueue,&status_radio);
                        radio_response_data_struct dataResponse;
                        
                        #if ROLE_NODE
                            bool status_execution=handler.processCommand(docFromRadio);
                            status_execution?dataResponse.status_code=PACKAGE_TRANSMITTED_SUCSESS_EXECUTION:dataResponse.status_code=PACKAGE_TRANSMITTED_FAILED_EXECUTION;
                            dataResponse.status=status_execution;
                            is_acknowledge.status_send=dataResponse.status_code;
                            
                            //-------- Tambahan register gateway_id dan channel untuk node
                            //serializeJsonPretty(docFromRadio,Serial);
                            //Serial.println();

                            if(strcmp(docFromRadio["command"].as<const char*>(),"radio_broadcast_data")==0){
                                ESP_LOGD("","BROADCAST DATA FROM GATEWAY");
                                ESP_LOGD("","GATEWAY ID %u",docFromRadio["from"].as<uint32_t>());
                                ESP_LOGD("","CHANNEL %d",docFromRadio["data"]["channel"].as<uint8_t>());
                                ESP_LOGD("","UNIX %u",docFromRadio["data"]["unix_time"].as<uint32_t>());
                                ESP_LOGD("","INTERVAL %u",docFromRadio["data"]["interval_broadcast"].as<uint32_t>());
                                ESP_LOGD("","RSSI %f",is_acknowledge.rssi);
                                ESP_LOGD("","SNR %f",is_acknowledge.snr);

                                if(radioModeParam.channel!=docFromRadio["channel"].as<uint8_t>()||radioModeParam.gateway_id!=docFromRadio["from"].as<uint32_t>()){
                                    radioModeParam.channel=docFromRadio["channel"].as<uint8_t>();
                                    radioModeParam.gateway_id=docFromRadio["from"].as<uint32_t>();
                                    handler.processCommand("radio_mode",COMMAND_HANDLER_TYPE_SET,radioModeParam,getID16());
                                }
                        }
                            if(dataReceived.type!=COMMAND_HANDLER_TYPE_DAT_WITHOUT_ACK){
                                ackBuffer.insert(is_acknowledge);
                                headerRadioStruct radioData=handler.dataToStructCommand(dataReceived.from,"radio_response_data",COMMAND_HANDLER_TYPE_DAT_WITHOUT_ACK,dataResponse,dataReceived.id_message,getID32());
                                uint8_t* data = reinterpret_cast<uint8_t*>(&radioData);
                                send_radio_external(radioData.id_message,data,radioData.len);
                            } 
                        #endif
                        
                        #if ROLE_GATEWAY
                            bool uploaded=false;
                            //if broadcast message, to == 0, replace the to with gateway id
                            if(dataReceived.to==0)
                                docFromRadio["to"]=getID32();
                            
                            docFromRadio["rssi"]= is_acknowledge.rssi;
                            docFromRadio["snr"]= is_acknowledge.snr;
                            uploaded=send_mqtt_external(docFromRadio,docFromRadio["command"].as<const char*>(),1,2000);
                            if(uploaded){
                                dataResponse.status=true;
                                dataResponse.status_code=PACKAGE_TRANSMITTED_TO_GATEWAY_SUCSESS_UPLOAD;
                                is_acknowledge.status_send=dataResponse.status_code;
                            }
                            else{
                                dataResponse.status=false;
                                dataResponse.status_code=PACKAGE_TRANSMITTED_TO_GATEWAY_FAILED_UPLOAD;
                                is_acknowledge.status_send=dataResponse.status_code;
                            }
                            if(dataReceived.type!=COMMAND_HANDLER_TYPE_DAT_WITHOUT_ACK){
                                headerRadioStruct radioData=handler.dataToStructCommand(dataReceived.from,"radio_response_data",COMMAND_HANDLER_TYPE_DAT_WITHOUT_ACK,dataResponse,dataReceived.id_message,getID32());
                                uint8_t* data = reinterpret_cast<uint8_t*>(&radioData);
                                ackBuffer.insert(is_acknowledge);
                                send_radio_external(radioData.id_message,data,radioData.len);
                            }
                        #endif
                        }
                        else{
                            ESP_LOGE("","Cannot convert to json failed format");
                        }
                }

            } 
            
            else{
                ESP_LOGD("","NEW MESSAGE FOR OTHER IGNORED....");
                int i=0;
                Serial.println(dataReceived.to);
                Serial.println(dataReceived.from);
                Serial.println(dataReceived.id_message);
                Serial.println(dataReceived.type);
                Serial.println(dataReceived.key);
                Serial.println(dataReceived.commandId);
                Serial.println(dataReceived.len);
                while(i<sizeof(dataReceived.data)){
                    
                    Serial.write(dataReceived.data[i]);
                    i++;
                }
            }
          }
        
        #if ROLE_GATEWAY  
        if(millis()-last_broadcast_message>interval.radioreconnect*1000){
            ESP_LOGI("","Broadcast Message --------");
                headerRadioStruct radioData;
                radio_broadcast_data_struct broadcastData;
                broadcastData.unix_time=getDateTime().unixtime();
                broadcastData.interval_broadcast=interval.radioreconnect;
                broadcastData.channel=radioModeParam.channel;
                radioData=handler.dataToStructCommand(0,"radio_broadcast_data",COMMAND_HANDLER_TYPE_DAT_WITHOUT_ACK,broadcastData);
                uint8_t* data = reinterpret_cast<uint8_t*>(&radioData);
                send_radio_external(radioData.id_message,data,radioData.len);    
                last_broadcast_message=millis();      
        }
        #endif
        
        #if ROLE_NODE
        if(millis()-last_broadcast_message >interval.radioreconnect*2*1000){
            radioModeParam.channel>=3?radioModeParam.channel=0:radioModeParam.channel++;
            ESP_LOGI("","Change Channel to %d",radioModeParam.channel);
            listenMode(radioChannel[radioModeParam.channel].rx);
            ESP_LOGI("","Waiting For Message --------");
            last_broadcast_message=millis();
            radio_status_struct status_radio;
            status_radio.connected=false;
            status_radio.channel=radioModeParam.channel;
            xQueueOverwrite(radioStatusQueue,&status_radio);  
        }
        #endif
        vTaskDelay(20/portTICK_PERIOD_MS);    
    }
  }

  bool radio_task(){
    TURN_ON_3_3V;
    TURN_ON_BATCONTROL;
    
    if(xTaskCreatePinnedToCore(radio_process,"radio_process",10000,NULL,1,&radioTaskHandler,1)==pdPASS){
        ESP_LOGD("","radio process ready");
    return true;
   }

   return false;
}
  
