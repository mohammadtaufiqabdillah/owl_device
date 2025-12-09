#ifndef COMMANDHANDLER_H
#define COMMANDHANDLER_H

#include <ArduinoJson.h>
#include <FS.h>
#include <RelayController.h>
#include <commandHandlerQueue.h>
#include <unordered_map>
#include <functional>
#include <vector>
#include <commondef.h>
#include <rtclib.h>

union converter_float
{
  float data;
  byte array[4];
  char chararr[4];
};

union converter_double
{
  double data;
  byte array[4];
  char chararr[4];
};

union converter_uint32
{
  uint32_t data;
  byte array[4];
  char chararr[4];
};

union converter_uint16
{
  uint16_t data;
  byte array[2];
  char chararr[2];  
};

union converter_uint8
{
  uint8_t data;
  byte array[1];
  char chararr[1];  
};

union converter_int32
{
  int32_t data;
  byte array[4];
  char chararr[4];
};

union converter_int16
{
  int16_t data;
  byte array[2];
  char chararr[2];  
};

union converter_int8
{
  int8_t data;
  byte array[1];
  char chararr[1];  
};

union converter_bool
{
  bool data;
  byte array[1];
  char chararr[1];  
};

class CommandHandler {
  private:
    fs::FS& fileSystem; 
    JsonDocument setupDoc;
    JsonDocument setupIdDoc;
    JsonDocument exeDoc;
    JsonDocument exeIdDoc;
    JsonDocument dataDoc;
    JsonDocument dataIdDoc;
    uint32_t setup_key=99912;
    typedef std::function<void(JsonDocument &)> CallbackFunc;
    std::unordered_map<std::string, CallbackFunc> commandMap;
    bool useKey=false;
    
    void merge(JsonVariant dst, JsonVariantConst src);

    bool getAccess();

    bool releaseAccess(bool status);

    void sendToQueue(const char* queueName, bool status=true);

    bool saveSettings(const char* commandName, JsonDocument &doc);
    // Helper function to check if the key is correct
    bool checkKey(uint32_t key);

    bool processCommandSetter(JsonDocument &doc);

    bool processCommandGetter(JsonDocument &doc);
    
    bool processCommandExecutter(JsonDocument &doc);

    bool executeCommand(const char* commandName, JsonDocument &doc);

    bool processCommandDeleter(JsonDocument &doc);

    bool loadSettingsInternal(const char* commandName, JsonDocument &doc);

    void queueBegin();    
  
    bool validateDataType(JsonDocument &setupDoc, JsonDocument &Doc);

    template <typename T>
    std::vector<uint8_t> structToByteArray(const T& data,uint8_t commandType, uint16_t commandId){
      std::vector<uint8_t> buffer;
      char commandIdChar [5];
      sprintf(commandIdChar,"%u",commandId);

      //Serial.printf("data id %u\n",commandId);
      if (commandType==COMMAND_HANDLER_TYPE_EXE){
          if(!exeIdDoc[commandIdChar]){
          ESP_LOGE("","tidak ada command id %d di exe doc",commandId);
          return buffer;
          }
      }
      else if(commandType==COMMAND_HANDLER_TYPE_DAT || commandType==COMMAND_HANDLER_TYPE_DAT_WITHOUT_ACK ){
          if(!dataIdDoc[commandIdChar]){
              ESP_LOGE("","tidak ada command id %d di data doc",commandId);
              return buffer;
              }
      }
      else{
          if(!setupIdDoc[commandIdChar]){
              ESP_LOGE("","tidak ada command id %d di setup doc",commandId);
              return buffer;
              }
      }


      JsonObject setupStruct; 
      if (commandType==COMMAND_HANDLER_TYPE_EXE)
          setupStruct = exeDoc[exeIdDoc[commandIdChar].as<const char*>()];
      else if(commandType==COMMAND_HANDLER_TYPE_DAT|| commandType==COMMAND_HANDLER_TYPE_DAT_WITHOUT_ACK)
          setupStruct = dataDoc[dataIdDoc[commandIdChar].as<const char*>()];
      else
          setupStruct = setupDoc[setupIdDoc[commandIdChar].as<const char*>()];

      // Loop through the expected keys in the setup JSON and validate the types
      JsonArray lens_json = setupStruct["data_len"].as<JsonArray>();
      JsonArray types_json = setupStruct["data_type"].as<JsonArray>();

      if (setupStruct) {
          //buffer.push_back(commandId);
          uint8_t* rawData = (uint8_t*)&data;
          int offset = 0;

          for (size_t i = 0; i < types_json.size(); i++) {
              const char* type = types_json[i];
              int len = lens_json[i];
              if (strcmp(type,"string")==0) {
                  int str_len=strlen((char*)rawData);
                  buffer.push_back(str_len);
                  char string_buffer[255];
                  for(int p=0; p<str_len;p++){
                    buffer.push_back(*rawData);
                    rawData++;  
                  }
                  int jump=len-str_len;
                  rawData=rawData+jump;
              } 
              else {
                  for(int p=0; p<len;p++){
                    buffer.push_back(*rawData);
                    rawData++; 
                    }
              }
          }
      }
      
      else{
          ESP_LOGE("","Tidak ada setup untuk mengencode struct to byte array");
      }
      return buffer;
    }

