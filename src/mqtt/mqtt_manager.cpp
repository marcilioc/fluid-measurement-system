#include "mqtt_manager.h"
#include <ArduinoJson.h>

// Static pointer for callback (needed for PubSubClient)
MqttManager* MqttManager_instance = nullptr;

MqttManager::MqttManager(WiFiClient* client) : wifi_client(client) {
    mqtt_client = new PubSubClient(*client);
    MqttManager_instance = this; // For static callback
}

MqttManager::~MqttManager() {
    if (mqtt_client) {
        delete mqtt_client;
    }
}

bool MqttManager::begin(const char* broker, int port, const char* client_id_str) {
    broker_address = String(broker);
    broker_port = port;
    client_id = String(client_id_str);
    
    Serial.printf("[MQTT] Initializing MQTT Manager...\n");
    Serial.printf("[MQTT] Broker: %s:%d\n", broker, port);
    Serial.printf("[MQTT] Client ID: %s\n", client_id_str);
    
    mqtt_client->setServer(broker, port);
    mqtt_client->setBufferSize(1024); // Increase buffer size
    mqtt_client->setCallback(staticCallback);
    
    // Try initial connection
    return reconnect();
}

bool MqttManager::isConnected() {
    return mqtt_client && mqtt_client->connected();
}

void MqttManager::loop() {
    if (!isConnected()) {
        reconnect();
    } else {
        mqtt_client->loop();
        processQueue();
    }
}

bool MqttManager::publish(const String& topic, const String& payload, uint8_t qos, bool retain) {
    if (publish_queue.size() >= MAX_QUEUE_SIZE) {
        Serial.printf("[MQTT] Queue full! Dropping message: %s\n", topic.c_str());
        messages_failed++;
        return false;
    }
    
    MqttMessage msg(topic, payload, qos, retain);
    publish_queue.push(msg);
    
    Serial.printf("[MQTT] Queued: %s = %s (queue size: %d)\n", 
                  topic.c_str(), payload.c_str(), publish_queue.size());
    
    return true;
}

bool MqttManager::publishImmediate(const String& topic, const String& payload) {
    if (!isConnected()) {
        Serial.println("[MQTT] Not connected, cannot publish immediately");
        return false;
    }
    
    bool success = mqtt_client->publish(topic.c_str(), payload.c_str());
    if (success) {
        messages_sent++;
        Serial.printf("[MQTT] Published immediately: %s = %s\n", topic.c_str(), payload.c_str());
    } else {
        messages_failed++;
        Serial.printf("[MQTT] Failed to publish: %s\n", topic.c_str());
    }
    
    return success;
}

void MqttManager::processQueue() {
    if (publish_queue.empty()) return;
    
    unsigned long current_time = millis();
    if (current_time - last_publish < PUBLISH_INTERVAL) return;
    
    MqttMessage msg = publish_queue.front();
    publish_queue.pop();
    
    // Check message age (30 second timeout)
    if (current_time - msg.timestamp > 30000) {
        Serial.printf("[MQTT] Message expired: %s\n", msg.topic.c_str());
        messages_failed++;
        return;
    }
    
    if (publishImmediate(msg.topic, msg.payload)) {
        last_publish = current_time;
    } else {
        // Re-queue if failed and not too old
        if (current_time - msg.timestamp < 10000) { // 10 second retry window
            publish_queue.push(msg);
        } else {
            messages_failed++;
        }
    }
}

bool MqttManager::reconnect() {
    unsigned long current_time = millis();
    if (current_time - last_reconnect_attempt < RECONNECT_INTERVAL) {
        return false; // Too soon to retry
    }
    
    last_reconnect_attempt = current_time;
    reconnect_attempts++;
    
    Serial.printf("[MQTT] Attempting reconnection #%lu...\n", reconnect_attempts);
    
    if (mqtt_client->connect(client_id.c_str())) {
        Serial.println("[MQTT] Connected successfully!");
        
        // Re-subscribe to topics (you'll need to track subscriptions)
        // For now, let the main code handle re-subscription
        
        return true;
    } else {
        Serial.printf("[MQTT] Connection failed, rc=%d\n", mqtt_client->state());
        return false;
    }
}

bool MqttManager::subscribe(const String& topic, uint8_t qos) {
    if (!isConnected()) {
        Serial.printf("[MQTT] Cannot subscribe to %s - not connected\n", topic.c_str());
        return false;
    }
    
    bool success = mqtt_client->subscribe(topic.c_str(), qos);
    if (success) {
        Serial.printf("[MQTT] Subscribed to: %s (QoS %d)\n", topic.c_str(), qos);
    } else {
        Serial.printf("[MQTT] Failed to subscribe to: %s\n", topic.c_str());
    }
    
    return success;
}

void MqttManager::setCallback(std::function<void(char*, uint8_t*, unsigned int)> callback) {
    message_callback = callback;
}

void MqttManager::staticCallback(char* topic, uint8_t* payload, unsigned int length) {
    if (MqttManager_instance && MqttManager_instance->message_callback) {
        MqttManager_instance->message_callback(topic, payload, length);
    }
}

// === CONVENIENCE METHODS ===
bool MqttManager::publishWeight(const String& scale_id, float weight) {
    String topic = "smfm/" + scale_id + "/measurement/weight";
    String payload = String(weight, 3); // 3 decimal places
    return publishImmediate(topic, payload);
}

bool MqttManager::publishStatus(const String& scale_id, int status) {
    String topic = "smfm/" + scale_id + "/operation/status";
    String payload = String(status);
    return publishImmediate(topic, payload);
}

bool MqttManager::publishLog(const String& message) {
    String topic = "smfm/log";
    return publish(topic, message);
}

bool MqttManager::publishDiagnostics() {
    StaticJsonDocument<256> doc;
    doc["uptime"] = millis();
    doc["free_heap"] = ESP.getFreeHeap();
    doc["wifi_rssi"] = WiFi.RSSI();
    doc["mqtt_queue_size"] = getQueueSize();
    doc["messages_sent"] = getMessagesSent();
    doc["messages_failed"] = getMessagesFailed();
    doc["reconnect_attempts"] = getReconnectAttempts();
    
    String payload;
    serializeJson(doc, payload);
    
    return publish("smfm/system/diagnostics", payload);
}

// === ESTATÍSTICAS DETALHADAS ===
void MqttManager::printStats() {
    Serial.println("\n=== MQTT MANAGER STATS ===");
    Serial.printf("Connected: %s\n", MqttManager::isConnected() ? "YES" : "NO");
    Serial.printf("Queue size: %d/%d\n", MqttManager::getQueueSize(), MAX_QUEUE_SIZE);
    Serial.printf("Messages sent: %lu\n", MqttManager::getMessagesSent());
    Serial.printf("Messages failed: %lu\n", MqttManager::getMessagesFailed());
    Serial.printf("Reconnect attempts: %lu\n", MqttManager::getReconnectAttempts());
    Serial.printf("Success rate: %.1f%%\n", 
                    getMessagesSent() > 0 ? 
                    (float)getMessagesSent() * 100.0 / (getMessagesSent() + getMessagesFailed()) : 0.0);
    Serial.println("========================\n");
}