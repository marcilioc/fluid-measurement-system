#ifndef SCALE_H
#define SCALE_H

#define DEFAULT_CALIBRATION_FACTOR 97980

#include <HX711.h>

extern HX711 scale;

void setup_scale();
void set_calibration_factor(float factor);
float get_calibration_factor();
void tare_scale();
float read_weight();

#endif