    bool jsonToByteArray(const JsonDocument &doc, headerCommandStruct &commandStruct){
      char commandIdChar [10];
      sprintf(commandIdChar,"%u",commandStruct.commandId);
  
      //Serial.printf("data id %u\n",commandId);
      if (commandStruct.type==COMMAND_HANDLER_TYPE_EXE){
          if(!exeIdDoc[commandIdChar]){
          ESP_LOGE("","tidak ada command id %d di exe doc",commandStruct.commandId);
          return false;
          }
      }
      else if(commandStruct.type==COMMAND_HANDLER_TYPE_DAT|| commandStruct.type==COMMAND_HANDLER_TYPE_DAT_WITHOUT_ACK){
          if(!dataIdDoc[commandIdChar]){
              ESP_LOGE("","tidak ada command id %d di data doc",commandStruct.commandId);
              return false;
              }
      }
      else{
          if(!setupIdDoc[commandIdChar]){
              ESP_LOGE("","tidak ada command id %d di setup doc",commandStruct.commandId);
              return false;
              }
      }
  
      JsonObject setupStruct; 
      if (commandStruct.type==COMMAND_HANDLER_TYPE_EXE)
          setupStruct = exeDoc[exeIdDoc[commandIdChar].as<const char*>()];
      else if(commandStruct.type==COMMAND_HANDLER_TYPE_DAT|| commandStruct.type==COMMAND_HANDLER_TYPE_DAT_WITHOUT_ACK)
          setupStruct = dataDoc[dataIdDoc[commandIdChar].as<const char*>()];
      else
          setupStruct = setupDoc[setupIdDoc[commandIdChar].as<const char*>()];
      // Loop through the expected keys in the setup JSON and validate the types
      JsonArray lens_json = setupStruct["data_len"].as<JsonArray>();
      JsonArray types_json = setupStruct["data_type"].as<JsonArray>();  
      JsonArray key_json = setupStruct["key"].as<JsonArray>();
      uint8_t output [1024];
      uint8_t* rawOutput = (uint8_t*)&commandStruct.data;
      uint16_t sizeDataReal=0;
      for (size_t i = 0; i < types_json.size(); i++) {
          const char * type = types_json[i];
          const char * keyNya=key_json[i];
          int len = lens_json[i];
          //Serial.printf("decoding loop %d, type %s, size %d, size_loop %d\n",i,type,len,types_json.size());
  
          if (strcmp(type,"string")==0) {
              uint8_t str_len=strlen(doc["data"][keyNya]);
              char data_string[255];
              strlcpy(data_string,doc["data"][keyNya]|"null",sizeof(data_string));
              data_string[str_len]='\0';
              *rawOutput=str_len;
              rawOutput++;
              sizeDataReal++;
              //Serial.println("-----------");
              
              if (str_len > len){
                //Serial.println("overflow.....");
                return false;
              }  
           //     Serial.printf("type string, len %u \n",str_len);
                for(int p=0; p<str_len;p++){
                    //Serial.printf("insert to struct\n");
                    //Serial.write(*data);
                    *rawOutput=data_string[p];
                    rawOutput++;  
                    sizeDataReal++;
                }
                            
          }
          
          else {
             // Serial.printf("type %s, len %d \n",type,len);
              
              if(strcmp(type,"uint32_t")==0){
                  //Serial.println("type UINT32");
                  converter_uint32 converter;
                  converter.data=doc["data"][keyNya];
                  for(int p=0; p<len;p++){
                      *rawOutput=converter.array[p];
                      rawOutput++; 
                      sizeDataReal++;
                  }
              }
              else if(strcmp(type,"uint16_t")==0){
                 // Serial.println("type UNT16");
                  converter_uint16 converter;
                  converter.data=doc["data"][keyNya];
                  for(int p=0; p<len;p++){
                      *rawOutput=converter.array[p];
                      rawOutput++; 
                      sizeDataReal++;
                  }
              }
              else if(strcmp(type,"uint8_t")==0){
                  converter_uint8 converter;
                  converter.data=doc["data"][keyNya];
                  for(int p=0; p<len;p++){
                      *rawOutput=converter.array[p];
                      rawOutput++; 
                      sizeDataReal++;
                  }
              }
              else if(strcmp(type,"int8_t")==0){
                  converter_int8 converter;
                  converter.data=doc["data"][keyNya];
                  for(int p=0; p<len;p++){
                      *rawOutput=converter.array[p];
                      rawOutput++; 
                      sizeDataReal++;
                  }
              }
              else if(strcmp(type,"bool")==0){
                  converter_bool converter;
                  converter.data=doc["data"][keyNya];
                  for(int p=0; p<len;p++){
                      *rawOutput=converter.array[p];
                      rawOutput++; 
                      sizeDataReal++;
                  }
              }
              if(strcmp(type,"int32_t")==0){
                  converter_int32 converter;
                  converter.data=doc["data"][keyNya];
                  for(int p=0; p<len;p++){
                      *rawOutput=converter.array[p];
                      rawOutput++; 
                      sizeDataReal++;
                  }
              }
              else if(strcmp(type,"int16_t")==0){
                  //Serial.println("type INT16");
                  converter_int16 converter;
                  converter.data=doc["data"][keyNya];
                  for(int p=0; p<len;p++){
                      *rawOutput=converter.array[p];
                      rawOutput++; 
                      sizeDataReal++;
                  }
              }
              if(strcmp(type,"float")==0){
                  converter_float converter;
                  converter.data=doc["data"][keyNya];
                  for(int p=0; p<len;p++){
                      *rawOutput=converter.array[p];
                      rawOutput++; 
                      sizeDataReal++;
                  }
              }
              if(strcmp(type,"double")==0){
                  //Serial.println("type double");
                  converter_double converter;
                  converter.data=doc["data"][keyNya];
                  for(int p=0; p<len;p++){
                      *rawOutput=converter.array[p];
                      rawOutput++; 
                      sizeDataReal++;
                  }
              }                
              
          }
      }
  
      uint16_t totalSizeStruct=sizeof(commandStruct);
      uint16_t sizeDataTotal=sizeof(commandStruct.data);
      uint16_t headersize=totalSizeStruct-sizeDataTotal;
      commandStruct.len=headersize+sizeDataReal-2;
      if (setupStruct)
          return true;
      else{
          ESP_LOGE("","Tidak ada setup untuk mengencode json to byte array");
          return false;
      }
    }

