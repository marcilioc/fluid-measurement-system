#include <WiFi.h>
#include "app_globals.h"
#include "mqtt_manager.h"
#include "command_handler.h"
#include "state_machine.h"
#include "config.h"
#include "scale.h"

static double s1_calibration_factor = S1_CALIBRATION_FACTOR;
static double s2_calibration_factor = S2_CALIBRATION_FACTOR;

bool prev_op = false;
bool curr_op = false;
bool op_started = false;

// Instantiate scales
Scale scale1(SCALE1_DOUT, SCALE1_SCK, s1_calibration_factor);
Scale scale2(SCALE2_DOUT, SCALE2_SCK, s2_calibration_factor);
MqttManager mqttManager;
CommandHandler commandHandler;
StateMachine stateMachine;

// Weighting variables
float s1_weight = 0;
float s2_weight = 0;

// Define standard reference scale
bool use_s1_as_reference = true;
String current_state = "IDLE";

unsigned long last_reading = 0;
const unsigned long SENSOR_INTERVAL = 500; // 0.5 between readings

void setup() {
    Serial.begin(115200);
    Serial.println("=== Adsorpion System Controller ===");

    // Connect to WiFi
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Connecting to WiFi");
    
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    
    Serial.println();
    Serial.println("WiFi connected!");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
    
    // Start components
    scale1.init();
    scale2.init();
    commandHandler.init();
    stateMachine.init();
    mqttManager.init();

    // Configure Callbacks
    mqttManager.setCommandCallback([](String topic, String payload) {
        commandHandler.handleCommand(topic, payload);
    });

    Serial.println("System started!");
}

void loop() {
    mqttManager.loop();
    
    // Read and send weights at defined intervals
    if (millis() - last_reading >= SENSOR_INTERVAL) {
        s1_weight = scale1.read_weight();
        s2_weight = scale2.read_weight();

        // Ler estado atual para selecionar peso de referência
        current_state = stateMachine.getStateString();
        if (current_state == "E1_DRYING" || current_state == "E1_PURGING" || 
            current_state == "E1_REGENERATING" || current_state == "E1_CONDITIONING") {
            use_s1_as_reference = true;
        } else if (current_state == "E2_DRYING" || current_state == "E2_PURGING" || 
                current_state == "E2_REGENERATING" || current_state == "E2_CONDITIONING") {
        use_s1_as_reference = false;
        }
        else {
            // Estado IDLE ou ERROR: volta para s1
            use_s1_as_reference = true;
        }

        // Select referenced scale based on current state
        if (use_s1_as_reference) {
            stateMachine.update(s1_weight, op_started);
        } else {
            stateMachine.update(s2_weight, op_started);
        }
        prev_op = curr_op;
        curr_op = op_started;
    
        // Stop operation
        if (curr_op == false && prev_op == true && (current_state != "IDLE" || current_state != "ERROR")) {
            stateMachine.forceState(SystemState::IDLE);
            digitalWrite(PUMP1, HIGH);
            digitalWrite(PUMP2, HIGH);
            digitalWrite(SLND1, HIGH);
            digitalWrite(SLND2, HIGH);
            Serial.println("Operation stopped.");
        }

        Serial.println("Scale 1 Weight (kg): " + String(s1_weight) + "\nScale 2 Weight (kg): " + String(s2_weight));
        Serial.println("Current State: " + stateMachine.getStateString());
        
        mqttManager.publishImmediate(WEIGHT1_TOPIC, String(s1_weight));
        mqttManager.publishImmediate(WEIGHT2_TOPIC, String(s2_weight));
        mqttManager.publishImmediate(STATES_TOPIC, String(static_cast<int>(stateMachine.getCurrentState())));
        last_reading = millis();
    }
}
