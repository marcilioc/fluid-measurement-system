#ifndef CONFIG_H
#define CONFIG_H

// Wi-Fi
#ifndef WIFI_SSID 
    #define WIFI_SSID "Caio Marcilio"
#endif

#ifndef WIFI_PASSWORD
    #define WIFI_PASSWORD "xtbf8160"
#endif

// MQTT CONFIG
#ifndef MQTT_BROKER
    #define MQTT_BROKER "192.168.151.180"
#endif

#define MQTT_PORT 1883
#define MQTT_CLIENT_ID "SMFM_ESP32"

// MQTT TOPICS
#define PUB_TOPIC "smfm/measurement/"
#define WEIGHT1_TOPIC "smfm/measurement/s1/weight"
#define WEIGHT2_TOPIC "smfm/measurement/s2/weight"
#define STATUS_TOPIC "smfm/status"
#define SUB_TOPIC "smfm/commands/+"

// RELAY PINS
#define SLND1 GPIO_NUM_23 // RELAY 1 - SOLENOID 1 - Pin D23
#define SLND2 GPIO_NUM_22 // RELAY 2 - SOLENOID 2 - Pin D22
#define PUMP1 GPIO_NUM_01 // RELAY 3 - PUMP 1 - Pin RX0
#define PUMP2 GPIO_NUM_03 // RELAY 4 - PUMP 2 - Pin TX0

// SCALE PINS
#define SCALE1_DOUT GPIO_NUM_16 // Pin RX2
#define SCALE1_SCK  GPIO_NUM_17 // Pin TX2
#define SCALE2_DOUT GPIO_NUM_18 // Pin D18
#define SCALE2_SCK  GPIO_NUM_19 // Pin D19

#endif