    bool jsonToByteArray(const JsonDocument &doc, headerRadioStruct &commandStruct){
      char commandIdChar [10];
      sprintf(commandIdChar,"%u",commandStruct.commandId);
  
      if (commandStruct.type==COMMAND_HANDLER_TYPE_EXE){
          if(!exeIdDoc[commandIdChar]){
          ESP_LOGE("","tidak ada command id %d di exe doc",commandStruct.commandId);
          return false;
          }
      }
      else if(commandStruct.type==COMMAND_HANDLER_TYPE_DAT|| commandStruct.type==COMMAND_HANDLER_TYPE_DAT_WITHOUT_ACK){
          if(!dataIdDoc[commandIdChar]){
              ESP_LOGE("","tidak ada command id %d di data doc",commandStruct.commandId);
              return false;
              }
      }
      else{
          if(!setupIdDoc[commandIdChar]){
              ESP_LOGE("","tidak ada command id %d di setup doc",commandStruct.commandId);
              return false;
              }
      }
  
      JsonObject setupStruct; 
      if (commandStruct.type==COMMAND_HANDLER_TYPE_EXE)
          setupStruct = exeDoc[exeIdDoc[commandIdChar].as<const char*>()];
      else if(commandStruct.type==COMMAND_HANDLER_TYPE_DAT|| commandStruct.type==COMMAND_HANDLER_TYPE_DAT_WITHOUT_ACK)
          setupStruct = dataDoc[dataIdDoc[commandIdChar].as<const char*>()];
      else
          setupStruct = setupDoc[setupIdDoc[commandIdChar].as<const char*>()];
      // Loop through the expected keys in the setup JSON and validate the types
      JsonArray lens_json = setupStruct["data_len"].as<JsonArray>();
      JsonArray types_json = setupStruct["data_type"].as<JsonArray>();  
      JsonArray key_json = setupStruct["key"].as<JsonArray>();
      uint8_t output [1024];
      uint8_t* rawOutput = (uint8_t*)&commandStruct.data;
  
      uint16_t sizeDataReal=0;
      for (size_t i = 0; i < types_json.size(); i++) {
          const char * type = types_json[i];
          const char * keyNya=key_json[i];
          int len = lens_json[i];
          //Serial.printf("decoding loop %d, type %s, size %d, size_loop %d\n",i,type,len,types_json.size());
  
          if (strcmp(type,"string")==0) {
              uint8_t str_len=strlen(doc["data"][keyNya]);
              char data_string[255];
              strlcpy(data_string,doc["data"][keyNya]|"null",sizeof(data_string));
              data_string[str_len]='\0';
              *rawOutput=str_len;
              rawOutput++;
              sizeDataReal++;
              //Serial.println("-----------");
              
              if (str_len > len){
                //Serial.println("overflow.....");
                return false;
              }  
           //     Serial.printf("type string, len %u \n",str_len);
                for(int p=0; p<str_len;p++){
                    //Serial.printf("insert to struct\n");
                    //Serial.write(*data);
                    *rawOutput=data_string[p];
                    rawOutput++;  
                    sizeDataReal++;
                }
                            
          }
          
          else {
             // Serial.printf("type %s, len %d \n",type,len);
              
              if(strcmp(type,"uint32_t")==0){
                  //Serial.println("type UINT32");
                  converter_uint32 converter;
                  converter.data=doc["data"][keyNya];
                  for(int p=0; p<len;p++){
                      *rawOutput=converter.array[p];
                      rawOutput++; 
                      sizeDataReal++;
                  }
              }
              else if(strcmp(type,"uint16_t")==0){
                 // Serial.println("type UNT16");
                  converter_uint16 converter;
                  converter.data=doc["data"][keyNya];
                  for(int p=0; p<len;p++){
                      *rawOutput=converter.array[p];
                      rawOutput++; 
                      sizeDataReal++;
                  }
              }
              else if(strcmp(type,"uint8_t")==0){
                  converter_uint8 converter;
                  converter.data=doc["data"][keyNya];
                  for(int p=0; p<len;p++){
                      *rawOutput=converter.array[p];
                      rawOutput++; 
                      sizeDataReal++;
                  }
              }
              else if(strcmp(type,"int8_t")==0){
                  converter_int8 converter;
                  converter.data=doc["data"][keyNya];
                  for(int p=0; p<len;p++){
                      *rawOutput=converter.array[p];
                      rawOutput++; 
                      sizeDataReal++;
                  }
              }
              else if(strcmp(type,"bool")==0){
                  converter_bool converter;
                  converter.data=doc["data"][keyNya];
                  for(int p=0; p<len;p++){
                      *rawOutput=converter.array[p];
                      rawOutput++; 
                      sizeDataReal++;
                  }
              }
              if(strcmp(type,"int32_t")==0){
                  converter_int32 converter;
                  converter.data=doc["data"][keyNya];
                  for(int p=0; p<len;p++){
                      *rawOutput=converter.array[p];
                      rawOutput++; 
                      sizeDataReal++;
                  }
              }
              else if(strcmp(type,"int16_t")==0){
                  //Serial.println("type INT16");
                  converter_int16 converter;
                  converter.data=doc["data"][keyNya];
                  for(int p=0; p<len;p++){
                      *rawOutput=converter.array[p];
                      rawOutput++; 
                      sizeDataReal++;
                  }
              }
              if(strcmp(type,"float")==0){
                  converter_float converter;
                  converter.data=doc["data"][keyNya];
                  for(int p=0; p<len;p++){
                      *rawOutput=converter.array[p];
                      rawOutput++; 
                      sizeDataReal++;
                  }
              }
              if(strcmp(type,"double")==0){
                  //Serial.println("type double");
                  converter_double converter;
                  converter.data=doc["data"][keyNya];
                  for(int p=0; p<len;p++){
                      *rawOutput=converter.array[p];
                      rawOutput++; 
                      sizeDataReal++;
                  }
              }                
              
          }
      }
  
      uint16_t totalSizeStruct=sizeof(commandStruct);
      uint16_t sizeDataTotal=sizeof(commandStruct.data);
      uint16_t headersize=totalSizeStruct-sizeDataTotal;
      commandStruct.len=headersize+sizeDataReal-2;
  
      if (setupStruct)
          return true;
      else{
          ESP_LOGE("","Tidak ada setup untuk mengencode json to byte array");
          return false;
      }
    }

