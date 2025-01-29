#ifndef RGB_LIBRARY
#define RGB_LIBRARY

#include "driver/ledc.h"
#include "esp_err.h"
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define TIMER_NUM           0      // Numero de Timer
#define RED_CHANNEL         0      // LEDC_CHANNEL_0
#define GREEN_CHANNEL       1       // LEDC_CHANNEL_1
#define BLUE_CHANNEL        2       // LEDC_CHANNEL_2
#define DUTY_RESOLUTION     13      // Resolución de 13 bits
#define FREQUENCY           5000    //Frecuencia de PWM Hz
#define MAX_VALUE_PER       100     //Maximo valor de porcentaje por color
#define MIN_VALUE_PER       0

// Definición de pines del LED RGB
typedef struct {
    uint8_t gpio_red;
    uint8_t gpio_green;
    uint8_t gpio_blue;
} rgb_gpio_pins_t;

// Definición de pines del LED RGB
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


esp_err_t  rgb_led_init( uint8_t red_pin, uint8_t green_pin, uint8_t blue_pin );
esp_err_t  rgb_select_color ( float value_red, float value_green, float value_blue );

#endif 