#ifndef CONFIG_H
#define CONFIG_H

// Wi-Fi
#ifndef WIFI_SSID 
    #define WIFI_SSID "DefaultSSID"
#endif

#ifndef WIFI_PASSWORD
    #define WIFI_PASSWORD "DefaultPass"
#endif

// MQTT
#ifndef MQTT_BROKER
    #define MQTT_BROKER "localhost"
#endif

#define MQTT_PORT 1883
#define MQTT_TOPIC "smfm"

// HX711
#define DOUT A4 // HX711 DATA OUT - ESP32 Pin D32
#define CLK A5  // HX711 SCK IN   - ESP32 Pin D33

// Outras configurações
#define PESO_LIMIAR 0.1f
#define MQTT_CLIENT_ID "ESP32Client"

#endif