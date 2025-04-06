#include "config.h"
#include "scale.h"

HX711 scale;

void set_scale_zero() {
    Serial.println();
    // Set new scale zero
    scale.tare();  
    Serial.println("New scale zero setted");
  }

void setup_scale() {
    // Start scale measurement
    scale.set_scale(CALIBRATION_FACTOR);  
    scale.begin(DOUT, CLK);
    set_scale_zero();
}

float read_weight() {
    return scale.get_units(10);
}
