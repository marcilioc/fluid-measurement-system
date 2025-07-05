#ifndef CONFIG_H
#define CONFIG_H

// Wi-Fi
#ifndef WIFI_SSID 
    #define WIFI_SSID "Choras Home-2.4"
#endif

#ifndef WIFI_PASSWORD
    #define WIFI_PASSWORD "pretinho"
#endif

// MQTT
#ifndef MQTT_BROKER
    #define MQTT_BROKER "192.168.15.70"
#endif

#define MQTT_PORT 1883
#define MQTT_CLIENT_ID "SMFM_ESP32"

#endif
