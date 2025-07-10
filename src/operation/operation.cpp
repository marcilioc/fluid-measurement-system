#include "gpio_utils.h"

void operate(int scale1_status, int scale2_status) {
    if (scale1_status == 1 && scale2_status == 1) {
        // Not operating, disable both scales consumption
        set_output_state(RLY1, 1); // Scale 01
        set_output_state(RLY2, 1); // Scale 02
    } else if(scale1_status == 2 && scale2_status == 2){
        // Start with scale_s02 consumption, by default
        set_output_state(RLY1, 1);
        set_output_state(RLY2, 0);
    } else if (scale1_status == 3 && scale2_status == 2) {
        // Disable scale_s01 consumption and enable scale_s02 consumption
        set_output_state(RLY1, 1);
        set_output_state(RLY2, 0);
    } else if (scale1_status == 2 && scale2_status == 3) {
        // Disable scale_s02 consumption and enable scale_s01 consumption
        set_output_state(RLY1, 0);
        set_output_state(RLY2, 1);
    } else if (scale1_status == 3 && scale2_status == 3) {
        // Disable both scales consumption
        set_output_state(RLY1, 1);
        set_output_state(RLY2, 1);
    }
}
