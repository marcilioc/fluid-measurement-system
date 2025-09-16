#include "command_handler.h"
#include "config.h"

void CommandHandler::init() {
    // System Outputs
    pinMode(SLND1, OUTPUT);
    pinMode(SLND2, OUTPUT);
    pinMode(PUMP1, OUTPUT);
    pinMode(PUMP2, OUTPUT);
    
    // Estado inicial
    digitalWrite(SLND1, HIGH);
    digitalWrite(SLND2, HIGH);
    digitalWrite(PUMP1, HIGH);
    digitalWrite(PUMP2, HIGH);
    
    Serial.println("Command Handler initialized.");
}

void CommandHandler::handleCommand(String topic, String payload) {
    Serial.println("Processing command: " + topic + " -> " + payload);

    // Extract command from topic (commands/pump1, commands/slnd1, etc.)
    String command = topic.substring(topic.lastIndexOf('/') + 1);
    if (command == "slnd1") {
        handleSlnd1Command(payload);
    }
    else if (command == "slnd2") {
        handleSlnd2Command(payload);
    }
    else if (command == "pump2") {
        handlePump2Command(payload);
    }
    if (command == "pump1") {
        handlePump1Command(payload);
    }
    else if (command == "config") {
        handleConfigCommand(payload);
    }
    else if (command == "start") {
        handleStartCommand(payload);
    }
    else {
        Serial.println("Command not recognized: " + command);
    }
}

void CommandHandler::handlePump1Command(String payload) {
    if (payload == "1") {
        digitalWrite(PUMP1, LOW); // Enables the pump (LOW for active relay)
        Serial.println("Pump 1 enabled");
    }
    else if (payload == "0") {
        digitalWrite(PUMP1, HIGH); // Disables the pump
        Serial.println("Pump 1 disabled");
    }
}

void CommandHandler::handlePump2Command(String payload) {
    if (payload == "1") {
        digitalWrite(PUMP2, LOW); // Enables the pump (LOW for active relay)
        Serial.println("Pump 2 enabled");
    }
    else if (payload == "0") {
        digitalWrite(PUMP2, HIGH); // Disables the pump
        Serial.println("Pump 2 disabled");
    }
}

void CommandHandler::handleSlnd1Command(String payload) {
    if (payload == "1") {
        digitalWrite(SLND1, LOW); // Enables the solenoid (LOW for active relay)
        Serial.println("Solenoid 1 enabled");
    }
    else if (payload == "0") {
        digitalWrite(SLND1, HIGH); // Disables the solenoid
        Serial.println("Solenoid 1 disabled");
    }
}

void CommandHandler::handleSlnd2Command(String payload) {
    if (payload == "1") {
        digitalWrite(SLND2, LOW); // Enables the solenoid (LOW for active relay)
        Serial.println("Solenoid 2 enabled");
    }
    else if (payload == "0") {
        digitalWrite(SLND2, HIGH); // Disables the solenoid
        Serial.println("Solenoid 2 disabled");
    }
}

void CommandHandler::handleStartCommand(String payload) {
    if (payload == "1") {
        op_started = true;
        Serial.println("Operation started.");
    }
    else if (payload == "0") {
        op_started = false;
        Serial.println("Operation stopped.");
    }
}

void CommandHandler::handleConfigCommand(String payload) {
    // Example: "reset" to restart the system
    if (payload == "reset") {
        Serial.println("Restarting the system...");
        ESP.restart();
    } else {
        Serial.println("Configuration command not recognized:" + payload);
    }
}