    bool typeIdToString(char * output, int id){
      switch (id)
      {
      case COMMAND_HANDLER_TYPE_SET:{
          strcpy(output,"set");
      }
          break;
      case COMMAND_HANDLER_TYPE_GET:{
          strcpy(output,"get");
      }
          break;
      case COMMAND_HANDLER_TYPE_DEL:{
          strcpy(output,"del");
      }
          break;
      case COMMAND_HANDLER_TYPE_EXE:{
          strcpy(output,"exe");
      }
          break;
      case COMMAND_HANDLER_TYPE_DAT:{
              strcpy(output,"dat");
          }
          break;
      case COMMAND_HANDLER_TYPE_DAT_WITHOUT_ACK:{
            strcpy(output,"datw");
      }    
          break;         
      case COMMAND_HANDLER_TYPE_RES:{
          strcpy(output,"res");
      }
          break;                                 
      default:
          return false;
      }
      return true;
    }

    uint8_t typeStringToId(const char * input){
      if(strcmp(input,"set")==0)
        return COMMAND_HANDLER_TYPE_SET;
      else if (strcmp(input,"get")==0)
          return COMMAND_HANDLER_TYPE_GET;
      else if (strcmp(input,"del")==0)
          return COMMAND_HANDLER_TYPE_DEL;    
      else if (strcmp(input,"exe")==0)
          return COMMAND_HANDLER_TYPE_EXE;
      else if (strcmp(input,"dat")==0)
          return COMMAND_HANDLER_TYPE_DAT;
      else if (strcmp(input,"datw")==0)
          return COMMAND_HANDLER_TYPE_DAT_WITHOUT_ACK;
      else if (strcmp(input,"res")==0)
          return COMMAND_HANDLER_TYPE_RES;                
      return  0;
    }

    template <typename T>
    bool byteArrayToData(uint8_t* data, uint8_t commandType, uint16_t commandId,  T& output){
      char commandIdChar [5];
      sprintf(commandIdChar,"%u",commandId);
  
          //Serial.printf("data id %u\n",commandId);
          if (commandType==COMMAND_HANDLER_TYPE_EXE){
              if(!exeIdDoc[commandIdChar]){
              ESP_LOGE("","tidak ada command id %d di exe doc",commandId);
              return false;
              }
          }
          else if(commandType==COMMAND_HANDLER_TYPE_DAT || commandType==COMMAND_HANDLER_TYPE_DAT_WITHOUT_ACK){
              if(!dataIdDoc[commandIdChar]){
                  ESP_LOGE("","tidak ada command id %d di data doc",commandId);
                  return false;
                  }
          }
          else{
              if(!setupIdDoc[commandIdChar]){
                  ESP_LOGE("","tidak ada command id %d di setup doc",commandId);
                  return false;
                  }
          }
  
  
          JsonObject setupStruct; 
          if (commandType==COMMAND_HANDLER_TYPE_EXE)
              setupStruct = exeDoc[exeIdDoc[commandIdChar].as<const char*>()];
          else if(commandType==COMMAND_HANDLER_TYPE_DAT|| commandType==COMMAND_HANDLER_TYPE_DAT_WITHOUT_ACK)
              setupStruct = dataDoc[dataIdDoc[commandIdChar].as<const char*>()];
          else
              setupStruct = setupDoc[setupIdDoc[commandIdChar].as<const char*>()];
      // Loop through the expected keys in the setup JSON and validate the types
      JsonArray lens_json = setupStruct["data_len"].as<JsonArray>();
      JsonArray types_json = setupStruct["data_type"].as<JsonArray>();  
  
      uint8_t* rawOutput = (uint8_t*)&output;
  
      for (size_t i = 0; i < types_json.size(); i++) {
          const char * type = types_json[i];
          int len = lens_json[i];
          //Serial.printf("decoding loop %d, type %s, size %d, size_loop %d\n",i,type,len,types_json.size());
  
          if (strcmp(type,"string")==0) {
              uint8_t str_len=*data;
              //Serial.print(*data);
              //Serial.println("-----------");
              data++;
              if (str_len > len){
                //Serial.println("overflow.....");
                return false;
              }  
              //Serial.printf("type string, len %u \n",str_len);
                for(int p=0; p<str_len;p++){
                    //Serial.printf("insert to struct\n");
                    //Serial.write(*data);
                    *rawOutput=*data;
                    data++;
                    rawOutput++;  
                }              
              int jump=len-str_len;
              *rawOutput='\0';
              rawOutput=rawOutput+jump;    
          }
          
          else {
              //Serial.printf("type %s, len %d \n",type,len);
              for(int p=0; p<len;p++){
                *rawOutput=*data;
                data++;
                rawOutput++; 
              }
          }
      }
      return true;
    }

