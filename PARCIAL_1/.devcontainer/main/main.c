#include <stdio.h>
#include "main.h"
#include "RGB_Library.h"
#include "ADC_manager.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "button_config.h"
#include "Tasks.h"
#include "NTC_Library.h"

//static const char *TAG = "MAIN";

void app_main ( void ) {
       // Init ADC 
    adc_init ( &potentiometer_adc, ADC_UNIT_1, ADC_CHANNEL_6, ADC_ATTEN_DB_12 );

     // Inicializar ADC para el NTC
   adc_init( &ntc_adc, ADC_UNIT_2, ADC_CHANNEL_6, ADC_ATTEN_DB_12 );

    // Init LED
    rgb_led_init ( LED_RED_PIN, LED_GREEN_PIN, LED_BLUE_PIN );
    //Button Config
    button_init ( BUTTON_PIN );
    

     // Crear tareas
    xTaskCreate(potentiometer_RGB_task, "Potentiometer Task", 2048, NULL, 5, NULL);
    xTaskCreate(ntc_task, "NTC Task", 2048, NULL, 5, NULL);
}