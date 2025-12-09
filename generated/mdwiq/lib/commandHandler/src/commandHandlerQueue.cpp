#include "CommandHandler.h"

SemaphoreHandle_t mutex_handler;

// Queue Initializations


// Initialize the queue
void CommandHandler::queueBegin(){
    mutex_handler = xSemaphoreCreateRecursiveMutex();
    
}

// Send settings to the specified RTOS queue
void CommandHandler::sendToQueue(const char* queueName, bool status) {
    JsonDocument doc;
    bool newValues = status;

    
}

// Load struct overload function
