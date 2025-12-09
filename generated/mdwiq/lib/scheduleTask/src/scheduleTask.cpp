#include "scheduleTask.h"

#define DAILY_SCHEDULE 1
#define MONTHLY_SCHEDULE 2
#define YEARLY_SCHEDULE 3

bool schedule_updated=false;
RTC_DATA_ATTR int last_schedule=-1;
RTC_DATA_ATTR int next_schedule=-1;

xQueueHandle rtcTempQueue;

float get_rtc_temp(){
  float temp;
  if(rtcTempQueue!=NULL && uxQueueMessagesWaiting(rtcTempQueue)>0){
    xQueuePeek(rtcTempQueue,&temp,0);
  }
  else
    temp=temperatureRead();
  return temp;
}

void get_schedule(JsonDocument &doc){
  DateTime date_schedule;
  date_schedule=DateTime(doc["data"]["date"].as<const char*>());
  CREATE_JSON_DOCUMENT(buffer_daily);
  CREATE_JSON_DOCUMENT(buffer_monthly);
  char dirpath [100];
  
  //cek daily schedule
  sprintf(dirpath,"/daily/%02d",date_schedule.dayOfTheWeek());
  handler.listFile(dirpath,buffer_daily);

  sprintf(dirpath,"/monthly/%02d/%02d",date_schedule.month(),date_schedule.day());
  handler.listFile(dirpath,buffer_monthly);

  doc["data"]["daily"]=buffer_daily.as<JsonArray>();
  doc["data"]["monthly"]=buffer_monthly.as<JsonArray>();
}

void clear_folder(const char * path){
  CREATE_JSON_DOCUMENT (doc);

  handler.listFile(path,doc);
  if (doc.isNull())
    return;

  else{
    JsonArray array_of_file=doc.as<JsonArray>();
    for (size_t i = 0; i < array_of_file.size(); i++) {
      char complete_path [120];
      sprintf(complete_path,"%s/%s",path,array_of_file[i].as<const char*>());
      handler.deleteFile(complete_path);
    }  
  }
}

