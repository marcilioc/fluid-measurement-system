#ifndef SCALE_H
#define SCALE_H

#define CALIBRATION_FACTOR 97980

#include <HX711.h>

extern HX711 scale;

void setup_scale();
void set_scale_zero();
float read_weight();

#endif