#include "gpio_utils.h"

void configure_pin(gpio_num_t pin, Mode mode, bool pullup, bool pulldown) {
    gpio_config_t io_conf = {};

    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.pin_bit_mask = 1ULL << pin;
    io_conf.mode = (mode == Mode::output) ? GPIO_MODE_OUTPUT : GPIO_MODE_INPUT;
    io_conf.pull_up_en = pullup ? GPIO_PULLUP_ENABLE : GPIO_PULLUP_DISABLE;
    io_conf.pull_down_en = pulldown ? GPIO_PULLDOWN_ENABLE : GPIO_PULLDOWN_DISABLE;

    gpio_config(&io_conf);
}

void set_output_state(gpio_num_t pin, bool state) {
    gpio_set_level(pin, state ? 1 : 0);
}

bool get_pin_state(gpio_num_t pin) {
    return gpio_get_level((gpio_num_t)pin) == 1;
}
