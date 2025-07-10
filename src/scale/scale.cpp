#include "scale.h"

// Define Scale object
Scale::Scale(uint8_t dout, uint8_t clk, float factor) 
    : dout_pin(dout), clk_pin(clk), calibration_factor(factor) {
}

void Scale::tare() {
    hx_conv.tare();
}

void Scale::begin() {
    // Start scale with default calibration factor
    hx_conv.set_scale(calibration_factor);
    hx_conv.begin(dout_pin, clk_pin);
    Scale::tare();
    status = 1;
}

long Scale::read_raw_value() {
    return hx_conv.read();
}

float Scale::read_weight() {
    return hx_conv.get_units(10);
}

float Scale::get_calibration_factor() {
    return calibration_factor;
}

void Scale::set_calibration_factor(float factor) {
    calibration_factor = factor;
    hx_conv.set_scale(calibration_factor);
}

void Scale::set_operation_status(bool op_status) {
    if (op_status == true) {
        operation = true;
        status = 2;
    } else if (op_status == false) {
        operation = false;
        status = 1;
    }
}

void Scale::set_alarm(float weight) {
    setpoint = weight;
}

void Scale::check_alarm(float weight) {
    if(operation = true) {
        if(weight <= setpoint) {
            status = 3;
        } else if (weight > setpoint) {
            status = 2;
        }
    } else if (operation = false) {
        status = 1;
    }
}
