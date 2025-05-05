#include "config.h"
#include "topics.h"
#include "gpio_utils.h"
#include <Arduino.h>

void handle_wb01_topic(std::string&& message) {
    // A lógica do optoacoplador do módulo de relés é pull down
    Serial.printf(message.c_str());
    if (message == "1") {
        set_output_state(RLY1, 0);
    } else if (message == "0"){
        set_output_state(RLY1, 1);
    }
}

void handle_wb02_topic(std::string&& message) {
    // A lógica do optoacoplador do módulo de relés é pull down
    if (message == "1") {
        set_output_state(RLY2, 0);
    } else if (message == "0"){
        set_output_state(RLY2, 1);
    }
}

void dispatch_messages(std::string&& topic, std::string&& message) {
    if (topic == ENABLE_WB01) {
        handle_wb01_topic(message.c_str());
    } else if (topic == ENABLE_WB02) {
        handle_wb02_topic(message.c_str());
    }
}