bool remove_schedule(int type){
  bool state=false;
  switch (type)
  {
    case DAILY_SCHEDULE:{
      clear_folder("/daily/01");
      clear_folder("/daily/02");
      clear_folder("/daily/03");
      clear_folder("/daily/04");
      clear_folder("/daily/05");
      clear_folder("/daily/06");
    }
    break;

    case MONTHLY_SCHEDULE:{
      //sprintf(dirpath,"/monthly");
    }
    break;
    
    case YEARLY_SCHEDULE:{
      //sprintf(dirpath,"/yearly");
    }  
    break;    
    
    default:{
      return false;
    }

    return true;
  }
  return false;  
}
/*
daily>0>1050

monthly>11>32>1256

*/
void add_schedule(int type, JsonDocument &doc){
  char dirpath [50];
  char filepath [80];
  switch (type)
  {
    case DAILY_SCHEDULE:{
      sprintf(dirpath,"/daily/%02d",doc["data"]["day"].as<uint8_t>());
      sprintf(filepath,"/daily/%02d/%02d%02d",doc["data"]["day"].as<uint8_t>(),doc["data"]["hour"].as<uint8_t>(),doc["data"]["minute"].as<uint8_t>());
      if(doc["data"]["day"].as<uint8_t>()<0 && doc["data"]["day"].as<uint8_t>()>6){
        doc["status"]="error";
        doc["error"]="day invalid";
        return;
      }
      if(doc["data"]["hour"].as<uint8_t>()<0 && doc["data"]["hour"].as<uint8_t>()>23){
        doc["status"]="error";
        doc["error"]="hour invalid";
        return;
      }
    }
    break;

    case MONTHLY_SCHEDULE:{
      sprintf(dirpath,"/monthly/%02d/%02d",doc["data"]["month"].as<uint8_t>(),doc["data"]["date"].as<uint8_t>());
      sprintf(filepath,"/monthly/%02d/%02d/%02d%02d",doc["data"]["month"].as<uint8_t>(),doc["data"]["date"].as<uint8_t>(),doc["data"]["hour"].as<uint8_t>(),doc["data"]["minute"].as<uint8_t>());
      
      if(doc["data"]["month"].as<uint8_t>()<1 && doc["data"]["month"].as<uint8_t>()>12){
        doc["status"]="error";
        doc["error"]="month invalid";
        return;
      }

      if(doc["data"]["date"].as<uint8_t>()<1 && doc["data"]["date"].as<uint8_t>()>31){
        doc["status"]="error";
        doc["error"]="date invalid";
        return;
      }

      if(doc["data"]["day"].as<uint8_t>()<0 && doc["data"]["day"].as<uint8_t>()>6){
        doc["status"]="error";
        doc["error"]="day invalid";
        return;
      }
      if(doc["data"]["hour"].as<uint8_t>()<0 && doc["data"]["hour"].as<uint8_t>()>23){
        doc["status"]="error";
        doc["error"]="hour invalid";
        return;
      }
    }
    break;
    
    case YEARLY_SCHEDULE:{
      sprintf(dirpath,"/yearly/%d/%02d/%02d",doc["data"]["year"].as<uint16_t>(),doc["data"]["month"].as<uint8_t>(),doc["data"]["date"].as<uint8_t>());
      sprintf(filepath,"/yearly/%d/%02d/%02d/%02d%02d",doc["data"]["year"].as<uint16_t>(),doc["data"]["month"].as<uint8_t>(),doc["data"]["date"].as<uint8_t>(),doc["data"]["hour"].as<uint8_t>(),doc["data"]["minute"].as<uint8_t>());       
      
      if(doc["data"]["month"].as<uint8_t>()<1 && doc["data"]["month"].as<uint8_t>()>12){
        doc["status"]="error";
        doc["error"]="month invalid";
        return;
      }

      if(doc["data"]["date"].as<uint8_t>()<1 && doc["data"]["date"].as<uint8_t>()>31){
        doc["status"]="error";
        doc["error"]="date invalid";
        return;
      }

      if(doc["data"]["day"].as<uint8_t>()<0 && doc["data"]["day"].as<uint8_t>()>6){
        doc["status"]="error";
        doc["error"]="day invalid";
        return;
      }
      if(doc["data"]["hour"].as<uint8_t>()<0 && doc["data"]["hour"].as<uint8_t>()>23){
        doc["status"]="error";
        doc["error"]="hour invalid";
        return;
      }  
  
    }
    break;    
  
    default:{
      doc["status"]="error";
      doc["error"]="unknown schedule type";
    }
    return;
  }


  if(!handler.createDir(dirpath)){
    doc["status"]="error";
    doc["error"]="failed to create dir";
    return;
  }

  CREATE_JSON_DOCUMENT(buffer);
  buffer=doc["data"]["command"].as<JsonObject>();
  if (buffer.isNull()) {
    doc["status"]="error";
    doc["error"]="failed to parsing command json";
    return;
  }

  //change to default password
  buffer["key"]=getID16();
  
  //validate command to be executed is valid?
        if(!handler.validateFormat(buffer)){
            doc["status"] = buffer["status"];
            doc["error"] = buffer["error"];
            if(!buffer["status_code"].isNull()){
                doc["status_code"] = buffer["status_code"];
            }
            //ESP_LOGD("COMMAND_RADIO", "status: error, error: invalid command format");
            return;
        }

  buffer["executed"]="2020-01-01 00:00:00"; //set schedule to true
  if(!handler.writeFile(filepath,buffer)){
    doc["status"]="error";
    doc["error"]="failed to write command";
    return;
  }
  ESP_LOGI("","schedule%s created",filepath);
  schedule_updated=true;
  doc["status"]="success";
  doc["error"]="none";
  //doc.remove("data"); //remove data to avoid memory leak
  doc["data"]["path"]=filepath;
  //serializeJsonPretty(buffer,Serial);
  //Serial.println();


}

