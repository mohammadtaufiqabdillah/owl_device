#include "gpsApp.h" 

struct VarGps{
  bool first_time=true;
  float last_latitude;
  float last_longitude;
  float accumulation;
  int fix_count;
};

static void smartDelay(unsigned long ms,TinyGPSPlus &gps)
{
  //DEBUG_PRINTLN("feed");
  unsigned long start = millis();
    while (Serial.available()>0){
      //Serial.write(Serial0.peek());
      gps.encode(Serial.read());
    }
  //DEBUG_PRINTLN("end feed");
}

void parsing_info(VarGps &var, TinyGPSPlus &gps,gps_status_struct &gpsStatus){
    double speed=0;
    double distance=0;
    if(gps.location.isValid() && gps.location.age()<=1500){
      speed=gps.speed.kmph();
      gpsStatus.status=true;
      if(var.first_time){
        var.last_latitude=gps.location.lat();
        var.last_longitude=gps.location.lng();
        var.first_time=false;
      }
      if(var.last_latitude!=0)
        distance=gps.distanceBetween(var.last_latitude,var.last_longitude,gps.location.lat(),gps.location.lng());
      if(var.fix_count>=20){
          if(distance>=2){
            var.accumulation+=distance/1000;
            gpsStatus.km_season=var.accumulation;

          }
      }
      gpsStatus.speed=speed;
      speed>gpsStatus.top_speed?gpsStatus.top_speed=speed:gpsStatus.top_speed=gpsStatus.top_speed;
      var.last_latitude=gps.location.lat();
      var.last_longitude=gps.location.lng();
      gpsStatus.latitude=gps.location.lat();
      gpsStatus.longitude=gps.location.lng();
      gpsStatus.altitude=gps.altitude.meters();
      
      DateTime gpsDate(gps.date.year(),gps.date.month(),gps.date.day(),gps.time.hour(),gps.time.minute(),gps.time.second());
      gpsStatus.unix=gpsDate.unixtime();      
    }
    else{
      gpsStatus.status=false;
      var.last_latitude=gps.location.lat();
      var.last_longitude=gps.location.lng();
      }
}

void gpsProcess( void * pvParameters ){
  TinyGPSPlus gps;
  VarGps gpsVar;
  gps_status_struct statusGps;
  statusGps.status=false;
  int fix_count=0;
  bool first_time=true;
  ESP_LOGI ("","GPS App Running");
  uint32_t last_gps_send=millis();
  uint32_t last_send_highwater=millis();
  for(;;){
    print_high_water("GPS TASK",&last_send_highwater);
    smartDelay(10,gps);
    parsing_info(gpsVar,gps,statusGps);
    if(gps.location.isUpdated()){
      //Serial.println(gps.location.lat());
      if(gps.location.isValid() && gps.location.age()<=1500){
        gpsVar.fix_count++;
      }
      else{
        gpsVar.first_time=true;
        gpsVar.fix_count=0;
      } 
    }
    
    if(millis()-last_gps_send>100){
      last_gps_send=millis();
    }
    xQueueOverwrite(gpsStatusQueue,&statusGps); 
    vTaskDelay(200/portTICK_PERIOD_MS);//give other task chance  
}
}
  

bool gps_task(){
    gpsStatusQueue=xQueueCreate(1,sizeof(gps_status_struct));
    if(xTaskCreatePinnedToCore(gpsProcess,"gpsApp",5000,NULL,1,&gpsTaskHandler,1)==pdPASS)
        return true;
    else
        return false;
}