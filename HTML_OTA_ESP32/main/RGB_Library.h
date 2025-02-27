/**
 * @file RGB_Library.h
 * @brief Librería para el control de un LED RGB mediante PWM.
 *
 * Proporciona funciones para la inicialización del LED RGB y la selección de colores 
 * utilizando PWM en el ESP32.
 */

 #ifndef RGB_LIBRARY
 #define RGB_LIBRARY
 
 #include "driver/ledc.h"
 #include "esp_err.h"
 #include <stdio.h>
 #include "freertos/FreeRTOS.h"
 #include "freertos/task.h"
 
 // Definiciones generales para PWM y control de color
 #define TIMER_NUM           0      ///< Número del temporizador
 #define RED_CHANNEL         0      ///< Canal LED rojo (LEDC_CHANNEL_0)
 #define GREEN_CHANNEL       1      ///< Canal LED verde (LEDC_CHANNEL_1)
 #define BLUE_CHANNEL        2      ///< Canal LED azul (LEDC_CHANNEL_2)
 #define DUTY_RESOLUTION     13     ///< Resolución de PWM en bits
 #define FREQUENCY           5000   ///< Frecuencia de PWM en Hz
 #define MAX_VALUE_PER       100    ///< Máximo porcentaje para cada color
 #define MIN_VALUE_PER       0      ///< Mínimo porcentaje para cada color
 
 /**
  * @brief Configuración de pines del LED RGB.
  */
 typedef struct {
     uint8_t gpio_red;    ///< Pin GPIO para el LED rojo
     uint8_t gpio_green;  ///< Pin GPIO para el LED verde
     uint8_t gpio_blue;   ///< Pin GPIO para el LED azul
 } rgb_gpio_pins_t;
 
 /**
  * @brief Configuración de PWM para el control del LED RGB.
  */
 typedef struct {
     int timer_num;             ///< Número del temporizador (0-3)
     int red_channel_num;       ///< Canal PWM para el LED rojo (0-7)
     int green_channel_num;     ///< Canal PWM para el LED verde (0-7)
     int blue_channel_num;      ///< Canal PWM para el LED azul (0-7)
     int duty_resolution_bits;  ///< Resolución en bits (ejemplo: 13 para 8192 niveles)
     int frequency_hz;          ///< Frecuencia de PWM en Hz
     int speed_mode;            ///< Modo de velocidad (LEDC_LOW_SPEED_MODE o LEDC_HIGH_SPEED_MODE)
 } rgb_led_config_t;
 
 /**
  * @brief Inicializa el LED RGB con los pines y configuración especificados.
  * 
  * @param config Puntero a la configuración del LED RGB.
  * @param red_pin Pin GPIO del LED rojo.
  * @param green_pin Pin GPIO del LED verde.
  * @param blue_pin Pin GPIO del LED azul.
  * @return ESP_OK si la inicialización fue exitosa, ESP_ERR_INVALID_ARG en caso de error.
  */
 esp_err_t rgb_led_init ( rgb_led_config_t *config, uint8_t red_pin, uint8_t green_pin, uint8_t blue_pin );
 
 /**
  * @brief Configura la intensidad de los colores del LED RGB.
  * 
  * @param config Puntero a la configuración del LED RGB.
  * @param value_red Valor de intensidad del LED rojo (0-100%).
  * @param value_green Valor de intensidad del LED verde (0-100%).
  * @param value_blue Valor de intensidad del LED azul (0-100%).
  * @return ESP_OK si los valores son válidos, ESP_ERR_INVALID_ARG si están fuera de rango.
  */
 esp_err_t rgb_select_color ( rgb_led_config_t *config, float value_red, float value_green, float value_blue );
 
 #endif // RGB_LIBRARY
 