    bool byteArrayToJson(uint8_t* data, JsonDocument &doc, uint8_t commandType, uint16_t commandId){
      char commandIdChar [5];
      sprintf(commandIdChar,"%u",commandId);
  
      //Serial.printf("data id %u\n",commandId);
      if (commandType==COMMAND_HANDLER_TYPE_EXE){
          if(!exeIdDoc[commandIdChar]){
          ESP_LOGE("","tidak ada command id %d di exe doc",commandId);
          return false;
          }
      }
      else if(commandType==COMMAND_HANDLER_TYPE_DAT|| commandType==COMMAND_HANDLER_TYPE_DAT_WITHOUT_ACK){
          if(!dataIdDoc[commandIdChar]){
              ESP_LOGE("","tidak ada command id %d di data doc",commandId);
              return false;
              }
      }
      else{
          if(!setupIdDoc[commandIdChar]){
              ESP_LOGE("","tidak ada command id %d di setup doc",commandId);
              return false;
              }
      }
  /*
      setupDoc;
      setupIdDoc;
      exeDoc;
      exeIdDoc;
  
  */
      JsonObject setupStruct; 
      if (commandType==COMMAND_HANDLER_TYPE_EXE)
          setupStruct = exeDoc[exeIdDoc[commandIdChar].as<const char*>()];
      else if(commandType==COMMAND_HANDLER_TYPE_DAT|| commandType==COMMAND_HANDLER_TYPE_DAT_WITHOUT_ACK)
          setupStruct = dataDoc[dataIdDoc[commandIdChar].as<const char*>()];
      else if(commandType==COMMAND_HANDLER_TYPE_RES){
        if(*data==0){  
            doc["error"]="error"; 
            doc["status"]="failed";
            doc["status_code"]=*data;
        }
        else if(*data==0){  
            doc["error"]="none"; 
            doc["status"]="processed";
            doc["status_code"]=*data;
        }
        return true;
      }        
      else
          setupStruct = setupDoc[setupIdDoc[commandIdChar].as<const char*>()];
      // Loop through the expected keys in the setup JSON and validate the types
      JsonArray lens_json = setupStruct["data_len"].as<JsonArray>();
      JsonArray types_json = setupStruct["data_type"].as<JsonArray>();  
      JsonArray key_json = setupStruct["key"].as<JsonArray>();
      //uint8_t output [1024];
      //uint8_t* rawOutput = (uint8_t*)&output;
  
      for (size_t i = 0; i < types_json.size(); i++) {
          const char * type = types_json[i];
          const char * keyNya=key_json[i];
          int len = lens_json[i];
          //Serial.printf("decoding loop %d, type %s, size %d, size_loop %d\n",i,type,len,types_json.size());
  
          if (strcmp(type,"string")==0) {
              uint8_t str_len=*data;
              char data_string[255];
              //Serial.print(*data);
              //Serial.println("-----------");
              data++;
              if (str_len > len){
                //Serial.println("overflow.....");
                return false;
              }  
           //     Serial.printf("type string, len %u \n",str_len);
                for(int p=0; p<str_len;p++){
                    //Serial.printf("insert to struct\n");
                    //Serial.write(*data);
                    //*rawOutput=*data;
                    data_string[p]=(char)*data;
                    data++;
                    //rawOutput++;  
                }
              data_string[str_len]='\0';              
              int jump=len-str_len;
              //*rawOutput='\0';
              //rawOutput=rawOutput+jump;   
              doc["data"][keyNya]=data_string; 
          }
          
          else {
             // Serial.printf("type %s, len %d \n",type,len);
              byte data_numeric [6];
              for(int p=0; p<len;p++){
                //*rawOutput=*data;
                data_numeric[p]=*data;
                data++;
                //rawOutput++; 
              }
              if(strcmp(type,"uint32_t")==0){
               //   Serial.println("type UINT32");
                  converter_uint32 uint32;
                  memcpy(uint32.array,data_numeric,len);
                  doc["data"][keyNya]=uint32.data;
              }
              else if(strcmp(type,"uint16_t")==0){
                 // Serial.println("type UNT16");
                  converter_uint16 uint16;
                  memcpy(uint16.array,data_numeric,len);
                  doc["data"][keyNya]=uint16.data;
              }
              else if(strcmp(type,"uint8_t")==0){
                  //Serial.println("type uint8_t");
                  doc["data"][keyNya]=(uint8_t)data_numeric[0];
              }
              else if(strcmp(type,"int8_t")==0){
                  //Serial.println("type int8_t");
                  doc["data"][keyNya]=(int8_t)data_numeric[0];
              }
              else if(strcmp(type,"bool")==0){
                  //Serial.println("type boolean");
                  doc["data"][keyNya]=(bool)data_numeric[0];
              }
              if(strcmp(type,"int32_t")==0){
                  //Serial.println("type INT32");
                  converter_int32 int32;
                  memcpy(int32.array,data_numeric,len);
                  doc["data"][keyNya]=int32.data;
              }
              else if(strcmp(type,"int16_t")==0){
                  //Serial.println("type INT16");
                  converter_int16 int16;
                  memcpy(int16.array,data_numeric,len);
                  doc["data"][keyNya]=int16.data;
              }
              if(strcmp(type,"float")==0){
                  //Serial.println("type float");
                  converter_float float_number;
                  memcpy(float_number.array,data_numeric,len);
                  doc["data"][keyNya]=float_number.data;
              }
              if(strcmp(type,"double")==0){
                  //Serial.println("type double");
                  converter_double double_number;
                  memcpy(double_number.array,data_numeric,len);
                  doc["data"][keyNya]=double_number.data;
              }                
          }
      }
      if (setupStruct)
          return true;
      else{
          ESP_LOGE("","Tidak ada setup untuk mendecode byte array to json");
          return false;
      }  
    }

