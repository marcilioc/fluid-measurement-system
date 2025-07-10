#include "config.h"
#include "app_globals.h"
#include "topics.h"

void handle_operation(std::string&& message) {
    if (message == "1") {
        scale_s01.set_operation_status(true);
        scale_s02.set_operation_status(true);

    } else if (message == "0") {
        scale_s01.set_operation_status(false);
        scale_s02.set_operation_status(false);
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

void handle_alarm_s01(std::string&& message) {
    float setpoint = std::stof(message);
    scale_s01.set_alarm(setpoint);
}

void handle_alarm_s02(std::string&& message) {
    float setpoint = std::stof(message);
    scale_s02.set_alarm(setpoint);
}

void dispatch_messages(std::string&& topic, std::string&& message) {
    if (topic == START){
        handle_operation(message.c_str());
    } else if (topic == TARE_S01) {
        handle_tare_s01(message.c_str());
    } else if (topic == TARE_S02) {
        handle_tare_s02(message.c_str());
    } else if (topic == ALARM_SETPOINT_01) {
        handle_alarm_s01(message.c_str());
    } else if (topic == ALARM_SETPOINT_02) {
        handle_alarm_s02(message.c_str());
    }
}
