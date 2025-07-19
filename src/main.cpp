#include "config.h"
#include "topics.h"
#include "app_globals.h"
#include "wifi_mqtt.h"
#include "gpio_utils.h"

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

Scale scale_s01(GPIO_NUM_16, GPIO_NUM_17, calibration_factor);
Scale scale_s02(GPIO_NUM_18, GPIO_NUM_19, calibration_factor);

void setup() {
    // configure_pin(STATUS, Mode::output);  // Status embedded LED
    configure_pin(RLY1, Mode::output);
    configure_pin(RLY2, Mode::output);
    set_output_state(RLY1, 1);  // Disabled
    set_output_state(RLY2, 1);  // Disabled
    Serial.begin(115200);
    setup_wifi();
    setup_mqtt();
    scale_s01.begin();
    scale_s02.begin();

    timer = timerBegin(0, 80, true);
    timerAttachInterrupt(timer, &onTimer, true);
    timerAlarmWrite(timer, 500000, true); // Em microssegundos
    timerAlarmEnable(timer);
}

void loop() {
    char payload[10];
    if (!client.connected()) reconnect_mqtt();
    client.loop();

    if (pending_read) {
        portENTER_CRITICAL(&timerMux);
        pending_read = false;
        portEXIT_CRITICAL(&timerMux);

        // Convert status enum class to int
        int status1 = static_cast<int>(scale_s01.conn_status);
        int status2 = static_cast<int>(scale_s02.conn_status);
        
        float weight_01 = scale_s01.read_weight();
        scale_s01.check_status();
        
        float weight_02 = scale_s02.read_weight();
        scale_s02.check_status();

        // Check scales status
        if (status1 == 1 && status2 == 1) {
            // Not operating, disable both scales consumption
            set_output_state(RLY1, 1); // Scale 01
            set_output_state(RLY2, 1); // Scale 02
            scale_s01.is_active = false;
            scale_s02.is_active = false;
        } else if(status1 == 2 && status2 == 2){
            if (scale_s01.operation == true) {
                set_output_state(RLY1, 0);
                set_output_state(RLY2, 1);
                scale_s01.is_active = true;
                scale_s02.is_active = false;
            } else if (scale_s01.operation == false){
                set_output_state(RLY1, 1);
                set_output_state(RLY2, 1);
                scale_s01.is_active = false;
                scale_s02.is_active = false;
            }
        } else if (status1 == 3 && status2 == 2){
            scale_s01.is_active = true;
            scale_s02.is_active = false;
        } else if (status1 == 2 && status2 == 3){
            scale_s01.is_active = false;
            scale_s02.is_active = true;
        } else if (status1 == 4 && status2 == 2) {
            // Disable scale_s01 consumption and enable scale_s02 consumption
            set_output_state(RLY1, 1);
            set_output_state(RLY2, 0);
            scale_s01.is_active = false;
            scale_s02.is_active = true;
        } else if (status1 == 2 && status2 == 4) {
            // Disable scale_s02 consumption and enable scale_s01 consumption
            set_output_state(RLY1, 0);
            set_output_state(RLY2, 1);
            scale_s01.is_active = true;
            scale_s02.is_active = false;
        } else if (status1 == 4 && status2 == 4) {
            // Disable both scales consumption
            set_output_state(RLY1, 1);
            set_output_state(RLY2, 1);
            scale_s01.is_active = false;
            scale_s02.is_active = false;
        }

        // Send Scales Status
        snprintf(payload, sizeof(payload), "%d", status1);
        client.publish(STATUS_TOPIC_01.c_str(), payload);
        snprintf(payload, sizeof(payload), "%d", status2);
        client.publish(STATUS_TOPIC_02.c_str(), payload);

        // Reading 01
        snprintf(payload, sizeof(payload), "%.3f", weight_01);
        client.publish(WEIGHT_TOPIC_01.c_str(), payload);
        // Reading 02
        snprintf(payload, sizeof(payload), "%.3f", weight_02);
        client.publish(WEIGHT_TOPIC_02.c_str(), payload);
    }
}
