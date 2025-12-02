#ifndef MQTTTASK_H
#define MQTTTASK_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <commonvar.h>
#include <commondef.h>
#include <MQTT.h>
#include <WiFiClient.h>
#include <WiFi.h>

bool send_mqtt_external(JsonDocument &doc,const char * description, int qos, int timeout);

struct status_mqtt_struct
{
    bool mqtt_connected=false;
    int32_t mqtt_reconnect=0;
    bool wifi_connected=false;
    int32_t wifi_reconnect=0;
};

extern QueueHandle_t statusMqttQueue;

bool get_mqtt_status(status_mqtt_struct &result);

template <typename T>
bool send_mqtt_external(const char * commandName,uint8_t command_hander_type,const T& data,const char* topic_description,uint8_t qos,uint16_t key,uint32_t timeout=2000){
  headerCommandStruct header=handler.dataToStructCommand(commandName,command_hander_type,data,0);
  CREATE_JSON_DOCUMENT(doc);  
  if(!handler.structCommandToJson(doc,header))
    return false;
  return send_mqtt_external(doc,topic_description,qos,timeout);
}

bool mqtt_task();
#endif
