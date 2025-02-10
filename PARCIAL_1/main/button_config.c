#include "button_config.h"

void button_init ( const uint8_t button_pin ) {
    gpio_config_t io_conf = {
        .pin_bit_mask =   ( 1ULL << button_pin ),  // Selecciona el pin
        .mode         =   GPIO_MODE_INPUT,        // Configura como entrada
        .pull_up_en   =   GPIO_PULLUP_ENABLE,     // Activa pull-up interno
        .pull_down_en =   GPIO_PULLDOWN_DISABLE,  // Desactiva pull-down
        .intr_type    =   GPIO_INTR_DISABLE       // Deshabilita interrupciones
    };
    
    gpio_config(&io_conf); // Aplica la configuración del GPIO
}
