#include "config.h"
#include "scale.h"
#include <driver/adc.h>

HX711 scale;

void set_scale_zero() {
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

    // Start scale measurement
    scale.set_scale(CALIBRATION_FACTOR);  
    scale.begin(DOUT, CLK);
    set_scale_zero();
}

float read_weight() {
    return scale.get_units(10);
}
