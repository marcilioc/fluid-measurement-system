#include "config.h"
#include "scale.h"
#include "wifi_mqtt.h"

// Timer
hw_timer_t * timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
volatile bool pending_read = false;

float last_reading = 0.0;

void IRAM_ATTR onTimer() {
    portENTER_CRITICAL_ISR(&timerMux);
    pending_read = true;
    portEXIT_CRITICAL_ISR(&timerMux);
}

void setup() {
    Serial.begin(115200);
    setup_wifi();
    client.setServer(MQTT_BROKER, MQTT_PORT);
    setup_scale();
  
    timer = timerBegin(0, 80, true);
    timerAttachInterrupt(timer, &onTimer, true);
    timerAlarmWrite(timer, 1000000, true); // Em microssegundos
    timerAlarmEnable(timer);
}

void loop() {
    if (!client.connected()) reconnect_mqtt();
    client.loop();

    if (pending_read) {
        portENTER_CRITICAL(&timerMux);
        pending_read = false;
        portEXIT_CRITICAL(&timerMux);

        float weight = read_weight();
        Serial.printf("Peso: %.2f\n", weight);
        char payload[50];
        snprintf(payload, sizeof(payload), "%.2f", weight);
        client.publish(MQTT_TOPIC, payload);
        last_reading = weight;
    }
}
