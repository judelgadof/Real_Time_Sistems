#include "RGB_Library.h"
#include "driver/ledc.h"

esp_err_t rgb_led_init ( rgb_led_config_t *config, const uint8_t red_pin, const uint8_t green_pin, const uint8_t blue_pin ) {

    // Verifica que los pines estén dentro del rango permitido
    if ( red_pin >= GPIO_NUM_MAX || green_pin >= GPIO_NUM_MAX || blue_pin >= GPIO_NUM_MAX ) {
        return ESP_ERR_INVALID_ARG;
    }

    // Configuración de los pines RGB
    rgb_gpio_pins_t rgb_gpio_config = {
        .gpio_red   = red_pin,
        .gpio_green = green_pin,
        .gpio_blue  = blue_pin
    };

    // Configuración del temporizador para PWM
    ledc_timer_config_t ledc_timer = {
        .speed_mode         = config->speed_mode,
        .duty_resolution    = config->duty_resolution_bits,
        .timer_num          = config->timer_num,
        .freq_hz            = config->frequency_hz,
        .clk_cfg            = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK ( ledc_timer_config ( &ledc_timer ));

    // Reinicia el canal del LED verde para evitar valores residuales
    ledc_stop ( config->speed_mode, config->green_channel_num, 0 );

    // Configuración de los canales PWM para los LEDs RGB
    ledc_channel_config_t channels[] = {
        { .gpio_num = rgb_gpio_config.gpio_red,   .speed_mode = config->speed_mode, .channel = config->red_channel_num,   .timer_sel = config->timer_num, .intr_type = LEDC_INTR_DISABLE, .duty = 0, .hpoint = 0 },
        { .gpio_num = rgb_gpio_config.gpio_green, .speed_mode = config->speed_mode, .channel = config->green_channel_num, .timer_sel = config->timer_num, .intr_type = LEDC_INTR_DISABLE, .duty = 0, .hpoint = 0 },
        { .gpio_num = rgb_gpio_config.gpio_blue,  .speed_mode = config->speed_mode, .channel = config->blue_channel_num,  .timer_sel = config->timer_num, .intr_type = LEDC_INTR_DISABLE, .duty = 0, .hpoint = 0 }
    };

    // Configura los canales de PWM
    for ( int i = 0; i < 3; i++ ) {
        ESP_ERROR_CHECK ( ledc_channel_config ( &channels[i] ));
    }
    
    return ESP_OK;
}

esp_err_t rgb_select_color ( rgb_led_config_t *config, float value_red, float value_green, float value_blue ) {

    // Verifica que los valores de color estén dentro del rango permitido (0-100%)
    if ( value_red   < MIN_VALUE_PER || value_red   > MAX_VALUE_PER ||
         value_green < MIN_VALUE_PER || value_green > MAX_VALUE_PER ||
         value_blue  < MIN_VALUE_PER || value_blue  > MAX_VALUE_PER ) {
        return ESP_ERR_INVALID_ARG;
    }

    // Calcula el valor máximo del duty cycle según la resolución configurada
    uint32_t max_duty = ( 1 << config->duty_resolution_bits ) - 1;

    // Calcula el duty cycle para cada color
    uint32_t red_duty   = max_duty - ( value_red   * ( max_duty / MAX_VALUE_PER ));
    uint32_t green_duty = max_duty - ( value_green * ( max_duty / MAX_VALUE_PER ));
    uint32_t blue_duty  = max_duty - ( value_blue  * ( max_duty / MAX_VALUE_PER ));

    // Aplica los valores de PWM a los canales correspondientes
    ledc_set_duty    ( config->speed_mode, config->red_channel_num, red_duty );
    ledc_update_duty ( config->speed_mode, config->red_channel_num );

    ledc_set_duty    ( config->speed_mode, config->green_channel_num, green_duty );
    ledc_update_duty ( config->speed_mode, config->green_channel_num );

    ledc_set_duty    ( config->speed_mode, config->blue_channel_num, blue_duty );
    ledc_update_duty ( config->speed_mode, config->blue_channel_num );

    return ESP_OK;
}
