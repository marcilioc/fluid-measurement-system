#ifndef TOPICS_H
#define TOPICS_H

#include <functional>

// SEND TOPICS
const std::string WEIGHT_TOPIC = "smfm/measurement/weight";
const std::string CAL_FACTOR_TOPIC = "smfm/config/cal_factor";

// RECEIVE TOPICS
const std::string CAL_FACTOR_TOPIC_RB = "smfm/config/cal_factor_rb";
const std::string ALARM_SETPOINT = "smfm/config/alarm_sp";
const std::string ENABLE_WB01 = "smfm/operation/enable_wb01";
const std::string ENABLE_WB02 = "smfm/operation/enable_wb02";

// DISPATCH MESSAGES
void dispatch_messages(std::string&& topic, std::string&& message);

// MESSAGE HANDLING
void handle_wb01_topic(std::string&& message);

#endif