#ifndef TOPICS_IMPROVED_H
#define TOPICS_IMPROVED_H

#include <string>

namespace Topics {
    // Base topics
    constexpr const char* BASE = "smfm";
    
    // Scale topics
    namespace Scale01 {
        constexpr const char* WEIGHT = "smfm/s01/measurement/weight";
        constexpr const char* STATUS = "smfm/s01/operation/status";
        constexpr const char* COMMANDS = "smfm/s01/operation/+";
        
        // Specific commands
        constexpr const char* TARE = "smfm/s01/operation/tare";
        constexpr const char* CALIBRATE = "smfm/s01/operation/calibrate";
        constexpr const char* SET_SETPOINT = "smfm/s01/operation/set_setpoint";
    }
    
    namespace Scale02 {
        constexpr const char* WEIGHT = "smfm/s02/measurement/weight";
        constexpr const char* STATUS = "smfm/s02/operation/status";
        constexpr const char* COMMANDS = "smfm/s02/operation/+";
        
        // Specific commands
        constexpr const char* TARE = "smfm/s02/operation/tare";
        constexpr const char* CALIBRATE = "smfm/s02/operation/calibrate";
        constexpr const char* SET_SETPOINT = "smfm/s02/operation/set_setpoint";
    }
    
    // System topics
    namespace System {
        constexpr const char* LOG = "smfm/log";
        constexpr const char* DIAGNOSTICS = "smfm/system/diagnostics";
        constexpr const char* START_OPERATION = "smfm/operation/start";
        constexpr const char* INFO = "smfm/system/info";
    }
}

// DISPATCH MESSAGES
void dispatch_messages(std::string&& topic, std::string&& message);

// MESSAGE HANDLING
void handle_enable_s01(std::string&& message);
void handle_enable_s02(std::string&& message);
void handle_tare_s01(std::string&& message);
void handle_tare_s02(std::string&& message);
void handle_alarm_s01(std::string&& message);
void handle_alarm_s02(std::string&& message);

#endif