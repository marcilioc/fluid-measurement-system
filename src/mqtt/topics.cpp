#include "config.h"
#include "app_globals.h"
#include "topics.h"
#include "gpio_utils.h"

void handle_enable_s01(std::string&& message) {
    // A lógica do optoacoplador do módulo de relés é pull down
    if (message == "1") {
        set_output_state(RLY1, 0);
    } else if (message == "0"){
        set_output_state(RLY1, 1);
    }
}

void handle_enable_s02(std::string&& message) {
    // A lógica do optoacoplador do módulo de relés é pull down
    if (message == "1") {
        set_output_state(RLY2, 0);
    } else if (message == "0"){
        set_output_state(RLY2, 1);
    }
}

void handle_tare_s01(std::string&& message) {
    if (message == "1") {
        // Send tare command to Scale object 01
        scale_s01.tare();
    }
}

void handle_tare_s02(std::string&& message) {
    if (message == "1") {
        // Send tare command to Scale object 02
        scale_s02.tare();
    }
}

void dispatch_messages(std::string&& topic, std::string&& message) {
    if (topic == ENABLE_S01){
        handle_enable_s01(message.c_str());
    } else if (topic == ENABLE_S02){
        handle_enable_s02(message.c_str());
    } else if (topic == TARE_S01) {
        handle_tare_s01(message.c_str());
    } else if (topic == TARE_S02) {
        handle_tare_s02(message.c_str());
    }
}
