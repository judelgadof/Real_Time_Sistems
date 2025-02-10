#include "button_config.h"

void button_init ( const uint8_t button_pin ) {
    gpio_config_t io_conf = {
        .pin_bit_mask =   ( 1ULL << button_pin ),
        .mode         =   GPIO_MODE_INPUT,
        .pull_up_en   =   GPIO_PULLUP_ENABLE,
        .pull_down_en =   GPIO_PULLDOWN_DISABLE,
        .intr_type    =   GPIO_INTR_DISABLE
    };
    gpio_config(&io_conf);
}

