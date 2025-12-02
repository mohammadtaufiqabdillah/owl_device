#include "CommandHandler.h"  

void CommandHandler::registerCommand(const char* commandName, CallbackFunc func) {
    commandMap[std::string(commandName)] = func;
}

bool CommandHandler::executeCommand(const char* commandName, JsonDocument &doc) {
    std::unordered_map<std::string, CallbackFunc>::iterator it = commandMap.find(std::string(commandName));
    
    if (it != commandMap.end()) {
        it->second(doc);
        return true;
    } 
    
    else {
        doc["status"]="error";
        doc["error"]="command not registered";
        return false;
    }
}

bool CommandHandler::processCommandExecutter(JsonDocument &doc){
    const char* commandName = doc["command"];
    //ESP_LOGI("","command handler executter %s",commandName);
    // If the command is not present in the setup JSON, return an error
    if (!commandName) {
        doc["status"]="error";
        doc["error"]="no command key exist";
        return false;
    }

    JsonObject commandSetup = exeDoc[commandName];
    //serializeJsonPretty(exeDoc,Serial);

    if (!commandName || commandSetup.isNull()) {
            doc["status"]="error";
            doc["error"]="unsupported command";
            return false;
    }
    
    if(!validateDataType(exeDoc,doc))
        return false;
        
    // EXECUTER---------
    executeCommand(commandName,doc);

    return true;
}