#ifndef COMMAND_HANDLER_H
#define COMMAND_HANDLER_H

#include <Arduino.h>
#include <ArduinoJson.h>

class CommandHandler {
private:
    void handleLedCommand(String payload);
    void handleRelayCommand(String payload);
    void handleConfigCommand(String payload);
    
public:
    void init();
    void handleCommand(String topic, String payload);
};

#endif