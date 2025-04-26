#include "config.h"
#include "scale.h"
#include <driver/adc.h>

HX711 scale;
static float calibrationFactor = DEFAULT_CALIBRATION_FACTOR;

void tare_scale() {
    Serial.println();
    // Set new scale zero
    scale.tare();  
    Serial.println("New scale zero setted");
  }

void setup_scale() {
    // ADCs configurarion
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ADC1_CHANNEL_4, ADC_ATTEN_DB_0);
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ADC1_CHANNEL_5, ADC_ATTEN_DB_0);

    // Start scale with default calibration factor
    scale.set_scale(calibrationFactor);
    scale.begin(DOUT, CLK);
    tare_scale();
}

float read_weight() {
    return scale.get_units(10);
}

float get_calibration_factor() {
    return calibrationFactor;
}

void set_calibration_factor(float factor) {
    calibrationFactor = factor;
    scale.set_scale(calibrationFactor);
}
