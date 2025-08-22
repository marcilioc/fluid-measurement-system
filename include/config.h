#ifndef CONFIG_H
#define CONFIG_H

// Wi-Fi
#ifndef WIFI_SSID 
    #define WIFI_SSID ""
#endif

#ifndef WIFI_PASSWORD
    #define WIFI_PASSWORD ""
#endif

// MQTT
#ifndef MQTT_BROKER
    #define MQTT_BROKER ""
#endif

#define MQTT_PORT 1883
#define MQTT_CLIENT_ID "SMFM_ESP32"

#endif