        bool byteArrayToJsonNoCompress(uint8_t* data, JsonDocument &doc, uint8_t commandType, uint16_t commandId){
      char commandIdChar [5];
      sprintf(commandIdChar,"%u",commandId);
  
      //Serial.printf("data id %u\n",commandId);
      if (commandType==COMMAND_HANDLER_TYPE_EXE){
          if(!exeIdDoc[commandIdChar]){
          ESP_LOGE("","tidak ada command id %d di exe doc",commandId);
          return false;
          }
      }
      else if(commandType==COMMAND_HANDLER_TYPE_DAT|| commandType==COMMAND_HANDLER_TYPE_DAT_WITHOUT_ACK){
          if(!dataIdDoc[commandIdChar]){
              ESP_LOGE("","tidak ada command id %d di data doc",commandId);
              return false;
              }
      }
      else{
          if(!setupIdDoc[commandIdChar]){
              ESP_LOGE("","tidak ada command id %d di setup doc",commandId);
              return false;
              }
      }
  /*
      setupDoc;
      setupIdDoc;
      exeDoc;
      exeIdDoc;
  
  */
      JsonObject setupStruct; 
      if (commandType==COMMAND_HANDLER_TYPE_EXE)
          setupStruct = exeDoc[exeIdDoc[commandIdChar].as<const char*>()];
      else if(commandType==COMMAND_HANDLER_TYPE_DAT|| commandType==COMMAND_HANDLER_TYPE_DAT_WITHOUT_ACK)
          setupStruct = dataDoc[dataIdDoc[commandIdChar].as<const char*>()];
      else if(commandType==COMMAND_HANDLER_TYPE_RES){
        if(*data==0){  
            doc["error"]="error"; 
            doc["status"]="failed";
            doc["status_code"]=*data;
        }
        else if(*data==0){  
            doc["error"]="none"; 
            doc["status"]="processed";
            doc["status_code"]=*data;
        }
        return true;
      }        
      else
          setupStruct = setupDoc[setupIdDoc[commandIdChar].as<const char*>()];
      // Loop through the expected keys in the setup JSON and validate the types
      JsonArray lens_json = setupStruct["data_len"].as<JsonArray>();
      JsonArray types_json = setupStruct["data_type"].as<JsonArray>();  
      JsonArray key_json = setupStruct["key"].as<JsonArray>();
      //uint8_t output [1024];
      //uint8_t* rawOutput = (uint8_t*)&output;
  
      for (size_t i = 0; i < types_json.size(); i++) {
          const char * type = types_json[i];
          const char * keyNya=key_json[i];
          int len = lens_json[i];
          //Serial.printf("decoding loop %d, type %s, size %d, size_loop %d\n",i,type,len,types_json.size());
  
          if (strcmp(type,"string")==0) {
              char data_string[255];
                for(int p=0; p<len;p++){
                    data_string[p]=(char)*data;
                    data++;
                }  
              doc["data"][keyNya]=data_string; 
          }
          
          else {
             // Serial.printf("type %s, len %d \n",type,len);
              byte data_numeric [8];
              for(int p=0; p<len;p++){
                //*rawOutput=*data;
                data_numeric[p]=*data;
                data++;
                //rawOutput++; 
              }
              if(strcmp(type,"uint32_t")==0){
               //   Serial.println("type UINT32");
                  converter_uint32 uint32;
                  memcpy(uint32.array,data_numeric,len);
                  doc["data"][keyNya]=uint32.data;
              }
              else if(strcmp(type,"uint16_t")==0){
                 // Serial.println("type UNT16");
                  converter_uint16 uint16;
                  memcpy(uint16.array,data_numeric,len);
                  doc["data"][keyNya]=uint16.data;
              }
              else if(strcmp(type,"uint8_t")==0){
                  //Serial.println("type uint8_t");
                  doc["data"][keyNya]=(uint8_t)data_numeric[0];
              }
              else if(strcmp(type,"int8_t")==0){
                  //Serial.println("type int8_t");
                  doc["data"][keyNya]=(int8_t)data_numeric[0];
              }
              else if(strcmp(type,"bool")==0){
                  //Serial.println("type boolean");
                  doc["data"][keyNya]=(bool)data_numeric[0];
              }
              if(strcmp(type,"int32_t")==0){
                  //Serial.println("type INT32");
                  converter_int32 int32;
                  memcpy(int32.array,data_numeric,len);
                  doc["data"][keyNya]=int32.data;
              }
              else if(strcmp(type,"int16_t")==0){
                  //Serial.println("type INT16");
                  converter_int16 int16;
                  memcpy(int16.array,data_numeric,len);
                  doc["data"][keyNya]=int16.data;
              }
              if(strcmp(type,"float")==0){
                  //Serial.println("type float");
                  converter_float float_number;
                  memcpy(float_number.array,data_numeric,len);
                  doc["data"][keyNya]=float_number.data;
              }
              if(strcmp(type,"double")==0){
                  //Serial.println("type double");
                  converter_double double_number;
                  memcpy(double_number.array,data_numeric,len);
                  doc["data"][keyNya]=double_number.data;
              }                
          }
      }
      if (setupStruct)
          return true;
      else{
          ESP_LOGE("","Tidak ada setup untuk mendecode byte array to json");
          return false;
      }  
    }


  public:
    // Constructor with default values
    CommandHandler(fs::FS &fileSystem);

    CommandHandler(fs::FS &fileSystem, SpiRamAllocator &allocatorInput);
    // Begin method to load stored settings from files
    bool begin();


    // Get data size of a command by its name
    size_t getDataSize(const char* commandName, uint8_t commandType);

    uint32_t getCommandId(const char* commandName, uint8_t commandType);

    bool validateFormat(JsonDocument &doc);
    
    fs::FS& getFileSystem();

    //create dir /folder_saya
    bool createDir(const char* dirPath);
    
    // return json array list nama file yang ada di sistem contoh secara asc /folder_saya
    bool listFile(const char *path, JsonDocument &doc, bool sortAsc=true);

    bool writeFile(const char* path,JsonDocument &doc);
    
    bool appendFile(const char* path,JsonDocument &doc);

    bool readFile(const char* path,JsonDocument &doc);

    bool deleteFile(const char* path);

    bool loadSettings(const char* commandName, JsonDocument &doc);

    bool loadSettings(const char* commandName);
    
    void notify(const char* commandName, bool status=true);
    // Method to process incoming JSON commands
    bool processCommand(const char* jsonData, char* statusResponse, size_t responseSize=1024);

    bool processCommand(const char* jsonData, JsonDocument &response, size_t responseSize=1024);
    
    bool processCommand(JsonDocument &commandDoc, JsonDocument &response);

    bool processCommand(JsonDocument &commandDoc);

    bool processCommand(headerRadioStruct &radioStruct, JsonDocument *docResult = nullptr);

    bool processCommand(headerCommandStruct &commandStruct, JsonDocument *docResult = nullptr);

    template <typename T>
    bool processCommand(const char * commandName,uint8_t command_hander_type,const T& data,uint16_t key=0){
        headerCommandStruct header=this->dataToStructCommand(commandName,command_hander_type,data,key);
        CREATE_JSON_DOCUMENT(doc);
        if(!this->structCommandToJson(doc,header))
            return false;
        return this->processCommand(doc);
    }