void add_schedule_daily(JsonDocument &doc){
  if(doc["data"]["everyday"].as<bool>()){
    for(int i=0;i<7;i++){
      doc["data"]["day"]=i; //set every day
      add_schedule(DAILY_SCHEDULE,doc);
    }
  }
  else
    add_schedule(DAILY_SCHEDULE,doc);  
  doc.remove("data"); //remove data to avoid memory leak
  }

void add_schedule_monthly(JsonDocument &doc){
  if(doc["data"]["everymonth"].as<bool>()){
    for(int i=1;i<=12;i++){
      doc["data"]["month"]=i; //set every month
      add_schedule(MONTHLY_SCHEDULE,doc);
    }
  }
  else
    add_schedule(MONTHLY_SCHEDULE,doc);
  doc.remove("data"); //remove data to avoid memory leak
  }

void add_schedule_yearly(JsonDocument &doc){
  add_schedule(YEARLY_SCHEDULE,doc);
  doc.remove("data"); //remove data to avoid memory leak
}

void clear_schedule(JsonDocument &doc){
  remove_schedule(DAILY_SCHEDULE);
  doc["status"]="sucess";
  doc["error"]="none";
  doc["description"]="please recheck using get schedule";
}

bool sinc_time(utcoffset_struct &utcoffset, DateTime &now) {
  struct tm timeinfo;
  // Dapatkan waktu lokal
  //ESP_LOGD("","mulai sinc time");
  gps_status_struct statusgps;
  if (!getLocalTime(&timeinfo,100)) {
      statusgps=getGpsStatus();
        if(statusgps.status){
          //ESP_LOGD(TAGS,"sinkronisasi time by gps");
          if(statusgps.unix<1672506000) //jam < 2023:01:01 >> 
            return false;
          now = DateTime(statusgps.unix); // Atur 'now' sebagai waktu terkini
          now = now.operator+(TimeSpan(utcoffset.utcoffset*60*60));            
          return true; // Sinkronisasi berhasil melalui gps
        }
      //ESP_LOGI(TAGS,"gagal sinkronisasi time");
      return false; // Gagal sinkronisasi
  }

  // Konversi struct tm menjadi Unix timestamp
  time_t epochTime = mktime(&timeinfo);
  now = DateTime(epochTime); // Atur 'now' sebagai waktu terkini
  now = now.operator+(TimeSpan(utcoffset.utcoffset*60*60));
  //ESP_LOGD(TAGS,"sinkronisasi time by wifi / local rtc");
  return true; // Sinkronisasi berhasil
}

