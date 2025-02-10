/**
 * @file tasks.h
 * @brief Declaraciones de tareas y configuración de LED RGB.
 *
 * Este archivo contiene las definiciones y funciones relacionadas con el control de 
 * los LEDs RGB, la lectura del potenciómetro, el sensor NTC y la comunicación UART.
 */

 #ifndef TASKS_H
 #define TASKS_H
 
 #include "freertos/FreeRTOS.h"
 #include "freertos/task.h"
 #include "freertos/queue.h"
 #include "RGB_Library.h"
 #include "ADC_manager.h"
 #include "esp_adc/adc_cali.h"
 #include "esp_adc/adc_cali_scheme.h"
 #include "esp_adc/adc_oneshot.h"
 #include "main.h"
 #include "NTC_Library.h"
 #include "UART_Library.h"
 
 // Número total de colores usados
 #define COLOR_COUNT 4
 
 /**
  * @brief Estados de color para el LED RGB.
  */
 typedef enum {
     COLOR_RED,    ///< Color rojo
     COLOR_GREEN,  ///< Color verde
     COLOR_BLUE,   ///< Color azul
     //COLOR_NEUTRO, ///< Color neutro (comentado si no se usa)
 } led_color_t;
 
 // Variables globales externas
 extern adc_t potentiometer_adc; 
 extern QueueHandle_t button_queue;
 extern QueueHandle_t uart_queue;
 extern rgb_led_config_t led1_config;
 extern rgb_led_config_t led2_config;
 
 /**
  * @brief Configura el color del LED RGB 1 según el potenciómetro.
  * 
  * @param red   Intensidad del color rojo (0-100%).
  * @param green Intensidad del color verde (0-100%).
  * @param blue  Intensidad del color azul (0-100%).
  */
 void rgb_select_color_potentiometer ( float red, float green, float blue );
 
 /**
  * @brief Configura el color del LED RGB 2 según la temperatura del NTC.
  * 
  * @param red   Intensidad del color rojo (0-100%).
  * @param green Intensidad del color verde (0-100%).
  * @param blue  Intensidad del color azul (0-100%).
  */
 void rgb_select_color_ntc ( float red, float green, float blue );
 
 /**
  * @brief Tarea que ajusta el LED RGB en función de la temperatura y el potenciómetro.
  * 
  * @param arg Parámetro opcional (NULL por defecto).
  */
 void potentiometer_RGB_task ( void *arg );
 
 /**
  * @brief Tarea que maneja la comunicación UART y el LED RGB basado en el sensor NTC.
  * 
  * @param arg Parámetro opcional (NULL por defecto).
  */
 void ntc_uart_task ( void *arg );
 
 #endif // TASKS_H
 