#ifndef COMMAND_HANDLER_H
#define COMMAND_HANDLER_H

#include <Arduino.h>

class CommandHandler {
private:
    void handlePump1Command(String payload);
    void handlePump2Command(String payload);
    void handleSlnd1Command(String payload);
    void handleSlnd2Command(String payload);
    void handleConfigCommand(String payload);
    void handleStartCommand(String payload);
    
public:
    void init();
    void handleCommand(String topic, String payload);
};

#endif