#ifndef CONFIG_H
#define CONFIG_H

// Wi-Fi
#ifndef WIFI_SSID 
    #define WIFI_SSID "Caio Marcilio"
#endif

#ifndef WIFI_PASSWORD
    #define WIFI_PASSWORD "xtbf8160"
#endif

// MQTT
#ifndef MQTT_BROKER
    #define MQTT_BROKER "192.168.181.180"
#endif

#define MQTT_PORT 1883
#define MQTT_CLIENT_ID "SMFM_ESP32"

// HX711 ADCs
#define DOUT_1 A4 // HX711 DATA OUT - ESP32 Pin D32
#define CLK_1 A5  // HX711 SCK IN   - ESP32 Pin D33
//#define DOUT_2  // HX711 DATA OUT - ESP32 Pin D
//#define CLK_2   // HX711 SCK IN   - ESP32 Pin D

#endif
