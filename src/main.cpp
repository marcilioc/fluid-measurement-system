#include <WiFi.h>
#include "mqtt_manager.h"
#include "config.h"
#include "scale.h"

MqttManager mqttManager;

static long calibration_factor = DEFAULT_CALIBRATION_FACTOR;

// Weighting variables
float s1_weight = 0;
float s2_weight = 0;

// Instantiate scales
Scale scale1(SCALE1_DOUT, SCALE1_SCK, calibration_factor);
Scale scale2(SCALE2_DOUT, SCALE2_SCK, calibration_factor);

unsigned long last_reading = 0;
const unsigned long SENSOR_INTERVAL = 500; // 0.5 segundos para começar

void setup() {
    Serial.begin(115200);
    Serial.println("=== ESP32 MQTT Client ===");

    // Conectar WiFi
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Conectando ao WiFi");
    
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    
    Serial.println();
    Serial.println("WiFi conectado!");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
    
    // Inicializar componentes
    scale1.init();
    scale2.init();
    mqttManager.init();
    // Command Processor

    // Configurar Callbacks
    
    Serial.println("Sistema inicializado - enviando dados a cada 0.5 segundos");
}

void loop() {
    mqttManager.loop();
    
    // Leitura e envio de sensores
    if (millis() - last_reading >= SENSOR_INTERVAL) {
        // Ler balanças
        s1_weight = scale1.read_weight();
        s2_weight = scale2.read_weight();

        // Debug local
        Serial.println("Scale 1 Weight: " + String(s1_weight) + "\nScale 2 Weight: " + String(s2_weight));
        
        mqttManager.publishImmediate(WEIGHT1_TOPIC, String(s1_weight));
        mqttManager.publishImmediate(WEIGHT2_TOPIC, String(s2_weight));
        last_reading = millis();
    }
}