void cronjob(JsonDocument &doc_schedule_daily, DateTime &now, int &last_schedule, bool is_montly=false){
    //ESP_LOGI("","CRONJOB CHECKER %d:%d",now.hour(),now.minute());  
    JsonArray array_daily=doc_schedule_daily.as<JsonArray>();
    for (size_t i = 0; i < array_daily.size(); i++) {
        
        int jam_schedule=atoi(array_daily[i].as<const char*>());
        int jam_sekarang=(now.hour()*100)+now.minute();
        //ESP_LOGI("","JAM SCHEDULE %d:%d",jam_schedule,jam_sekarang);
        /*
        hour * 100 + minute

        jika jam skedul sudah lewat, dan last skedule nya bukan skedule sekarang
        dan lewatnya masih dalam rentang 1 menit

        lakukan pengecekan lanjutan
        */
       //melakukan pengecekan schedule 5 menit ke belakang
        if(jam_sekarang>jam_schedule && jam_schedule!=last_schedule && ((jam_sekarang==jam_schedule)||(jam_sekarang-jam_schedule)<=5)){
          //ESP_LOGI("","JAM SCHEDULE TO BE EXECUTED %d:%d",jam_schedule,jam_sekarang);
          CREATE_JSON_DOCUMENT(cronjob);
          char path [100];
          if(is_montly)
            sprintf(path,"/monthly/%02d/%02d/%04d",now.month(),now.day(),jam_schedule);  
          else
            sprintf(path,"/daily/%02d/%04d",now.dayOfTheWeek(),jam_schedule);
          
          if(!handler.readFile(path,cronjob)){//coba dua kali baca
            vTaskDelay(100);
            handler.readFile(path,cronjob);
          }
          //serializeJsonPretty(cronjob,Serial);
          //Serial.println();
            
          //JIKA BELUM PERNAH DILAKSANAKAN LANGSUNG LAKSANAKAN
          if(!cronjob["executed"].is<JsonVariant>()){
            ESP_LOGI("","EXECUTING SCHEDULE");
            if(strcmp(cronjob["command"].as<const char*>(),"restart") == 0){
              ESP_LOGW("","RESTART SCHEDULE EXECUTED, UPDATE EXECUTED TIME BEFORE RESTART"); 
              cronjob["executed"]=now.timestamp();
              send_mqtt_external(cronjob,"schedule",0);
              last_schedule=jam_schedule;
              if((i+1)<array_daily.size())
                next_schedule=atoi(array_daily[i].as<const char*>());
            //coba write 2 kali dengan delay 100 ms jika gagal
              if(!handler.writeFile(path,cronjob)){
                vTaskDelay(100);
                handler.writeFile(path,cronjob);
              }
            }
            cronjob["key"]=getID16();//set key to default password
            handler.processCommand(cronjob);
            cronjob["executed"]=now.timestamp();
            send_mqtt_external(cronjob,"schedule",0);
            last_schedule=jam_schedule;
            if((i+1)<array_daily.size())
                next_schedule=atoi(array_daily[i].as<const char*>());
            //coba write 2 kali dengan delay 100 ms jika gagal
            if(!handler.writeFile(path,cronjob)){
              vTaskDelay(100);
              handler.writeFile(path,cronjob);
            }
          }

          //JIKA SUDAH PERNAH DILAKSANAKAN, CEK APAKAH SUDAH DIEXECUTE HARI INI
          else{
            DateTime executed=DateTime(cronjob["executed"].as<const char*>());
            if(executed.day()!=now.day()){
              ESP_LOGI("","EXECUTING SCHEDULE DIFFERENT DAY");
              if(strcmp(cronjob["command"].as<const char*>(),"restart") == 0){
                ESP_LOGW("","RESTART SCHEDULE EXECUTED, UPDATE EXECUTED TIME BEFORE RESTART"); 
                cronjob["executed"]=now.timestamp();
                send_mqtt_external(cronjob,"schedule",0);
                last_schedule=jam_schedule;
                if((i+1)<array_daily.size())
                  next_schedule=atoi(array_daily[i].as<const char*>());
              //coba write 2 kali dengan delay 100 ms jika gagal
                if(!handler.writeFile(path,cronjob)){
                  vTaskDelay(100);
                  handler.writeFile(path,cronjob);
                }
              }
              cronjob["key"]=getID16();//set key to default password
              handler.processCommand(cronjob);
              cronjob["executed"]=now.timestamp();
              send_mqtt_external(cronjob,"schedule",0);
              last_schedule=jam_schedule;
              if((i+1)<array_daily.size())
                next_schedule=atoi(array_daily[i].as<const char*>());
              //coba write 2 kali dengan delay 100 ms jika gagal
              if(!handler.writeFile(path,cronjob)){
                vTaskDelay(100);
                handler.writeFile(path,cronjob);
              }
            }

            else{
              ESP_LOGI("","Cron Already executed");
              last_schedule=jam_schedule;
              if((i+1)<array_daily.size())
                next_schedule=atoi(array_daily[i].as<const char*>());
            }

          }
        }
    
    }
}

