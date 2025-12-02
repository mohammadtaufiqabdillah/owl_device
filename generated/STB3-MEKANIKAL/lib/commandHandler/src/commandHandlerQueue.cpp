#include "CommandHandler.h"

SemaphoreHandle_t mutex_handler;

// Queue Initializations
{{QUEUE_DECLARATION}}

// Initialize the queue
void CommandHandler::queueBegin(){
    mutex_handler = xSemaphoreCreateRecursiveMutex();
    {{QUEUE_INITIALIZATIONS}}
}

// Send settings to the specified RTOS queue
void CommandHandler::sendToQueue(const char* queueName, bool status) {
    JsonDocument doc;
    bool newValues = status;

    {{QUEUE_SEND_BLOCKS}}
}

// Load struct overload function
{{LOAD_STRUCT_FUNCTIONS}}