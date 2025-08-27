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

    void init();
    void tare();
    long read_raw_value();
    float read_weight();
    float get_calibration_factor();
    void set_calibration_factor(float factor);
};

#endif
