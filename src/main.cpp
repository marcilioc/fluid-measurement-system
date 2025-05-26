#include "config.h"
#include "topics.h"
#include "scale.h"
#include "wifi_mqtt.h"
#include "gpio_utils.h"
#include <driver/adc.h>

static int calibration_factor = DEFAULT_CALIBRATION_FACTOR;
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

Scale scale_wb01(GPIO_NUM_16, GPIO_NUM_17, calibration_factor);
Scale scale_wb02(GPIO_NUM_18, GPIO_NUM_19, calibration_factor);

void setup() {
    // configure_pin(STATUS, Mode::output);  // Status embedded LED
    configure_pin(RLY1, Mode::output);
    configure_pin(RLY2, Mode::output);
    set_output_state(RLY1, 0);  // Enabled
    set_output_state(RLY2, 1);  // Disabled
    Serial.begin(115200);
    setup_wifi();
    setup_mqtt();
    scale_wb01.begin();
    scale_wb02.begin();
  
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

        float weight_01 = scale_wb01.read_weight();
        float weight_02 = scale_wb02.read_weight();
        char payload[50];

        // Reading 01
        snprintf(payload, sizeof(payload), "%.3f", weight_01);
        client.publish(WEIGHT_TOPIC_01.c_str(), payload);
        // Reading 02
        snprintf(payload, sizeof(payload), "%.3f", weight_02);
        client.publish(WEIGHT_TOPIC_02.c_str(), payload);
        last_reading = weight_01;
    }
}
