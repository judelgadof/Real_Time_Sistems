#include <stdio.h>
#include "RGB.h"

void app_main() {
    // Configuración de pines
    rgb_gpio_pins_t led_pins = {
        .gpio_red   = 5,  // Cambia según tu hardware
        .gpio_green = 19,
        .gpio_blue  = 4
    };

    // Configuración del LED RGB usando enteros
    rgb_led_config_t led_config = {
        .timer_num           = 0,       // LEDC_TIMER_0
        .red_channel_num     = 0,       // LEDC_CHANNEL_0
        .green_channel_num   = 1,       // LEDC_CHANNEL_1
        .blue_channel_num    = 2,       // LEDC_CHANNEL_2
        .duty_resolution_bits= 13,      // Resolución de 13 bits
        .frequency_hz        = 5000,    // Frecuencia de PWM 5 kHz
        .speed_mode          = 0        // LEDC_LOW_SPEED_MODE
    };

    // Inicialización del LED RGB
    if (rgb_led_init(&led_pins, &led_config) == ESP_OK) {
        printf("LED RGB configurado correctamente.\n");
    } else {
        printf("Error al configurar el LED RGB.\n");
    }

     // Define un color usando la estructura
    rgb_color_t color = {
        .red_percent   = 0,
        .green_percent = 0, 
        .blue_percent  = 0  
    };

    // Ajusta el LED a este color
    rgb_led_set_color_struct(&led_config, &color);
}