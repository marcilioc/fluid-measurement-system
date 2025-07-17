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
    conn_status = Status::CONNECTED;
}

long Scale::read_raw_value() {
    return hx_conv.read();
}

float Scale::read_weight() {
    last_reading = hx_conv.get_units(10);
    return last_reading;
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
    } else if (op_status == false) {
        operation = false;
    }
}

void Scale::set_alarm(float weight) {
    setpoint = weight;
}

void Scale::check_status() {    
    if (operation == true) {
        if (is_active == true & last_reading >= setpoint) {
            conn_status = Status::OPERATING;
        } else if (is_active == true & last_reading < setpoint) {
            conn_status = Status::EMPTY;
        } else if (is_active == false & last_reading >= setpoint) {
            conn_status = Status::STANDBY;
        } else if (is_active == false & last_reading < setpoint) {
            conn_status = Status::EMPTY;
        }
    } else if (operation = false) {
        if (last_reading >= setpoint) {
            conn_status = Status::STANDBY;
        } else if (last_reading < setpoint){
            conn_status = Status::EMPTY;
        }
    }   
}
