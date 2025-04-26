#ifndef CONFIG_H
#define CONFIG_H

// Wi-Fi
#ifndef WIFI_SSID 
    #define WIFI_SSID "Defaul SSID"
#endif

#ifndef WIFI_PASSWORD
    #define WIFI_PASSWORD "Default Pass"
#endif

// MQTT
#ifndef MQTT_BROKER
    #define MQTT_BROKER "0.0.0.0"
#endif

#define MQTT_PORT 1883
#define MQTT_CLIENT_ID "SMFM_ESP32"

// HX711
#define DOUT A4 // HX711 DATA OUT - ESP32 Pin D32
#define CLK A5  // HX711 SCK IN   - ESP32 Pin D33

#endif
