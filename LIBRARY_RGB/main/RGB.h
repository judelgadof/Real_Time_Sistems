#ifndef RGB_H
#define RGB_H

#include "driver/ledc.h"
#include "esp_err.h"

// Definición de pines del LED RGB
typedef struct {
    uint8_t gpio_red;
    uint8_t gpio_green;
    uint8_t gpio_blue;
} rgb_gpio_pins_t;

// Configuración de PWM para el LED RGB
typedef struct {
    int timer_num;             // Número del temporizador (0-3)
    int red_channel_num;       // Número de canal para el LED rojo (0-7)
    int green_channel_num;     // Número de canal para el LED verde (0-7)
    int blue_channel_num;      // Número de canal para el LED azul (0-7)
    int duty_resolution_bits;  // Resolución en bits (por ejemplo, 13 para 8192 niveles)
    int frequency_hz;          // Frecuencia de PWM en Hz (por ejemplo, 5000)
    int speed_mode;            // Modo de velocidad (0 para baja, 1 para alta)
} rgb_led_config_t;

typedef struct {
    float red_percent;
    float green_percent;
    float blue_percent;
} rgb_color_t;

esp_err_t rgb_led_init(rgb_gpio_pins_t *pins, rgb_led_config_t *config);
esp_err_t rgb_led_set_color_struct(const rgb_led_config_t *config, const rgb_color_t *color);

#endif // RGB_H