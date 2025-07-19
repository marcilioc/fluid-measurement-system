#ifndef SCALE_H
#define SCALE_H

#define DEFAULT_CALIBRATION_FACTOR 97980

#include <HX711.h>
#include "config.h"

class Scale {
private:
    HX711 hx_conv;
    int dout_pin;
    int clk_pin;
    float last_reading;
    float calibration_factor;
    float setpoint;

public:
    Scale(uint8_t dout, uint8_t sck, float factor);

    bool is_active;
    enum class Status {
        DISCONNECTED,
        CONNECTED,
        STANDBY,
        OPERATING,
        EMPTY
    };
    Status conn_status = Status::DISCONNECTED;
    bool operation = false;

    void set_operation_status(bool op_status);
    void set_alarm(float weight);
    void check_status();
    void begin();
    float read_weight();
    long read_raw_value();
    void tare();
    void set_calibration_factor(float factor);
    float get_calibration_factor();
};

#endif
