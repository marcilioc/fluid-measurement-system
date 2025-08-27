#ifndef MQTT_MANAGER_H
#define MQTT_MANAGER_H

#include <WiFi.h>
#include <PubSubClient.h>
#include <functional>

class MqttManager {
private:
    WiFiClient wifiClient;
    PubSubClient client;
    std::function<void(String, String)> commandCallback;

    void reconnect();
    static void messageCallback(char* topic, byte* payload, unsigned int length);
    static MqttManager* instance; // Para callback estático
    
public:
    MqttManager();
    void init();
    void loop();
    void publishImmediate(String topic, String payload);
    //void publishSensorData(const SensorData& data);
    void publishStatus(String status);
    void setCommandCallback(std::function<void(String, String)> callback);
    void handleMessage(String topic, String payload);
};

#endif