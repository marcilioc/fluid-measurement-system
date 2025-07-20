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
    if (topic == Topics::System::START_OPERATION){
        handle_operation(message.c_str());
    } else if (topic == Topics::Scale01::TARE) {
        handle_tare_s01(message.c_str());
    } else if (topic == Topics::Scale02::TARE) {
        handle_tare_s02(message.c_str());
    } else if (topic == Topics::Scale01::SET_SETPOINT) {
        handle_alarm_s01(message.c_str());
    } else if (topic == Topics::Scale02::SET_SETPOINT) {
        handle_alarm_s02(message.c_str());
    }
}
