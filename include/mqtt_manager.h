#ifndef MQTT_MANAGER_H
#define MQTT_MANAGER_H

#include <PubSubClient.h>
#include <WiFi.h>
#include <queue>
#include <functional>

struct MqttMessage {
    String topic;
    String payload;
    uint8_t qos = 0;
    bool retain = false;
    unsigned long timestamp;
    
    MqttMessage(const String& t, const String& p, uint8_t q = 0, bool r = false) 
        : topic(t), payload(p), qos(q), retain(r), timestamp(millis()) {}
};

class MqttManager {
private:
    PubSubClient* mqtt_client;
    WiFiClient* wifi_client;
    
    // Queue system
    std::queue<MqttMessage> publish_queue;
    static const size_t MAX_QUEUE_SIZE = 50;
    
    // Timing control
    unsigned long last_publish = 0;
    unsigned long last_reconnect_attempt = 0;
    static const unsigned long PUBLISH_INTERVAL = 100;    // 100ms between publishes
    static const unsigned long RECONNECT_INTERVAL = 5000; // 5s between reconnect attempts
    
    // Connection info
    String broker_address;
    int broker_port;
    String client_id;
    
    // Statistics
    unsigned long messages_sent = 0;
    unsigned long messages_failed = 0;
    unsigned long reconnect_attempts = 0;
    
    // Callback
    std::function<void(char*, uint8_t*, unsigned int)> message_callback;
    
public:
    MqttManager(WiFiClient* client);
    ~MqttManager();
    
    // === SETUP AND CONNECTION ===
    bool begin(const char* broker, int port, const char* client_id);
    bool isConnected();
    void loop();
    void printStats();
    
    // === PUBLISHING (WITH QUEUE) ===
    bool publish(const String& topic, const String& payload, uint8_t qos = 0, bool retain = false);
    bool publishImmediate(const String& topic, const String& payload);
    
    // === SUBSCRIPTION MANAGEMENT ===
    bool subscribe(const String& topic, uint8_t qos = 0);
    bool unsubscribe(const String& topic);
    
    // === CALLBACK SETUP ===
    void setCallback(std::function<void(char*, uint8_t*, unsigned int)> callback);
    
    // === DIAGNOSTICS ===
    size_t getQueueSize() const { return publish_queue.size(); }
    unsigned long getMessagesSent() const { return messages_sent; }
    unsigned long getMessagesFailed() const { return messages_failed; }
    unsigned long getReconnectAttempts() const { return reconnect_attempts; }
    
    // === CONVENIENCE METHODS ===
    bool publishWeight(const String& scale_id, float weight);
    bool publishStatus(const String& scale_id, int status);
    bool publishLog(const String& message);
    bool publishDiagnostics();
    
private:
    void processQueue();
    bool reconnect();
    static void staticCallback(char* topic, uint8_t* payload, unsigned int length);
};

#endif