void schedule_process(void * pvParameters){
  RTC_DS3231 rtc;  
  DateTime now;
  bool is_use_rtc=rtc.begin();
  bool rtc_lost_power=rtc.lostPower();
  utcoffset_struct utcoffset;
  handler.loadStruct(utcoffset);

  configTime(0,0,"0.pool.ntp.org","1.pool.ntp.org");

  if(is_use_rtc && !rtc_lost_power){//directly sinc using
    now=rtc.now();
    ESP_LOGI("","USING RTC EXTERNAL-----------------------------------------");
  }
  else{
    if(!sinc_time(utcoffset,now)){//sinc using local rtc
        ESP_LOGE("","All sycn time fail, sync using last saved value");
        last_epoch_struct epoch;
        while(!handler.loadStruct(epoch))
          vTaskDelay(1000/portTICK_RATE_MS);
        now=DateTime(epoch.epoch);
    }
  }

  interval_struct interval;
  handler.loadStruct(interval);
  //-------------------------------------
  uint32_t last_sync_time=0;
  uint32_t last_send_highwater=millis();
  uint32_t last_backup=millis();
  CREATE_JSON_DOCUMENT(doc_schedule_daily);
  CREATE_JSON_DOCUMENT(doc_schedule_monthly);
  uint8_t change_day=99;
  for(;;){        
    print_high_water("SCHEDULE TASK",&last_send_highwater);
    uint32_t time_eplased=millis()-last_sync_time;
    bool sync_sucess=sinc_time(utcoffset,now);
    last_sync_time=millis();

    if(sync_sucess&&is_use_rtc&&rtc_lost_power){// is use rtc and rtc lost power and success sycn adjust the rtc
        rtc.adjust(now);
        ESP_LOGI("","SYNC RTC EXTERNAL-----------------------------------------");
        rtc_lost_power=false;
      }  
    if(!sync_sucess){
        TimeSpan oneSecond(time_eplased/1000);
        now=now.operator+(oneSecond);
      }
    uint32_t unixtime=now.unixtime();
    xQueueOverwrite(unixTimeStatusQueue,&unixtime);

    if(is_use_rtc){
      float temp=rtc.getTemperature();
      xQueueOverwrite(rtcTempQueue,&temp);
    }
    
    if(millis()-last_backup>(interval.backup_data*1000)){
      last_backup=millis();
      last_epoch_struct epoch;
      epoch.epoch=now.unixtime();
      handler.processCommand("last_epoch",COMMAND_HANDLER_TYPE_SET,epoch,getID16());
    }
    
    
    vTaskDelay(1000/portTICK_PERIOD_MS);
    
    if(change_day!=now.dayOfTheWeek() || schedule_updated){//day changed reload schedule
      schedule_updated=false;
      change_day=now.dayOfTheWeek();
      last_schedule=-1;
      char dirpath [100];
      //cek daily schedule
      sprintf(dirpath,"/daily/%02d",now.dayOfTheWeek());
      handler.listFile(dirpath,doc_schedule_daily);
      //serializeJson(doc_schedule_daily,Serial);
      //Serial.println();
      sprintf(dirpath,"/monthly/%02d/%02d",now.month(),now.day());
      handler.listFile(dirpath,doc_schedule_monthly);
      //serializeJson(doc_schedule_monthly,Serial);
      //Serial.println();
    }

    cronjob(doc_schedule_daily,now,last_schedule);
    cronjob(doc_schedule_monthly,now,last_schedule,true);
    //Serial.printf("last schedule %d\n",last_schedule);
  }
}

bool schedule_task(){
unixTimeStatusQueue=xQueueCreate(1,sizeof(uint32_t));
rtcTempQueue=xQueueCreate(1,sizeof(float));
handler.registerCommand("schedule_daily",add_schedule_daily);
handler.registerCommand("schedule_monthly",add_schedule_monthly);
handler.registerCommand("schedule_yearly",add_schedule_yearly);
handler.registerCommand("get_schedule",get_schedule);
handler.registerCommand("clear_schedule",clear_schedule);

if(xTaskCreatePinnedToCore(schedule_process,"schedule_task",8024,NULL,1,&scheduleTaskHandler,1)==pdPASS){
    ESP_LOGI("","schedule_task ready");
    return true;
 }
 return false;
}