#include "command_handler.h"
#include "app_globals.h"
#include "config.h"
#include <string.h>

void CommandHandler::init() {
    // System Outputs
    pinMode(SLND1, OUTPUT);
    pinMode(SLND2, OUTPUT);
    pinMode(PUMP1, OUTPUT);
    pinMode(PUMP2, OUTPUT);
    pinMode(PURGE_LED, OUTPUT);
    pinMode(E1_COND_LED, OUTPUT);
    pinMode(E2_COND_LED, OUTPUT);
    
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
    std::string s_topic = topic.c_str();    
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
    else if (command == "stop") {
        handleStartCommand(payload);
    }
    else if (command == "tare_s1") {
        scale1.tare();
        Serial.println("Scale S1 tared.");
    }
    else if (command == "tare_s2") {
            scale2.tare();
            Serial.println("Scale S2 tared.");
    }
    else if (command == "set_factor_s1"){
        double factor = payload.toFloat();
        scale1.set_calibration_factor(factor);
        Serial.println("Scale S1 calibration factor set to " + String(factor));
    } 
    else if (command == "set_factor_s2") {
        float factor = payload.toFloat();
        scale2.set_calibration_factor(factor);
        Serial.println("Scale S2 calibration factor set to " + String(factor));
    }
    else if (command == "get_factor"){
        if (s_topic.find("s1")) {
            float factor = scale1.get_calibration_factor();
            Serial.println("Scale S1 calibration factor: " + String(factor));
        } else if (s_topic.find("s2")) {
            float factor = scale2.get_calibration_factor();
            Serial.println("Scale S2 calibration factor: " + String(factor));
        }
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
