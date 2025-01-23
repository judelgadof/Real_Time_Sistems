#include <stdio.h>
#include "main.h"
#include "RGB_Library.h"
#include "ADC_manager.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "button_config.h"
#include "Tasks.h"

//static const char *TAG = "MAIN";

void app_main ( void ) {
     int global_intensity;
    //adc_t potentiometer_adc;

    // Init ADC 
    adc_init ( &potentiometer_adc, ADC_UNIT_1, ADC_CHANNEL_6, ADC_ATTEN_DB_12 );
    // Init LED
    rgb_led_init ( LED_RED_PIN, LED_GREEN_PIN, LED_BLUE_PIN );
    //Button Config
    button_init ( BUTTON_PIN );
    
    // Crear cola para comunicación
    button_queue = xQueueCreate(10, sizeof(led_color_t));
    if (button_queue == NULL) {
        printf("Error al crear la cola.\n");
        return;
    }

    // Crear tareas
    xTaskCreate( button_task, "Button Task", 2048, NULL, 1, NULL );
    xTaskCreate( led_task,    "LED Task",    2048, &global_intensity, 1, NULL );
    xTaskCreate( potentiometer_task, "potentiometer_task", 2048, &global_intensity, 5, NULL);
}