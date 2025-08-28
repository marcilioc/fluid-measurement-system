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
    
    Serial.println("Command Handler inicializado");
}

void CommandHandler::handleCommand(String topic, String payload) {
    Serial.println("Processando comando: " + topic + " -> " + payload);

    // Extrair comando do tópico (comandos/led, comandos/relay, etc.)
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
    else {
        Serial.println("Comando não reconhecido: " + command);
    }
}

void CommandHandler::handlePump1Command(String payload) {
    if (payload == "1") {
        digitalWrite(PUMP1, LOW); // Ativa a bomba (LOW para relé ativo)
        Serial.println("Bomba 1 ligada");
    }
    else if (payload == "0") {
        digitalWrite(PUMP1, HIGH); // Desativa a bomba
        Serial.println("Bomba 1 desligada");
    }
}

void CommandHandler::handlePump2Command(String payload) {
    if (payload == "1") {
        digitalWrite(PUMP2, LOW); // Ativa a bomba (LOW para relé ativo)
        Serial.println("Bomba 2 ligada");
    }
    else if (payload == "0") {
        digitalWrite(PUMP2, HIGH); // Desativa a bomba
        Serial.println("Bomba 2 desligada");
    }
}

void CommandHandler::handleSlnd1Command(String payload) {
    if (payload == "1") {
        digitalWrite(SLND1, LOW); // Ativa o solenóide (LOW para relé ativo)
        Serial.println("Solenoide 1 ativado");
    }
    else if (payload == "0") {
        digitalWrite(SLND1, HIGH); // Desativa o solenóide
        Serial.println("Solenoide 1 desativado");
    }
}

void CommandHandler::handleSlnd2Command(String payload) {
    if (payload == "1") {
        digitalWrite(SLND2, LOW); // Ativa o solenóide (LOW para relé ativo)
        Serial.println("Solenoide 2 ativado");
    }
    else if (payload == "0") {
        digitalWrite(SLND2, HIGH); // Desativa o solenóide
        Serial.println("Solenoide 2 desativado");
    }
}

void CommandHandler::handleConfigCommand(String payload) {
    // Exemplo simples: payload "reset" para reiniciar o sistema
    if (payload == "reset") {
        Serial.println("Reiniciando sistema...");
        ESP.restart();
    } else {
        Serial.println("Comando de configuração não reconhecido: " + payload);
    }
}
