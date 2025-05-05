#include "config.h"
#include "topics.h"
#include "wifi_mqtt.h"

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
    Serial.print("Conecting to Wi-Fi");
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
  Serial.println("\nWi-Fi Connected!");
}

void setup_mqtt() {
    client.setServer(MQTT_BROKER, MQTT_PORT);
    client.setCallback(callback);
}

void reconnect_mqtt() {
    while (!client.connected()) {
        Serial.print("Trying to connect to MQTT Broker...");
        if (client.connect(MQTT_CLIENT_ID)) {
            Serial.println("Connected!");
            client.subscribe(ALARM_SETPOINT.c_str());
            client.subscribe(ENABLE_WB01.c_str());
            client.subscribe(ENABLE_WB02.c_str());
        } else {
            Serial.print("Error, rc=");
            Serial.print(client.state());
            delay(5000);
        }
    }
}

// Reads messages in subscribed topics
void callback(std::string&& topic, byte *payload, unsigned int length) {
    std::string message;
    for (int i = 0; i < length; i++) {
        message += (char)payload[i];
    }

    Serial.printf("[MQTT] Mensagem recebida. Tópico: %s | Conteúdo: %s\n", topic.c_str(), message.c_str());

    dispatch_messages(topic.c_str(), message.c_str());
}
