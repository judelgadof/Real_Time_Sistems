#include <stdio.h>
#include "RGB.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"
#include "esp_err.h"
#include "driver/ledc.h"

esp_err_t rgb_led_init(rgb_gpio_pins_t *pins, rgb_led_config_t *config ) {
    if (!pins || !config) return ESP_ERR_INVALID_ARG;

    // Configuración del temporizador
    ledc_timer_config_t ledc_timer = {
        .speed_mode      = config->speed_mode,                // Modo de velocidad
        .duty_resolution = config->duty_resolution_bits,     // Resolución del duty cycle
        .timer_num       = config->timer_num,                // Número del temporizador
        .freq_hz         = config->frequency_hz,             // Frecuencia en Hz
        .clk_cfg         = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    // Configuración de los canales
    ledc_channel_config_t channels[] = {
        { .speed_mode = config->speed_mode, .channel = config->red_channel_num,   .timer_sel = config->timer_num, .intr_type = LEDC_INTR_DISABLE, .gpio_num = pins->gpio_red,   .duty = 0, .hpoint = 0 },
        { .speed_mode = config->speed_mode, .channel = config->green_channel_num, .timer_sel = config->timer_num, .intr_type = LEDC_INTR_DISABLE, .gpio_num = pins->gpio_green, .duty = 0, .hpoint = 0 },
        { .speed_mode = config->speed_mode, .channel = config->blue_channel_num,  .timer_sel = config->timer_num, .intr_type = LEDC_INTR_DISABLE, .gpio_num = pins->gpio_blue,  .duty = 0, .hpoint = 0 }
    };

    // Aplica la configuración de cada canal
    for (int i = 0; i < 3; i++) {
        ESP_ERROR_CHECK(ledc_channel_config(&channels[i]));
    }

    return ESP_OK;
}

esp_err_t rgb_led_set_color_struct(const rgb_led_config_t *config, const rgb_color_t *color) {
    if (!config || !color) return ESP_ERR_INVALID_ARG;

    // Verifica que los porcentajes estén en rango
    if (color->red_percent < 0 || color->red_percent > 100 ||
        color->green_percent < 0 || color->green_percent > 100 ||
        color->blue_percent < 0 || color->blue_percent > 100) {
        return ESP_ERR_INVALID_ARG;
    }

    // Calcula el máximo valor de duty cycle según la resolución configurada
    uint32_t max_duty = (1 << config->duty_resolution_bits) - 1;

    // Calcula el duty cycle para cada canal
    uint32_t red_duty = (uint32_t)(max_duty * (100-color->red_percent / 100.0));
    uint32_t green_duty = (uint32_t)(max_duty * (100-color->green_percent / 100.0));
    uint32_t blue_duty = (uint32_t)(max_duty * (100-color->blue_percent / 100.0));

    // Aplica los valores usando la API de LEDC
    ledc_set_duty(config->speed_mode, config->red_channel_num, red_duty);
    ledc_update_duty(config->speed_mode, config->red_channel_num);

    ledc_set_duty(config->speed_mode, config->green_channel_num, green_duty);
    ledc_update_duty(config->speed_mode, config->green_channel_num);

    ledc_set_duty(config->speed_mode, config->blue_channel_num, blue_duty);
    ledc_update_duty(config->speed_mode, config->blue_channel_num);

    return ESP_OK;
}


