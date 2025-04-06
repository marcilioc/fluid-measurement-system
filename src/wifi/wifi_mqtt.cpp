#include "config.h"
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

void reconnect_mqtt() {
    client.setServer(MQTT_BROKER, MQTT_PORT);
    while (!client.connected()) {
        Serial.print("Trying to connect to MQTT Broker...");
        if (client.connect(MQTT_CLIENT_ID)) {
            Serial.println("Connected!");
        } else {
            Serial.print("Error, rc=");
            Serial.print(client.state());
            delay(5000);
        }
    }
}

// Reads messages in subscribed topics
void callback(char *topic, byte *payload, unsigned int length) {
    String rec_message;
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    
    for (int i = 0; i < length; i++) {
      // Serial.print((char)payload[i]);
      rec_message += (char)payload[i];
    }
}
