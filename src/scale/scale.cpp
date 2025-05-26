#include "config.h"
#include "scale.h"
#include <driver/adc.h>

// Define Scale object
Scale::Scale(uint8_t dout, uint8_t clk, float factor) 
    : dout_pin(dout), clk_pin(clk), calibration_factor(factor) {}

void Scale::tare() {
    hx_conv.tare();
}

void Scale::begin() {
    // Start scale with default calibration factor
    hx_conv.set_scale(calibration_factor);
    hx_conv.begin(dout_pin, clk_pin);
    Scale::tare();
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
