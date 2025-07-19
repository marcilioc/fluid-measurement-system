#include "config.h"
#include "topics.h"
#include "mqtt_manager.h"
#include "app_globals.h"
#include "gpio_utils.h"

WiFiClient esp_client;
MqttManager mqtt(&esp_client);

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

// Reads messages in subscribed topics
void callback(char* topic, byte* payload, unsigned int length) {
    std::string message;
    for (int i = 0; i < length; i++) {
        message += (char)payload[i];
    }

    Serial.printf("[MQTT] Mensagem recebida. Tópico: %s | Conteúdo: %s\n", topic, message.c_str());

    dispatch_messages(topic, message.c_str());
}

void setup() {
    Serial.begin(115200);

    // GPIO Setup
    configure_pin(RLY1, Mode::output);
    configure_pin(RLY2, Mode::output);
    set_output_state(RLY1, 1);  // Disabled
    set_output_state(RLY2, 1);  // Disabled

    // WiFi Setup
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Conecting to Wi-Fi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWi-Fi Connected!");
    Serial.printf("IP address: %s\n", WiFi.localIP().toString().c_str());

    mqtt.begin(MQTT_BROKER, MQTT_PORT, MQTT_CLIENT_ID);
    mqtt.setCallback(callback);

    // Subscribe to topics
    mqtt.subscribe("smfm/operation/start");
    mqtt.subscribe("smfm/s01/operation/+");
    mqtt.subscribe("smfm/s02/operation/+");

    scale_s01.begin();
    scale_s02.begin();

    timer = timerBegin(0, 80, true);
    timerAttachInterrupt(timer, &onTimer, true);
    timerAlarmWrite(timer, 100000, true); // Em microssegundos
    timerAlarmEnable(timer);
}

void loop() {
    char payload[10];
    mqtt.loop();

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
        if (scale_s01.operation == false) {
            set_output_state(RLY1, 1);
            set_output_state(RLY2, 1);
            scale_s01.is_active = false;
            scale_s02.is_active = false;
        } else if (status1 == 1 && status2 == 1) {
            // Not operating, disable both scales consumption
            set_output_state(RLY1, 1); // Scale 01
            set_output_state(RLY2, 1); // Scale 02
            scale_s01.is_active = false;
            scale_s02.is_active = false;
        } else if(status1 == 2 && status2 == 2){
            set_output_state(RLY1, 0);
            set_output_state(RLY2, 1);
            scale_s01.is_active = true;
            scale_s02.is_active = false;
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

        mqtt.publishWeight("s01", weight_01);
        mqtt.publishWeight("s02", weight_02);
        mqtt.publishStatus("s01", status1);
        mqtt.publishStatus("s02", status2);
    }
}
