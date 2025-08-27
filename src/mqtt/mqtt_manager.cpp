#include "mqtt_manager.h"
#include "config.h"
#include <ArduinoJson.h>

MqttManager* MqttManager::instance = nullptr;

MqttManager::MqttManager() : client(wifiClient) {
    instance = this;
}

void MqttManager::init() {
    client.setServer(MQTT_BROKER, MQTT_PORT);
    client.setCallback(messageCallback);
    reconnect();
}

void MqttManager::reconnect() {
    while (!client.connected()) {
        Serial.print("Tentando conexão MQTT...");
        
        if (client.connect(MQTT_CLIENT_ID)) {
            Serial.println("Conectado!");
            // Reconnect to topics

            publishStatus("online");
        } else {
            Serial.print("Falha, rc=");
            Serial.print(client.state());
            Serial.println(" Tentando novamente em 5 segundos");
            delay(5000);
        }
    }
}

void MqttManager::loop() {
    if (!client.connected()) {
        reconnect();
    }
    client.loop();
}

void MqttManager::messageCallback(char* topic, byte* payload, unsigned int length) {
    if (instance) {
        String topicStr = String(topic);
        String payloadStr = "";
        
        for (int i = 0; i < length; i++) {
            payloadStr += (char)payload[i];
        }
        
        instance->handleMessage(topicStr, payloadStr);
    }
}

void MqttManager::handleMessage(String topic, String payload) {
    Serial.println("Mensagem recebida [" + topic + "]: " + payload);
    
    if (commandCallback) {
        commandCallback(topic, payload);
    }
}


// void MqttManager::publishSensorData(const SensorData& data) {
//     DynamicJsonDocument doc(200);
//     doc["temperatura"] = data.temperature;
//     doc["umidade"] = data.humidity;
//     doc["timestamp"] = millis();
//     doc["device_id"] = MQTT_CLIENT_ID;
    
//     String jsonString;
//     serializeJson(doc, jsonString);
    
//     bool success = client.publish(TOPIC_SENSORS, jsonString.c_str());
//     if (success) {
//         Serial.println("Dados enviados: " + jsonString);
//     } else {
//         Serial.println("Falha ao enviar dados");
//     }
// }

void MqttManager::publishImmediate(String topic, String payload) {
    client.publish(topic.c_str(), payload.c_str());
}

void MqttManager::publishStatus(String status) {
    client.publish(STATUS_TOPIC, status.c_str());
}

void MqttManager::setCommandCallback(std::function<void(String, String)> callback) {
    this->commandCallback = callback;
}
