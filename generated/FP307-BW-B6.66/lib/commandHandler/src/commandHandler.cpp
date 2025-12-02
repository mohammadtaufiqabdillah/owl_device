    #include <commandHandler.h>
    #include <setupExeJson.h>
    #include <commandIdExeJson.h>
    #include <setupSetJson.h>
    #include <commandIdSetJson.h>
    #include <setupDatJson.h>
    #include <commandIdDatJson.h>
    // Constructor with default values

    static const char* TAG = "commandHandler.cpp";

    bool CommandHandler::getAccess(){
        if(xSemaphoreTakeRecursive(mutex_handler,2000/portTICK_PERIOD_MS)==pdTRUE)
            return true;
        else{
            ESP_LOGE(TAG,"failed get mutex handler");
            return false;
        }
    }

    bool CommandHandler::releaseAccess(bool status){
        xSemaphoreGiveRecursive(mutex_handler);
        return status;
    }

    void CommandHandler::merge(JsonVariant dst, JsonVariantConst src)
    {
    if (src.is<JsonObjectConst>())
    {
        for (JsonPairConst kvp : src.as<JsonObjectConst>())
        {
        if (dst[kvp.key()]) 
            merge(dst[kvp.key()], kvp.value());
        else
            dst[kvp.key()] = kvp.value();
        }
    }
    else
    {
        dst.set(src);
    }
    }

    CommandHandler::CommandHandler(fs::FS &fileSystemI) : fileSystem(fileSystemI) {
    }

    CommandHandler::CommandHandler(fs::FS &fileSystemI,SpiRamAllocator &allocatorInput) : fileSystem(fileSystemI),setupDoc(&allocatorInput),setupIdDoc(&allocatorInput),exeDoc(&allocatorInput),exeIdDoc(&allocatorInput),dataDoc(&allocatorInput),dataIdDoc(&allocatorInput) {
    }

    //Load Setting with return
    void CommandHandler::notify(const char* commandName, bool status) {
        const char* queueName = setupDoc[commandName]["queue"];
        if (queueName) {
                    sendToQueue(queueName,status);  // Hypothetical send function to RTOS queue
                }        
    }

    //Load Setting with return for internal
    bool CommandHandler::loadSettingsInternal(const char* commandName, JsonDocument &doc) {
        const char* path = setupDoc[commandName]["path"];

        CREATE_JSON_DOCUMENT(docValue);
        
        if (fileSystem.exists(path)) {
            File file = fileSystem.open(path, "r");
            if (!file) {
                doc["status"]="error";
                doc["error"]="failed to open file";
                return false;
            }

            // Read and print file content
            DeserializationError error = deserializeJson(docValue, file);
            if (error) {
                doc["status"]="error";
                doc["error"]="failed to parse file";
                return false;
            } else {
                ESP_LOGD(TAG,"Settings loaded for %s: ", commandName);
                const char* queueName = setupDoc[commandName]["queue"];

                file.close();
                doc["status"]="success";
                doc["error"]="none";
                doc["data"]=docValue.as<JsonObject>();
                if (queueName) {
                    //sendToQueue(queueName);  // Hypothetical send function to RTOS queue
                }
                return true;
            }
        } else {
            ESP_LOGW(TAG,"Settings file not found: %s\n", path);
            doc["status"]="error";
            doc["error"]="file not exist";
            return false;
        }
    }

    //Load Setting with return
    bool CommandHandler::loadSettings(const char* commandName, JsonDocument &doc) {
        if(!getAccess()){
            doc["status"]="error";
            doc["error"]="failed to get access please retry";
            return false;
        }

        const char* path = setupDoc[commandName]["path"];

        CREATE_JSON_DOCUMENT(docValue);
        
        if (fileSystem.exists(path)) {
            File file = fileSystem.open(path, "r");
            if (!file) {
                doc["status"]="error";
                doc["error"]="failed to open file";
                return releaseAccess(false);
            }

            // Read and print file content
            DeserializationError error = deserializeJson(docValue, file);
            if (error) {
                doc["status"]="error";
                doc["error"]="failed to parse file";
                return releaseAccess(false);
            } else {
                ESP_LOGD(TAG,"Settings loaded for %s: ", commandName);
                const char* queueName = setupDoc[commandName]["queue"];
                file.close();
                doc["status"]="success";
                doc["error"]="none";
                doc["data"]=docValue.as<JsonObject>();
                if (queueName) {
                    //sendToQueue(queueName);  // Hypothetical send function to RTOS queue
                }
                return releaseAccess(true);
            }
        } else {
            ESP_LOGW(TAG,"Settings file not found: %s\n", path);
            doc["status"]="error";
            doc["error"]="file not exist";
            return releaseAccess(false);
        }
    }

    //Load Setting without return but send to queue
    bool CommandHandler::loadSettings(const char* commandName) {
        if(!getAccess()){
            return false;
        }

        const char* path = setupDoc[commandName]["path"];
        if (fileSystem.exists(path)) {
            sendToQueue(setupDoc[commandName]["queue"]);
            return releaseAccess(true);
            }

        else
            return releaseAccess(false);
    }

    // Helper method to save settings into a file
    bool CommandHandler::saveSettings(const char* commandName, JsonDocument& doc) {
            const char* path = setupDoc[commandName]["path"];
            File file = fileSystem.open(path, "w");
            if (!file) {
                doc["status"]="error";
                doc["error"]="failed to open file";
                return false;
            }

            // Read and print file content
            if(serializeJson(doc["data"],file)==0){
                doc["status"]="error";
                doc["error"]="failed to write file";
                file.close();
                return false;
            }
            else {
                ESP_LOGD(TAG,"Settings saved for %s: \n", commandName);
                const char* queueName = setupDoc[commandName]["queue"];
                if (queueName) {
                    sendToQueue(queueName);  // Hypothetical send function to RTOS queue
                }
                file.close();
                doc["status"]="success";
                doc["error"]="none";
                if(strcmp(commandName,"key")==0){// If the "key" changed refresh the variabel (apply)
                    setup_key=doc["data"]["key"];
                }
                if(strcmp(commandName,"use_key")==0){// If the "use_key" changed refresh the variabel (apply)
                    useKey=doc["data"]["state"];
                }
                return true;
            }
    }

    // Helper function to check if the key is correct
    bool CommandHandler::checkKey(uint32_t key) {
        if(useKey){
            uint32_t low     = ESP.getEfuseMac() & 0xFFFFFFFF; 
            uint32_t high    = ( ESP.getEfuseMac() >> 32 ) % 0xFFFFFFFF;
            uint16_t id=low+high;
            return (key == setup_key||key == id);
        }
            
        else
            return true;
    }

    // Begin method to load stored settings from files
    bool CommandHandler::begin() {
        this->queueBegin();
        
        this->createDir(DATA_DIRECTORY);
       
        DeserializationError error = deserializeJson(setupDoc,setupSetJson);
        if (error) {
            ESP_LOGE(TAG,"failed deserializeJson Setup Json");
            return false;
        }

        error = deserializeJson(setupIdDoc,commandIdSetJson);
        if (error) {
            ESP_LOGE(TAG,"failed deserializeJson Setup ID Json");
            return false;
        }          
        
        error = deserializeJson(exeDoc,setupExeJson);
        if (error) {
            ESP_LOGE(TAG,"failed deserializeJson Exe Json");
            return false;
        }
        
        error = deserializeJson(exeIdDoc,commandIdExeJson);
        if (error) {
            ESP_LOGE(TAG,"failed deserializeJson Exe ID Json");
            return false;
        }

        error = deserializeJson(dataDoc,setupDatJson);
        if (error) {
            ESP_LOGE(TAG,"failed deserializeJson Dat Json");
            return false;
        }
        
        error = deserializeJson(dataIdDoc,commandIdDatJson);
        if (error) {
            ESP_LOGE(TAG,"failed deserializeJson Dat ID Json");
            return false;
        }        
            
        if(true){
        CREATE_JSON_DOCUMENT(doc);
        if(loadSettingsInternal("key",doc))
            setup_key=doc["data"]["key"];
        }
        if(true){
        CREATE_JSON_DOCUMENT(doc);
        if(loadSettingsInternal("use_key",doc))
            useKey=doc["data"]["state"];            
        }

        return true;    
    }

    // Method to process incoming JSON commands return char array
    bool CommandHandler::processCommand(const char* jsonData, char* statusResponse, size_t responseSize) {

        CREATE_JSON_DOCUMENT(commandDoc);

        DeserializationError error = deserializeJson(commandDoc, jsonData);
        
        CREATE_JSON_DOCUMENT(responseDoc);
        
        if (error) {
            commandDoc["status"]="error";
            commandDoc["error"]="failed to parse json";
            serializeJson(commandDoc,statusResponse,responseSize);
            return false;
        }
        
        bool status=this->processCommand(commandDoc,responseDoc);
        serializeJson(responseDoc,statusResponse,responseSize);
        return status;
    }

    // Method to process incoming JSON commands return json object
    bool CommandHandler::processCommand(const char* jsonData, JsonDocument &response, size_t responseSize) {
        
        CREATE_JSON_DOCUMENT(commandDoc);
        DeserializationError error = deserializeJson(commandDoc, jsonData);

        if (error) {
            commandDoc["status"]="error";
            commandDoc["error"]="failed to parse json";
            merge(response,commandDoc);
            return false;
        }

        return this->processCommand(commandDoc,response);
    }

    //Assume no failed parsing
    bool CommandHandler::processCommand(JsonDocument &commandDoc, JsonDocument &response) {
        if (commandDoc.isNull() || commandDoc.size() == 0) {
            commandDoc["status"]="error";
            commandDoc["error"]="JsonDocument Invalid or empty";
            merge(response,commandDoc);
            ESP_LOGE(TAG,"Invalid JsonDocument: Document is empty or null.\n");
            return false;
        }

        if(!getAccess()){
                commandDoc["status"]="error";
                commandDoc["error"]="failed to get access please retry";
                merge(response,commandDoc);
                return false;
        }
        //validate key
        if(!checkKey(commandDoc["key"])){
            commandDoc["status"]="error";
            commandDoc["error"]="invalid or unauthorized command";
            merge(response,commandDoc);
            return releaseAccess(false);
        }

        // Extract the command type
        const char* commandType = commandDoc["type"];
        if(!commandType){
            commandDoc["status"]="error";
            commandDoc["error"]="invalid or unauthorized command";
            merge(response,commandDoc);
            return releaseAccess(false);
        }
        //setter
        if(strcmp(commandType,"set")==0){
            bool status=processCommandSetter(commandDoc);
            commandDoc.remove("data");
            merge(response,commandDoc);
            return releaseAccess(status);
        }

        //getter
        else if (strcmp(commandType,"get")==0){
            bool status=processCommandGetter(commandDoc);
            merge(response,commandDoc);
            return releaseAccess(status);
        }

        //getter
        else if (strcmp(commandType,"del")==0){
            bool status=processCommandDeleter(commandDoc);
            merge(response,commandDoc);
            return releaseAccess(status);
        }        

        //custom command
        else if (strcmp(commandType,"exe")==0){
            bool status=processCommandExecutter(commandDoc);
            //commandDoc.remove("data");
            merge(response,commandDoc);
            return releaseAccess(status);
        }
        else{
            commandDoc["status"]="error";
            commandDoc["error"]="unknown command type";
            merge(response,commandDoc);
            return releaseAccess(false);
        }
    }

    //Assume no failed parsing
    bool CommandHandler::processCommand(JsonDocument &commandDoc) {
        if (commandDoc.isNull() || commandDoc.size() == 0) {
            commandDoc["status"]="error";
            commandDoc["error"]="JsonDocument Invalid or empty";
            ESP_LOGE(TAG,"Invalid JsonDocument: Document is empty or null.\n");
            return false;
        }

        if(!getAccess()){
                commandDoc["status"]="error";
                commandDoc["error"]="failed to get access please retry";
                return false;
        }
        //validate key
        if(!checkKey(commandDoc["key"])){
            commandDoc["status"]="error";
            commandDoc["error"]="invalid or unauthorized command";
            return releaseAccess(false);
        }

        // Extract the command type
        const char* commandType = commandDoc["type"];
        
        //setter
        if(strcmp(commandType,"set")==0){
            bool status=processCommandSetter(commandDoc);
            commandDoc.remove("data");
            return releaseAccess(status);
        }

        //getter
        else if (strcmp(commandType,"get")==0){
            bool status=processCommandGetter(commandDoc);
            return releaseAccess(status);
        }

        //getter
        else if (strcmp(commandType,"del")==0){
            bool status=processCommandDeleter(commandDoc);
            return releaseAccess(status);
        }        

        //custom command
        else if (strcmp(commandType,"exe")==0){
            bool status=processCommandExecutter(commandDoc);
            //commandDoc.remove("data");
            return releaseAccess(status);
        }
        else{
            commandDoc["status"]="error";
            commandDoc["error"]="unknown command type";
            return releaseAccess(false);
        }
    }

    bool CommandHandler::processCommand(headerRadioStruct &radioStruct, JsonDocument *docResult){
                    if (docResult == nullptr) {
                CREATE_JSON_DOCUMENT(doc);
                this->structCommandToJson(doc,radioStruct);
                return processCommand(doc);
            } else {
                // Gunakan dokumen yang sudah ada
                this->structCommandToJson(*docResult,radioStruct);
                return processCommand(*docResult);
            }
    }

    size_t CommandHandler::getDataSize(const char* commandName, uint8_t commandType){
        JsonObject commandSetup;
        switch (commandType)
        {
        case COMMAND_HANDLER_TYPE_SET:
        case COMMAND_HANDLER_TYPE_GET:
        case COMMAND_HANDLER_TYPE_DEL:
            commandSetup=setupDoc[commandName];
            break;
        
        case COMMAND_HANDLER_TYPE_EXE:
            commandSetup=exeDoc[commandName];
            break;

        case COMMAND_HANDLER_TYPE_DAT:
        case COMMAND_HANDLER_TYPE_DAT_WITHOUT_ACK:
            commandSetup=dataDoc[commandName];
        default:
            break;
        }
        if(!commandSetup){
            ESP_LOGE(TAG,"command %s not found in setup json",commandName);
            return 0;
        }
        // Loop through the expected keys in the setup JSON and validate the types
        JsonArray lens = commandSetup["data_len"].as<JsonArray>();
        uint32_t totalSize=0;
        for (size_t i = 0; i < lens.size(); i++) {
            totalSize+=lens[i].as<uint32_t>();
            }
        return totalSize;
    }

    uint32_t CommandHandler::getCommandId(const char* commandName, uint8_t commandType){
        JsonObject commandSetup;
        switch (commandType)
        {
        case COMMAND_HANDLER_TYPE_SET:
        case COMMAND_HANDLER_TYPE_GET:
        case COMMAND_HANDLER_TYPE_DEL:
            commandSetup=setupDoc[commandName];
            break;
        
        case COMMAND_HANDLER_TYPE_EXE:
            commandSetup=exeDoc[commandName];
            break;

        case COMMAND_HANDLER_TYPE_DAT:
        case COMMAND_HANDLER_TYPE_DAT_WITHOUT_ACK:
            commandSetup=dataDoc[commandName];
        default:
            break;
        }
        if(!commandSetup){
            ESP_LOGE(TAG,"command %s not found in setup json",commandName);
            return 0;
        }
        
        return commandSetup["commandId"].as<uint32_t>();
    }
    
    bool CommandHandler::processCommand(headerCommandStruct &commandStruct, JsonDocument *docResult){
            if (docResult == nullptr) {
                CREATE_JSON_DOCUMENT(doc);
                this->structCommandToJson(doc,commandStruct);
                return processCommand(doc);
            } else {
                // Gunakan dokumen yang sudah ada
                this->structCommandToJson(*docResult,commandStruct);
                return processCommand(*docResult);
            }
    }

    bool CommandHandler::validateDataType(JsonDocument &setupDoc, JsonDocument &doc){
        const char * commandName=doc["command"];
        JsonObject commandSetup = setupDoc[commandName];
        // Loop through the expected keys in the setup JSON and validate the types
        JsonArray keys = commandSetup["key"].as<JsonArray>();
        JsonArray types = commandSetup["data_type"].as<JsonArray>();
        
        JsonObject isKeyExists = doc["data"];
        for (size_t i = 0; i < keys.size(); i++) {
            const char* expectedKey = keys[i];
            const char* expectedType = types[i];

            // Access the key directly and check if it is valid
            JsonVariant value = doc["data"][expectedKey];

            if (!doc["data"][expectedKey].is<JsonVariant>()) {
                char error [100];
                snprintf(error, sizeof(error), "missing key: %s", expectedKey);
                doc["status"]="error";
                doc["error"]=error;
                return false;
            }

            // Validate the data type against the expected type
            if (
                (strcmp(expectedType, "string") == 0 && !value.is<const char*>()) ||
                (strcmp(expectedType, "uint8_t") == 0 && !value.is<uint8_t>())|| 
                (strcmp(expectedType, "uint16_t") == 0 && !value.is<uint16_t>())|| 
                (strcmp(expectedType, "uint32_t") == 0 && !value.is<uint32_t>())|| 
                (strcmp(expectedType, "int8_t") == 0 && !value.is<int8_t>())|| 
                (strcmp(expectedType, "int16_t") == 0 && !value.is<int16_t>())|| 
                (strcmp(expectedType, "int32_t") == 0 && !value.is<int32_t>())||
                (strcmp(expectedType, "bool") == 0 && !value.is<bool>())||
                (strcmp(expectedType, "float") == 0 && !value.is<float>())||
                (strcmp(expectedType, "double") == 0 && !value.is<double>())||
                (strcmp(expectedType, "object") == 0 && !value.is<JsonObject>())
                ) {
                char error [100];
                snprintf(error, sizeof(error), "invalid type data for key: %s", expectedKey);
                doc["status"]="error";
                doc["error"]=error;
                return false;
                
            }
        }
        return true;
        
    }

    bool CommandHandler::validateFormat(JsonDocument &doc){
        if (doc.isNull() || doc.size() == 0) {
            doc["status"]="error";
            doc["error"]="JsonDocument Invalid or empty";
            ESP_LOGE(TAG,"Invalid JsonDocument: Document is empty or null.\n");
            return false;
        }

        JsonVariant id_value = doc["id"];
        if(!id_value.is<uint32_t>()){
            doc["status"]="error";
            doc["error"]="missing or invalid id";
            return false;
        }

        if(!getAccess()){
                doc["status"]="error";
                doc["error"]="failed to get access please retry";
                return false;
        }
        //validate key
        if(!checkKey(doc["key"])){
            doc["status"]="error";
            doc["error"]="invalid or unauthorized command";
            return releaseAccess(false);
        }

        // Extract the command type
        const char* commandType = doc["type"];
        const char* commandName = doc["command"];
        //setter
        if(strcmp(commandType,"set")==0){
            return releaseAccess(validateDataType(setupDoc,doc));
        }

        //getter
        else if (strcmp(commandType,"get")==0){
            JsonObject commandSetup = setupDoc[commandName];
            if (!commandName || commandSetup.isNull()) {
                doc["status"]="error";
                doc["error"]="unsupported command";
                return releaseAccess(false);
            }
            return releaseAccess(true);
        }

        //getter
        else if (strcmp(commandType,"del")==0){
            JsonObject commandSetup = setupDoc[commandName];
            if (!commandName || commandSetup.isNull()) {
                doc["status"]="error";
                doc["error"]="unsupported command";
                return releaseAccess(false);
            }
            return releaseAccess(true);
        }        

        //custom command
        else if (strcmp(commandType,"exe")==0){
            return releaseAccess(validateDataType(exeDoc,doc));
        }

        else if (strcmp(commandType,"dat")==0){
            return releaseAccess(validateDataType(dataDoc,doc));
        }

        else if (strcmp(commandType,"datw")==0){
            return releaseAccess(validateDataType(dataDoc,doc));
        }

        else if (strcmp(commandType,"res")==0){
            return releaseAccess(validateDataType(dataDoc,doc));
        }

        else{
            doc["status"]="error";
            doc["error"]="unknown command type";
            return releaseAccess(false);
        }

        doc["status"]="error";
        doc["error"]="unknown error";
        return releaseAccess(false);
    }

    bool CommandHandler::processCommandSetter(JsonDocument &doc){
        // Extract the command name directly and check if it exists in the setup
        const char* commandName = doc["command"];
        
        // If the command is not present in the setup JSON, return an error
        JsonObject commandSetup = setupDoc[commandName];
        if (!commandName || commandSetup.isNull()) {
            doc["status"]="error";
            doc["error"]="unsupported command";
            return false;
        }

        if(!validateDataType(setupDoc,doc))
            return false;
        return saveSettings(commandName,doc);
    }

    bool CommandHandler::processCommandGetter(JsonDocument &doc){
        const char* commandName = doc["command"];
        
        // If the command is not present in the setup JSON, return an error
        JsonObject commandSetup = setupDoc[commandName];
        if (!commandName || commandSetup.isNull()) {
            doc["status"]="error";
            doc["error"]="unsupported command";
            return false;
        }
        return loadSettingsInternal(commandName,doc);
    }

    bool CommandHandler::processCommandDeleter(JsonDocument &doc){
        const char* commandName = doc["command"];
        
        // If the command is not present in the setup JSON, return an error
        JsonObject commandSetup = setupDoc[commandName];
        if (!commandName || commandSetup.isNull()) {
            doc["status"]="error";
            doc["error"]="unsupported command";
            return false;
        }
        const char* path = setupDoc[commandName]["path"];
        bool status_del=fileSystem.remove(path);
        if(status_del){
            doc["status"]="success";
            doc["error"]="none";
            return true;
        }
        else {
            doc["status"]="error";
            doc["error"]="failed delete";
            return false;
        }
        return false;
    }

    fs::FS& CommandHandler::getFileSystem() {
        return fileSystem;
    }

    bool createDirectoryRecursive(fs::FS &fs, const char *path) {
        // Jika path sudah ada dan merupakan direktori, return true
        if (fs.exists(path)) {
            File file = fs.open(path);
            if (file.isDirectory()) {
                file.close();
                return true;
            }
            file.close();
            return false;  // Path ada tapi bukan direktori
        }

        char *pp = strdup(path);  // Duplikat string path
        if (!pp) return false;    // Gagal alokasi memori

        char *p = pp;
        if (*p == '/') p++;  // Lewati root jika ada

        while (*p != '\0') {
            if (*p == '/') {
                *p = '\0';  // Sementara potong string
                
                // Buat direktori jika belum ada
                if (!fs.exists(pp)) {
                    if (!fs.mkdir(pp)) {
                        free(pp);
                        return false;
                    }
                }
                *p = '/';  // Kembalikan slash
            }
            p++;
        }

        // Buat direktori terakhir (full path)
        bool success = fs.mkdir(path);
        free(pp);
        return success;
    }

    bool CommandHandler::createDir(const char* dirPath) {
        if(!getAccess()){
            ESP_LOGE(TAG,"failed to get access");
            return false;
        }
        if (fileSystem.exists(dirPath)) {
            ESP_LOGW(TAG,"Directory already exists: %s", dirPath);
            return releaseAccess(true);
        }
        
        if (createDirectoryRecursive(fileSystem,dirPath)) {
            ESP_LOGI(TAG,"Directory created successfully: %s", dirPath);
            return releaseAccess(true);
        } else {
            ESP_LOGE(TAG,"Failed to create directory: %s", dirPath);
            return releaseAccess(false);
        }
    }
    
    bool CommandHandler::listFile(const char *path, JsonDocument &doc, bool sortAsc) {
        if(!getAccess()){
            ESP_LOGE(TAG,"failed to get access");
            return false;
        }
        // Buka direktori yang diminta
        File dir = fileSystem.open(path);
        
        
        // Cek jika path tidak valid atau bukan direktori
        if (!dir || !dir.isDirectory()) {
            if(dir) dir.close();
            ESP_LOGE(TAG,"Invalid path or not a directory: %s", path);
            return releaseAccess(false);
        }

        // Koleksi sementara untuk sorting
        std::vector<String> fileList;

        File file = dir.openNextFile();
        while (file) {
            if (!file.isDirectory()) {
            const char *fullPath = file.name();
            const char *baseName = strrchr(fullPath, '/');
            fileList.push_back(baseName ? baseName + 1 : fullPath);
            }
            file = dir.openNextFile();
        }
        dir.close();

        // Sorting vector
        if(sortAsc) {
            std::sort(fileList.begin(), fileList.end(), [](const String& a, const String& b) {
            return a < b;
            });
        } else {
            std::sort(fileList.begin(), fileList.end(), [](const String& a, const String& b) {
            return a > b;
            });
        }

        // Konversi ke JSON array
        JsonArray array = doc.to<JsonArray>();
        for(const auto& name : fileList) {
            array.add(name);
        }

        return releaseAccess(!fileList.empty());
    }

    bool CommandHandler::readFile(const char *path, JsonDocument &doc) {
        if(!getAccess()){
            ESP_LOGE(TAG,"failed to get access");
            return false;
        }
   
        File file=fileSystem.open(path,FILE_READ);
        
        if(!file){
            ESP_LOGE(TAG,"Failed to open file %s for writing", path);
            return releaseAccess(false);
        }
        
        DeserializationError err=deserializeJson(doc,file);
        file.close();
        if(err) {
            ESP_LOGE(TAG,"Failed to read file %s, error: %s", path, err.c_str());
            return releaseAccess(false);
        } else {
            ESP_LOGI(TAG,"File %s read successfully", path);
            return releaseAccess(true);
        }
    }

    bool CommandHandler::writeFile(const char *path, JsonDocument &doc) {
        if(!getAccess()){
            ESP_LOGE(TAG,"failed to get access");
            return false;
        }
        
        File file=fileSystem.open(path,FILE_WRITE);
        if(!file){
            ESP_LOGE(TAG,"Failed to open file %s for writing", path);
            return releaseAccess(false);
        }

        if(serializeJson(doc,file)==0){
            ESP_LOGE(TAG,"Failed to write to file %s", path);
            file.close();
            return releaseAccess(false);
        } else {
            file.close();
            ESP_LOGI(TAG,"File %s written successfully", path);
            return releaseAccess(true);
        }
    }

    bool CommandHandler::appendFile(const char *path, JsonDocument &doc) {
        if(!getAccess()){
            ESP_LOGE(TAG,"failed to get access");
            return false;
        }
        
        File file=fileSystem.open(path,FILE_APPEND);
        if(!file){
            ESP_LOGE(TAG,"Failed to open file %s for appending", path);
            return releaseAccess(false);
        }

        if(serializeJson(doc,file)==0){
            ESP_LOGE(TAG,"Failed to append to file %s", path);
            file.close();
            return releaseAccess(false);
        } else {
            file.close();
            ESP_LOGI(TAG,"File %s appended successfully", path);
            return releaseAccess(true);
        }
    }

    bool CommandHandler::deleteFile(const char *path) {
        if(!getAccess()){
            ESP_LOGE(TAG,"failed to get access");
            return false;
        }
        
        if (fileSystem.remove(path)) {
            ESP_LOGI(TAG,"File %s deleted successfully", path);
            return releaseAccess(true);
        } else {
            ESP_LOGE(TAG,"Failed to delete file %s", path);
            return releaseAccess(false);
        }
    }