    void registerCommand(const char* commandName, CallbackFunc func);

    template <typename T>
    bool structCommandToData(headerRadioStruct &structCommand, const T& data){
      return byteArrayToData(structCommand.data,structCommand.type,structCommand.commandId,data);
    }

    template <typename T>
    bool structCommandToData(headerCommandStruct &structCommand, const T& data){
      return byteArrayToData(structCommand.data,structCommand.type,structCommand.commandId,data);
    }

    template <typename T>
    headerCommandStruct dataToStructCommand(const char* commandName, uint8_t commandType, const T& data,uint32_t message_id=0, uint16_t key=0){
      headerCommandStruct CommandStruct;
      if (commandType==COMMAND_HANDLER_TYPE_EXE){
          CommandStruct.commandId=exeDoc[commandName]["commandId"];
      }
      else if (commandType==COMMAND_HANDLER_TYPE_DAT|| commandType==COMMAND_HANDLER_TYPE_DAT_WITHOUT_ACK){
          CommandStruct.commandId=dataDoc[commandName]["commandId"];
      }
      else{
          CommandStruct.commandId=setupDoc[commandName]["commandId"];
      }
      //sudah
      message_id==0?CommandStruct.id_message=esp_random():CommandStruct.id_message=message_id;
      CommandStruct.key=key;
      CommandStruct.type=commandType;
      std::vector<uint8_t> encodedData=structToByteArray(data,CommandStruct.type,CommandStruct.commandId);
      uint16_t totalSizeStruct=sizeof(CommandStruct);
      uint16_t sizeDataTotal=sizeof(CommandStruct.data);
      uint16_t sizeActualData=encodedData.size();
      CommandStruct.len=(totalSizeStruct-sizeDataTotal)+sizeActualData-2;
      for (int i=0 ;i<encodedData.size();i++) {
          CommandStruct.data[i]=encodedData[i];
      }
      return CommandStruct;
    }

    template <typename T>    
    headerRadioStruct dataToStructCommand(uint32_t to, const char* commandName, uint8_t commandType, const T& data, uint32_t message_id=0,  uint32_t from=0, uint16_t key=0){
      headerRadioStruct CommandStruct;
      if(from==0){
            uint8_t mac[6];
            char mac_str[18];
            // Baca MAC address Bluetooth
            esp_err_t ret = esp_read_mac(mac, ESP_MAC_BT);
            uint32_t high     = mac[4]|mac[5]<<8&0xFFFFFFFF;
            uint32_t low    = mac[0]|mac[1]<<8|mac[2]<<16|mac[3]<<24;        
            CommandStruct.from=low+high;    
      }
      else
          CommandStruct.from=from;
      
      CommandStruct.to=to;
      if (commandType==COMMAND_HANDLER_TYPE_EXE){
          CommandStruct.commandId=exeDoc[commandName]["commandId"];
      }
      else if (commandType==COMMAND_HANDLER_TYPE_DAT|| commandType==COMMAND_HANDLER_TYPE_DAT_WITHOUT_ACK){
          CommandStruct.commandId=dataDoc[commandName]["commandId"];
      }
      else{
          CommandStruct.commandId=setupDoc[commandName]["commandId"];
      }
  
  
      message_id==0?CommandStruct.id_message=esp_random():CommandStruct.id_message=message_id;
      CommandStruct.key=key;
      CommandStruct.type=commandType;
      std::vector<uint8_t> encodedData=structToByteArray(data,CommandStruct.type,CommandStruct.commandId);
      uint16_t totalSizeStruct=sizeof(CommandStruct);
      uint16_t sizeDataTotal=sizeof(CommandStruct.data);
      uint16_t sizeActualData=encodedData.size();
      CommandStruct.len=(totalSizeStruct-sizeDataTotal)+sizeActualData-2;
      for (int i=0 ;i<encodedData.size();i++) {
          CommandStruct.data[i]=encodedData[i];
      }
      return CommandStruct;
    }

    bool structCommandToJson(JsonDocument &doc,headerRadioStruct &radioStruct){
      doc["to"]=radioStruct.to;
      doc["from"]=radioStruct.from;
      doc["id"]=radioStruct.id_message;
      char type[10];
      if(!typeIdToString(type,radioStruct.type))
          return false;
      doc["type"]=type;
      doc["key"]=radioStruct.key;
      char commandIdChar [10];
      sprintf(commandIdChar,"%u",radioStruct.commandId);
      if(radioStruct.type==COMMAND_HANDLER_TYPE_EXE)
          doc["command"]=exeIdDoc[commandIdChar].as<const char*>();
      else if(radioStruct.type==COMMAND_HANDLER_TYPE_DAT|| radioStruct.type==COMMAND_HANDLER_TYPE_DAT_WITHOUT_ACK)
          doc["command"]=dataIdDoc[commandIdChar].as<const char*>();        
      else if(radioStruct.type==COMMAND_HANDLER_TYPE_RES){
        doc["status_send"]=radioStruct.data[0];
        return true;
      }
      else
          doc["command"]=setupIdDoc[commandIdChar].as<const char*>();

      bool result=byteArrayToJson(radioStruct.data,doc,radioStruct.type,radioStruct.commandId);      
      if(result){
           //Validate recorded_at (in json it should be datetime timestamp while in struct it will be epoch)
            if (doc["data"]["recorded_at"].is<uint32_t>()){// there is key recorded_at and in epoch because it get from struct
                DateTime recorded_at(doc["data"]["recorded_at"].as<uint32_t>());  
                //change to datetime timestamp
                doc["data"]["recorded_at"]=recorded_at.timestamp(); 
            } 
      }
      return result;
    }

