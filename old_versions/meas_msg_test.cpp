/*
  Weight Measurement Messaging
  Description: MQTT and strain gauge application
  Platform: ESP32 - DOIT ESP32 DEVKIT V1
  Owner: Caio Marcilio dos Santos
*/

#include "HX711.h"                              // HX711 library
#include "stdio.h"                              // Miscelaneous library
#include <driver/adc.h>                         // ESP32 ADC Driver
#include <WiFi.h>                               // WiFi library
#include <PubSubClient.h>                       // MQTT PubSub library

#define DOUT A4                                 // HX711 DATA OUT - ESP32 Pin D32 pin
#define CLK A5                                  // HX711 SCK IN   - ESP32 Pin D33 pin
#define calibration_factor 97980                // Scale Calibration Factor 

HX711 scale;                                    // Scale library instance

// WiFi -- Transferir para PSRAM
const char *ssid = "Caio Marcilio";             // Enter your WiFi name
const char *password = "xtbf8160";              // Enter WiFi password

// MQTT Broker -- Transferir para PSRAM
const char *mqtt_broker = "192.168.193.180";    // Broker IP Address
const int mqtt_port = 1883;
char message[15];
long currentTime, lastTime;                     // Publish Timing

// Output topics
const char *conn_topic = "smfm/conn";

// Input topics
const char *preset_topic = "smfm/conf/sp";
const char *meas_topic = "smfm/measure";

// Starts wifi pubsub
WiFiClient espClient;
PubSubClient client(espClient);

// Scale configuration
float sp_weight;

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

  // Weight setpoint configuration
  if(strcmp(topic, preset_topic) == 0){
    sp_weight = float(message);
  }
}

void setup() {
  // ESP32 ADCs configurarion
  adc1_config_width(ADC_WIDTH_BIT_12);
  adc1_config_channel_atten(ADC1_CHANNEL_4, ADC_ATTEN_DB_0);
  adc1_config_width(ADC_WIDTH_BIT_12);
  adc1_config_channel_atten(ADC1_CHANNEL_5, ADC_ATTEN_DB_0);

  // Set software serial baud to 115200;
  Serial.begin(115200);

  // Start scale measurement
  scale.set_scale(calibration_factor);  
  scale.begin(DOUT, CLK);
  setScaleZero();

  // Connecting to a WiFi network
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to the Wi-Fi network");
  //connecting to a mqtt broker
  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);
  while (!client.connected()) {
      String client_id = "esp32-client-";
      client_id += String(WiFi.macAddress());
      Serial.printf("The client %s connects to the public MQTT broker\n", client_id.c_str());
      if (client.connect(client_id.c_str())) {
          Serial.println("Mosquitto broker connected");
      } else {
          Serial.print("failed with state ");
          Serial.print(client.state());
          delay(2000);
      }
  }
  // Publish connection aknowledge
  client.publish(conn_topic, "ESP32 connected!");
  client.subscribe(preset_topic);

  // Starts callback to read subscribed topics
  client.setCallback(callback);
}

void setScaleZero() {
  Serial.println();
  // Set new scale zero
  scale.tare();  
  Serial.println("New scale zero setted");
}

void loop() {
  float weight = 0;
  client.loop();

  currentTime = millis();
  if (currentTime - lastTime > 500) {
    weight = scale.get_units(10);
    dtostrf(weight, 5, 2, message);
    Serial.print("Peso: ");
    Serial.print(weight);
    Serial.print(" kg\n");

    client.publish(meas_topic, message);
  }
}

// Acionamento dos relés com o preset
// Alarme de underweight (subpeso)