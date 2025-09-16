#include "mqtt_manager.h"
#include "config.h"

MqttManager* MqttManager::instance = nullptr;

MqttManager::MqttManager() : client(wifiClient) {
    instance = this;
}

void MqttManager::init() {
    pinMode(EMBD_LED, OUTPUT);
    digitalWrite(EMBD_LED, LOW); // LED off initially
    client.setServer(MQTT_BROKER, MQTT_PORT);
    client.setCallback(messageCallback);
    reconnect();
}

void MqttManager::reconnect() {
    while (!client.connected()) {
        Serial.print("Trying MQTT connection...");
        
        if (client.connect(MQTT_CLIENT_ID)) {
            Serial.println("Connected!");
            digitalWrite(EMBD_LED, HIGH); // Indicates successful connection
            // Reconnect to topics
            bool success = client.subscribe(SUB_TOPIC);
            if (success) {
                Serial.println("Subscribed to: " + String(SUB_TOPIC));
            } else {
                Serial.println("Failed to subscribing to: " + String(SUB_TOPIC));
            }

            publishStatus("online");
        } else {
            digitalWrite(EMBD_LED, LOW); // Indicates failed connection
            Serial.print("Failure, rc=");
            Serial.print(client.state());
            Serial.println(" Retrying in 5 seconds");
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
    Serial.println("Message received [" + topic + "]: " + payload);
    
    if (commandCallback) {
        commandCallback(topic, payload);
    }
}

void MqttManager::publishImmediate(String topic, String payload) {
    client.publish(topic.c_str(), payload.c_str());
}

void MqttManager::publishStatus(String status) {
    client.publish(STATUS_TOPIC, status.c_str());
}

void MqttManager::setCommandCallback(std::function<void(String, String)> callback) {
    this->commandCallback = callback;
    Serial.println("Command callback set.");
}
