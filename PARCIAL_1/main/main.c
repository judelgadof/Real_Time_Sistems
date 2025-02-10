#include <stdio.h>
#include "main.h"
#include "RGB_Library.h"
#include "ADC_manager.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "button_config.h"
#include "Tasks.h"
#include "NTC_Library.h"
#include "UART_Library.h"

void app_main ( void ) {  

    // Inicializar ADC para el potenciómetro
    adc_init ( &potentiometer_adc, ADC_UNIT_1, ADC_CHANNEL_6, ADC_ATTEN_DB_12 );

    // Inicializar ADC para el NTC
    adc_init ( &ntc_adc, ADC_UNIT_2, ADC_CHANNEL_2, ADC_ATTEN_DB_12 );

    // Inicializar LED1 (Potenciómetro)
    rgb_led_init ( &led1_config, LED_RED_PIN, LED_GREEN_PIN, LED_BLUE_PIN );
    
    // Inicializar LED2 (NTC)
    rgb_led_init ( &led2_config, LED2_RED_PIN, LED2_GREEN_PIN, LED2_BLUE_PIN );

    // Inicializar botón
    button_init ( BUTTON_PIN );

    // Inicializar UART con una cola para recibir datos
    uart_init ( UART_TX, UART_RX, BAUD_RATE, NUM_UART, &uart_queue );
    
    // Crear tareas
    xTaskCreate ( potentiometer_RGB_task, "Potentiometer Task", 8192, NULL, 2, NULL );
    xTaskCreate ( ntc_uart_task, "NTC_UART_Task", 8192, NULL, 5, NULL );
}
