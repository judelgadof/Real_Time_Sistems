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
#include "UART_Library.h"

// LED color states
typedef enum {
    COLOR_RED,
    COLOR_GREEN,
    COLOR_BLUE,
    COLOR_NEUTRO,
    COLOR_COUNT
} led_color_t;


/**
 * @brief Tarea para manejar la ejecución de comandos UART y actualizar el LED RGB.
 * @param arg Argumento opcional.
 */
void uart_led_task(void *arg);

#endif // TASKS_H