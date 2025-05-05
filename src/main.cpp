#include "config.h"
#include "topics.h"
#include "scale.h"
#include "wifi_mqtt.h"
#include "gpio_utils.h"

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
    // configure_pin(STATUS, Mode::output);  // Status embedded LED
    configure_pin(RLY1, Mode::output);
    configure_pin(RLY2, Mode::output);
    set_output_state(RLY1, 0);  // Enabled
    set_output_state(RLY2, 1);  // Disabled
    Serial.begin(115200);
    setup_wifi();
    setup_mqtt();
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
        // Serial.printf("Peso: %.3f kg\n", weight);
        char payload[50];
        snprintf(payload, sizeof(payload), "%.3f", weight);
        client.publish(WEIGHT_TOPIC.c_str(), payload);
        last_reading = weight;
    }
}
