#include "command_handler.h"
#include "Config.h"

void CommandHandler::init() {
    pinMode(SLND1, OUTPUT);
    pinMode(SLND2, OUTPUT);
    
    // Estado inicial
    digitalWrite(SLND1, HIGH);
    digitalWrite(SLND2, HIGH);
    
    Serial.println("Command Handler inicializado");
}

void CommandHandler::handleCommand(String topic, String payload) {
    Serial.println("Processando comando: " + topic + " -> " + payload);
    
    // Parse do JSON
    DynamicJsonDocument doc(200);
    DeserializationError error = deserializeJson(doc, payload);
    
    if (error) {
        Serial.println("Erro ao fazer parse do JSON");
        return;
    }
    
    // Extrair comando do tópico (comandos/led, comandos/relay, etc.)
    String command = topic.substring(topic.lastIndexOf('/') + 1);
    
    if (command == "led") {
        handleLedCommand(payload);
    }
    else if (command == "relay") {
        handleRelayCommand(payload);
    }
    else if (command == "config") {
        handleConfigCommand(payload);
    }
    else {
        Serial.println("Comando não reconhecido: " + command);
    }
}

void CommandHandler::handleLedCommand(String payload) {
    DynamicJsonDocument doc(100);
    deserializeJson(doc, payload);
    
    String action = doc["action"];
    
    if (action == "on") {
        digitalWrite(LED_PIN, HIGH);
        Serial.println("LED ligado");
    }
    else if (action == "off") {
        digitalWrite(LED_PIN, LOW);
        Serial.println("LED desligado");
    }
    else if (action == "toggle") {
        digitalWrite(LED_PIN, !digitalRead(LED_PIN));
        Serial.println("LED alternado");
    }
}

void CommandHandler::handleRelayCommand(String payload) {
    DynamicJsonDocument doc(100);
    deserializeJson(doc, payload);
    
    bool state = doc["state"];
    digitalWrite(RELAY_PIN, state ? HIGH : LOW);
    
    Serial.println("Relay " + String(state ? "ligado" : "desligado"));
}

void CommandHandler::handleConfigCommand(String payload) {
    DynamicJsonDocument doc(200);
    deserializeJson(doc, payload);
    
    if (doc.containsKey("sensor_interval")) {
        // Aqui você pode alterar configurações em tempo de execução
        Serial.println("Nova configuração recebida");
    }
}