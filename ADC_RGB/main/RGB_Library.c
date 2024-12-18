#include "RGB_Library.h"
#include "driver/ledc.h"


rgb_led_config_t led_config_t = {
    .timer_num              = TIMER_NUM,       // LEDC_TIMER_0
    .red_channel_num        = RED_CHANNEL,       // LEDC_CHANNEL_0
    .green_channel_num      = GREEN_CHANNEL,       // LEDC_CHANNEL_1
    .blue_channel_num       = BLUE_CHANNEL,       // LEDC_CHANNEL_2
    .duty_resolution_bits   = DUTY_RESOLUTION,      // Resolución de 13 bits
    .frequency_hz           = FREQUENCY,    // Frecuencia de PWM 5 kHz
        
};
/**
 * @brief Inicializa los pines del LED RGB.
 * 
 * Esta función configura los pines especificados como salidas para manejar un LED RGB.
 * 
 * @param red_pin   Número del pin GPIO para el LED rojo.
 * @param green_pin Número del pin GPIO para el LED verde.
 * @param blue_pin  Número del pin GPIO para el LED azul.
 * 
 * @return 
 *      - ESP_OK: La inicialización fue exitosa.
 *      - ESP_ERR_INVALID_ARG: Alguno de los parámetros es inválido.
 */
esp_err_t rgb_led_init( const uint8_t red_pin, const uint8_t green_pin, const uint8_t blue_pin )
{

    if (red_pin >= GPIO_NUM_MAX || green_pin >= GPIO_NUM_MAX || blue_pin >= GPIO_NUM_MAX) {
        return ESP_ERR_INVALID_ARG; // Pines fuera de rango
    }

    rgb_gpio_pins_t rgb_gpio_config = {
        .gpio_red   = red_pin,
        .gpio_green = green_pin,
        .gpio_blue  = blue_pin
    };

    //configuracion temporizador
    ledc_timer_config_t ledc_timer = {
        .speed_mode         =   led_config_t.speed_mode,
        .duty_resolution    =   led_config_t.duty_resolution_bits,
        .timer_num          =   led_config_t.timer_num,
        .freq_hz            =   led_config_t.frequency_hz,
        .clk_cfg            =   LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK( ledc_timer_config ( &ledc_timer ));

    //Reinicciar configuracion de canales pra evitar valores residuales
    ledc_stop ( led_config_t.speed_mode, led_config_t.green_channel_num, 0 );
    //ledc_stop ( led_config_t.speed_mode, led_config_t.red_channel_num,   0 );
    //ledc_stop ( led_config_t.speed_mode, led_config_t.blue_channel_num,  0 );


    //configuracion de canales
    ledc_channel_config_t channels[] = {
        {.gpio_num = rgb_gpio_config.gpio_red,   .speed_mode = led_config_t.speed_mode, .channel = led_config_t.red_channel_num, .timer_sel   = led_config_t.timer_num, .intr_type = LEDC_INTR_DISABLE,.duty = 0., .hpoint = 0},
        {.gpio_num = rgb_gpio_config.gpio_green, .speed_mode = led_config_t.speed_mode, .channel = led_config_t.green_channel_num, .timer_sel = led_config_t.timer_num, .intr_type = LEDC_INTR_DISABLE, .duty = 0,  .hpoint = 0},
        {.gpio_num = rgb_gpio_config.gpio_blue,  .speed_mode = led_config_t.speed_mode, .channel = led_config_t.blue_channel_num, .timer_sel  = led_config_t.timer_num, .intr_type = LEDC_INTR_DISABLE, .duty = 0,  .hpoint = 0}
    };

    for (int i = 0; i < 3; i++){
        ESP_ERROR_CHECK( ledc_channel_config ( &channels[i] ));
    }
    
    return ESP_OK;
}

esp_err_t  rgb_select_color ( float value_red, float value_green, float value_blue ){

    // Verifica que los porcentajes estén en rango
    if (    value_red   < MIN_VALUE_PER || value_red   > MAX_VALUE_PER ||
            value_green < MIN_VALUE_PER || value_green > MAX_VALUE_PER ||
            value_blue  < MIN_VALUE_PER || value_blue  > MAX_VALUE_PER )
        {
            return ESP_ERR_INVALID_ARG;
        }

    // Calcula el máximo valor de duty cycle según la resolución configurada
    uint32_t max_duty = ( 1 << led_config_t.duty_resolution_bits ) - 1;

    uint32_t red_duty   = max_duty - (  value_red   * ( max_duty  / MAX_VALUE_PER ));
    uint32_t green_duty = max_duty - (  value_green * ( max_duty  / MAX_VALUE_PER ));
    uint32_t blue_duty  = max_duty - (  value_blue  * ( max_duty  / MAX_VALUE_PER ));

    // Aplicamos los valores usando la API de LEDC
    ledc_set_duty    ( led_config_t.speed_mode, led_config_t.red_channel_num, red_duty );
    ledc_update_duty ( led_config_t.speed_mode, led_config_t.red_channel_num );

    ledc_set_duty    ( led_config_t.speed_mode, led_config_t.green_channel_num, green_duty );
    ledc_update_duty ( led_config_t.speed_mode, led_config_t.green_channel_num );

    ledc_set_duty    ( led_config_t.speed_mode, led_config_t.blue_channel_num, blue_duty );
    ledc_update_duty ( led_config_t.speed_mode, led_config_t.blue_channel_num );

    printf("red_duty: %lu\n", red_duty);

    return ESP_OK;
}