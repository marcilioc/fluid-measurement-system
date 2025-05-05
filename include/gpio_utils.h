#ifndef GPIO_UTILS_H
#define GPIO_UTILS_H

// OUTPUT PINS
#define RLY1 GPIO_NUM_23 // RELAY 1 ENABLE - ESP32 Pin D23
#define RLY2 GPIO_NUM_22 // RELAY 2 ENABLE - ESP32 Pin D22

// INDICATION LED
// #define STATUS GPIO_NUM_2 // Embedded LED

#include "driver/gpio.h"

enum class Mode {
    input,
    output
};

// Configure pin as input/output and set pull-up/down if it is input
void configure_pin(gpio_num_t pin, Mode mode, bool pullup = false, bool pulldown = false);
void set_output_state(gpio_num_t pin, bool state);
// void blink(gpio_num_t pin, unsigned long interval_ms);

#endif
