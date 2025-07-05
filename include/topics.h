#ifndef TOPICS_H
#define TOPICS_H

#include <functional>
const std::string SCALE_01 = "smfm/s01";
const std::string SCALE_02 = "smfm/s02";

// SEND TOPICS
const std::string WEIGHT_TOPIC_01 = "smfm/s01/measurement/weight";
const std::string WEIGHT_TOPIC_02 = "smfm/s02/measurement/weight";
const std::string CAL_FACTOR_TOPIC = "smfm/config/cal_factor";

// RECEIVE TOPICS
const std::string CAL_FACTOR_TOPIC_RB = "smfm/config/cal_factor_rb";
const std::string ALARM_SETPOINT = "smfm/config/alarm_sp";
const std::string ENABLE_S01 = "smfm/s01/operation/enable";
const std::string ENABLE_S02 = "smfm/s02/operation/enable";
const std::string TARE_S01 = "smfm/s01/operation/tare";
const std::string TARE_S02 = "smfm/s02/operation/tare";

// DISPATCH MESSAGES
void dispatch_messages(std::string&& topic, std::string&& message);

// MESSAGE HANDLING
void handle_enable_s01(std::string&& message);
void handle_enable_s02(std::string&& message);
void handle_tare_s01(std::string&& message);
void handle_tare_s02(std::string&& message); 

#endif