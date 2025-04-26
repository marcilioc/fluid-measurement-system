#ifndef WIFI_MQTT_H
#define WIFI_MQTT_H

#include <WiFi.h>
#include <PubSubClient.h>

extern WiFiClient espClient;
extern PubSubClient client;

void setup_wifi();
void setup_mqtt();
void callback(char *topic, byte *payload, unsigned int length);
void reconnect_mqtt();

#endif
