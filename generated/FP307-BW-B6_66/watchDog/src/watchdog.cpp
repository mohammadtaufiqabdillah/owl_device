#include "watchDog.h"

errorBuffer error_class(20);

bool get_error_status(JsonDocument &doc){
  return error_class.get(doc);
}

bool get_error_status(error_struct &error){
  return error_class.get(error.error_code, error);
}

bool insert_errorcode(error_struct &error){
  return error_class.update(error.error_code, error);
}

void watchdogApp( void * pvParameters ){
  INIT_CE;
  TURN_ON_CE;

  INIT_BATSENSE;
  INIT_SOLAR_SENSE;
  INIT_BATCONTROL_SENSE;
  
  INIT_3_3V_CONTROL;
  INIT_BATCONTROL;

  long long last_kick_main=millis();
  long long last_kick_ui=millis();
  uint32_t lost_main=0;
  uint32_t lost_ui=0;
  #if CONFIG_IDF_TARGET_ESP32
    esp_adc_cal_characteristics_t *adc_chars;
    const adc_atten_t atten = ADC_ATTEN_DB_12;
    const adc_unit_t unit = ADC_UNIT_1;
    adc_chars = (esp_adc_cal_characteristics_t *)calloc(1, sizeof(esp_adc_cal_characteristics_t));
    adc1_config_width(ADC_WIDTH_BIT_12);
    
    #if USE_BATSENSE
      const adc1_channel_t batsense = BATSENSE_CHANNEL;
      adc1_config_channel_atten(batsense, atten);
    #endif

    #if USE_SOLAR_SENSE
      const adc1_channel_t solarvoltage = SOLAR_VOLTAGE_CHANNEL;
      adc1_config_channel_atten(solarvoltage, atten);
    #endif
    
    esp_adc_cal_value_t val_type = esp_adc_cal_characterize(unit, atten, ADC_WIDTH_BIT_12, 1100, adc_chars);
    
    if (val_type == ESP_ADC_CAL_VAL_EFUSE_VREF) {
      ESP_LOGD("","eFuse Vref digunakan untuk kalibrasi");
    } else if (val_type == ESP_ADC_CAL_VAL_EFUSE_TP) {
      ESP_LOGD("","eFuse Two Point digunakan untuk kalibrasi");
    } else {
      ESP_LOGD("","Default Vref digunakan untuk kalibrasi");
    }
  #endif

  power_status_struct deviceInfo;
  bool first_time=true;
  uint8_t last_bat=0;
  bool cut_off=false;
  for(;;){

    #if USE_BATSENSE
      #if CONFIG_IDF_TARGET_ESP32S3
        deviceInfo.vBat = analogReadMilliVolts(BATSENSE);
      
      #elif CONFIG_IDF_TARGET_ESP32
        uint32_t voltageReadingBat = adc1_get_raw(batsense);
        deviceInfo.vBat = esp_adc_cal_raw_to_voltage(voltageReadingBat, adc_chars);
      #endif

      deviceInfo.vBat=(deviceInfo.vBat*BATSENSE_MULTIPLIER)+BATSENSE_CAL;
      deviceInfo.bat=int(mapFloat(float(deviceInfo.vBat),3300,4200,0,100));
    #endif

    
    #if USE_SOLAR_SENSE
      #if CONFIG_IDF_TARGET_ESP32S3
        deviceInfo.vCharge = analogReadMilliVolts(SOLAR_VOLTAGE_SENSE);
      
      #elif CONFIG_IDF_TARGET_ESP32
        uint32_t voltageReadingSolar = adc1_get_raw(solarvoltage);
        deviceInfo.vCharge = esp_adc_cal_raw_to_voltage(voltageReadingSolar, adc_chars);
      #endif

      deviceInfo.vCharge=(deviceInfo.vCharge*SOLAR_VOLTAGE_MULTIPLIER)+SOLAR_VOLTAGE_CAL;

      if(!deviceInfo.isCharge && deviceInfo.vCharge>=3700){//first plugin recharge
          ESP_LOGD("","PLUGGED");
          deviceInfo.isCharge=true;
          TURN_ON_CE;
      }
      else if(deviceInfo.isCharge && deviceInfo.vCharge<3700){
          ESP_LOGD("","UNPLUGGED");
          deviceInfo.isCharge=false;
      }

    #else

      if(first_time){
        last_bat=deviceInfo.bat;
        first_time=false;
      }
      else{
        if(int(last_bat-deviceInfo.bat)>10){//lonjakan 10 % anggap sedang di charge
          deviceInfo.isCharge=true;
          ESP_LOGD("","PLUGGED");
          last_bat=deviceInfo.bat;
          //TURN_ON_CE;
        }
        else if(int(deviceInfo.bat-last_bat)<10){
          deviceInfo.isCharge=false;
          ESP_LOGD("","UNPLUGGED");
          last_bat=deviceInfo.bat;
        }
      }
    #endif

    #if USE_CHARGE_ENABLE
      if(!cut_off && deviceInfo.vBat>=DEFAULT_CUTOFF_CHARGE){
        cut_off=true;
        TURN_OFF_CE;
        ESP_LOGI("","CUTOFF CHARGE");
        //Serial.printf("CUTOFF %d, \n",deviceInfo.vBat);
      }      
      
      else if(cut_off && deviceInfo.vBat<=DEFAULT_RESUME_CHARGE){
        cut_off=false;
        TURN_ON_CE;
        ESP_LOGI("","RESUME CHARGE");
      }
    #endif
      
    if(powerStatusQueue!=NULL){
      xQueueOverwrite(powerStatusQueue,&deviceInfo);
    }

        /*
      if(xSemaphoreTake(watchDogQueue,0)==pdTRUE){ 
        last_kick_main=millis();
        lost_main=0;
      }
      else
        lost_main++;

 ///WATCHDOG UNTUK UI THREAD
      if(xSemaphoreTake(watchDogQueueUI,0)==pdTRUE){ 
        last_kick_ui=millis();
        lost_ui=0;
      }
      else
        lost_ui++;        
*/
  vTaskDelay(5000/portTICK_PERIOD_MS);
  if(lost_main>=7){
    ESP_LOGD("","MAIN WATCHDOG TRIGGERED");
    esp_restart();
  }

  if(lost_ui>=7){
    ESP_LOGD("","UI WATCHDOG TRIGGERED");
    esp_restart();
  }

  }
}
  
bool watchdog_task(){
    //watchDogQueue=xSemaphoreCreateBinary();
    //watchDogQueueUI=xSemaphoreCreateBinary();
    powerStatusQueue=xQueueCreate(1,sizeof(power_status_struct));
    if(xTaskCreatePinnedToCore(watchdogApp,"watchDog",2048,NULL,1,&watchDogTaskHandler,1)==pdPASS)
      return true;
    else
      return false;
}