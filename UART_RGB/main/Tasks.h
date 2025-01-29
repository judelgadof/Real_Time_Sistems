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

extern adc_t potentiometer_adc; 
extern QueueHandle_t button_queue;
extern QueueHandle_t uart_queue;

void button_task ( void *arg );
void led_task    ( int *arg );
void potentiometer_task ( int *arg );
void uart_task ( void *arg );
void set_rgb_color ( float red_t, float green_t, float blue_t );

#endif // TASKS_H