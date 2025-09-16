#ifndef CONFIG_H
#define CONFIG_H

// Wi-Fi
#ifndef WIFI_SSID 
    #define WIFI_SSID ""
#endif

#ifndef WIFI_PASSWORD
    #define WIFI_PASSWORD ""
#endif

// MQTT CONFIG
#ifndef MQTT_BROKER
    #define MQTT_BROKER ""
#endif

#define MQTT_PORT 1883
#define MQTT_CLIENT_ID "SMFM_ESP32"

// MQTT TOPICS
#define PUB_TOPIC "smfm/measurement/"
#define WEIGHT1_TOPIC "smfm/measurement/s1/weight"
#define WEIGHT2_TOPIC "smfm/measurement/s2/weight"
#define STATUS_TOPIC "smfm/status"
#define STATES_TOPIC "smfm/state"
#define SUB_TOPIC "smfm/commands/+"

// RELAY PINS
#define SLND1 GPIO_NUM_23 // RELAY 1 - SOLENOID 1 - Pin D23
#define SLND2 GPIO_NUM_22 // RELAY 2 - SOLENOID 2 - Pin D22
#define PUMP1 GPIO_NUM_12  // RELAY 3 - PUMP 1 - Pin D12
#define PUMP2 GPIO_NUM_14  // RELAY 4 - PUMP 2 - Pin D14

// SCALE PINS
#define SCALE1_DOUT GPIO_NUM_16 // Pin RX2
#define SCALE1_SCK  GPIO_NUM_17 // Pin TX2
#define SCALE2_DOUT GPIO_NUM_18 // Pin D18
#define SCALE2_SCK  GPIO_NUM_19 // Pin D19

// LED PINS
#define EMBD_LED GPIO_NUM_2 // Pin D2 - Embedded LED
#define PURGE_LED GPIO_NUM_4 // Pin D4 - Purge LED
#define E1_COND_LED GPIO_NUM_33 // Pin D32 - E1 Conditioning LED
#define E2_COND_LED GPIO_NUM_32 // Pin D33 - E2 Conditioning LED

extern bool op_started; // Operation started flag

#endif