    bool structCommandToJson(JsonDocument &doc, headerCommandStruct &commandStruct){
      doc["id"]=commandStruct.id_message;
      char type[10];
      if(!typeIdToString(type,commandStruct.type))
          return false;
      doc["type"]=type;
      doc["key"]=commandStruct.key;
      char commandIdChar [8];
      //printf("converting struct to json command Id %d, command type %d=======================\n",commandStruct.commandId, commandStruct.type);
      sprintf(commandIdChar,"%u",commandStruct.commandId);
        if(commandStruct.type==COMMAND_HANDLER_TYPE_EXE)
          doc["command"]=exeIdDoc[commandIdChar].as<const char*>();
        else if(commandStruct.type==COMMAND_HANDLER_TYPE_DAT|| commandStruct.type==COMMAND_HANDLER_TYPE_DAT_WITHOUT_ACK)
          doc["command"]=dataIdDoc[commandIdChar].as<const char*>();        
        else if(commandStruct.type==COMMAND_HANDLER_TYPE_RES){
            doc["status_send"]=commandStruct.data[0];
            return true;
          }
        else
          doc["command"]=setupIdDoc[commandIdChar].as<const char*>();
      
        bool result=byteArrayToJson(commandStruct.data,doc,commandStruct.type,commandStruct.commandId);      
        if(result){
           //Validate recorded_at (in json it should be datetime timestamp while in struct it will be epoch)
            if (doc["data"]["recorded_at"].is<uint32_t>()){// there is key recorded_at and in epoch because it get from struct
                DateTime recorded_at(doc["data"]["recorded_at"].as<uint32_t>());  
                //change to datetime timestamp
                doc["data"]["recorded_at"]=recorded_at.timestamp(); 
            } 
        }
        return result;
    }

    bool structDataToJson(JsonDocument &doc, uint8_t* data, const char* commandName){
      uint16_t commandId=0;
      if(!dataDoc[commandName].as<JsonVariant>())
        return false; //tidak ada command name yang sesuai
    
      commandId=dataDoc[commandName]["commandId"].as<uint16_t>();
      bool result=byteArrayToJsonNoCompress(data,doc,COMMAND_HANDLER_TYPE_DAT,commandId);      
      if(result){
           //Validate recorded_at (in json it should be datetime timestamp while in struct it will be epoch)
            if (doc["data"]["recorded_at"].is<uint32_t>()){// there is key recorded_at and in epoch because it get from struct
                DateTime recorded_at(doc["data"]["recorded_at"].as<uint32_t>());  
                //change to datetime timestamp
                doc["data"]["recorded_at"]=recorded_at.timestamp(); 
            } 
      }
      return result;
    }
    
    bool jsonToStructCommand(const JsonDocument &doc, headerCommandStruct &commandStruct){
      CREATE_JSON_DOCUMENT(checkDoc);
      checkDoc=doc;
      
      //Validate recorded_at (in json it will be datetime timestamp while in struct it will be epoch)
      if (checkDoc["data"]["recorded_at"].is<const char*>()){// there is key recorded_at
        DateTime recorded_at(checkDoc["data"]["recorded_at"].as<const char*>());  
        //change to epoch
        checkDoc["data"]["recorded_at"]=recorded_at.unixtime(); 
      }

      if(validateFormat(checkDoc)==false){
          ESP_LOGE("","format json tidak valid");
          return false;
      }
      
      commandStruct.id_message=doc["id"]|0;
      commandStruct.type=typeStringToId(doc["type"].as<const char*>());
      commandStruct.key=doc["key"]|0;
      //Serial.printf("---------------------- %s command, command type %d\n",doc["command"].as<const char*>(),commandStruct.type);
      //Serial.print("commandID");
      //Serial.println(setupDoc[doc["command"].as<const char*>()]["commandId"].as<uint32_t>());
      if(commandStruct.type==COMMAND_HANDLER_TYPE_EXE)
          commandStruct.commandId=exeDoc[doc["command"].as<const char*>()]["commandId"].as<uint16_t>();
      else if(commandStruct.type==COMMAND_HANDLER_TYPE_DAT|| commandStruct.type==COMMAND_HANDLER_TYPE_DAT_WITHOUT_ACK)
          commandStruct.commandId=dataDoc[doc["command"].as<const char*>()]["commandId"].as<uint16_t>();        
      else
          commandStruct.commandId=setupDoc[doc["command"].as<const char*>()]["commandId"].as<uint16_t>();
      //Serial.print("commandID");
      //Serial.println(commandStruct.commandId);
      return jsonToByteArray(checkDoc,commandStruct);
    }

    bool jsonToStructCommand(const JsonDocument &doc, headerRadioStruct &commandStruct){
      CREATE_JSON_DOCUMENT(checkDoc);
      checkDoc=doc;
    
      //Validate recorded_at (in json it will be datetime timestamp while in struct it will be epoch)
      if (checkDoc["data"]["recorded_at"].is<const char*>()){// there is key recorded_at
        DateTime recorded_at(checkDoc["data"]["recorded_at"].as<const char*>());  
        //change to epoch
        checkDoc["data"]["recorded_at"]=recorded_at.unixtime(); 
      }

      if(validateFormat(checkDoc)==false){
          ESP_LOGE("","format json tidak valid");
          return false;
      }
      commandStruct.to=doc["to"]|0;
      commandStruct.from=doc["from"]|getID32();
      commandStruct.id_message=doc["id"];
      commandStruct.type=typeStringToId(doc["type"]);
      commandStruct.key=doc["key"]|0;
  
  
      if(commandStruct.type==COMMAND_HANDLER_TYPE_EXE)
          commandStruct.commandId=exeDoc[doc["command"].as<const char*>()]["commandId"].as<uint16_t>();
      else if(commandStruct.type==COMMAND_HANDLER_TYPE_DAT|| commandStruct.type==COMMAND_HANDLER_TYPE_DAT_WITHOUT_ACK)
          commandStruct.commandId=dataDoc[doc["command"].as<const char*>()]["commandId"].as<uint16_t>();        
      else
          commandStruct.commandId=setupDoc[doc["command"].as<const char*>()]["commandId"].as<uint16_t>();
  
      return jsonToByteArray(checkDoc,commandStruct);
    }

    
};


#endif