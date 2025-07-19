#ifndef TOPICS_H
#define TOPICS_H

#include <functional>
const std::string SCALE_01 = "smfm/s01";
const std::string SCALE_02 = "smfm/s02";

// SEND TOPICS
const std::string WEIGHT_TOPIC_01 = SCALE_01 + "/measurement/weight";
const std::string WEIGHT_TOPIC_02 = SCALE_02 + "/measurement/weight";
const std::string STATUS_TOPIC_01 = SCALE_01 + "/operation/status";
const std::string STATUS_TOPIC_02 = SCALE_02 + "/operation/status";
const std::string CAL_FACTOR_TOPIC = "smfm/config/cal_factor";

// RECEIVE TOPICS
const std::string CAL_FACTOR_TOPIC_RB = "smfm/config/cal_factor";
const std::string ALARM_SETPOINT_01 = SCALE_01 + "/config/alarm";
const std::string ALARM_SETPOINT_02 = SCALE_02 + "/config/alarm";
const std::string TARE_S01 = SCALE_01 + "/operation/tare";
const std::string TARE_S02 = SCALE_02 + "/operation/tare";
const std::string START = "smfm/operation/start";

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