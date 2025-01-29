#include <stdio.h>
#include "main.h"
#include "RGB_Library.h"
#include "ADC_manager.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "button_config.h"
#include "Tasks.h"
#include "UART_Library.h"

void app_main(void) {

    // Init LED
    rgb_led_init ( LED_RED_PIN, LED_GREEN_PIN, LED_BLUE_PIN );
    //Button Config
    button_init ( BUTTON_PIN );

    // Inicializar UART en GPIO1 (TX) y GPIO3 (RX) con velocidad de 115200
    uart_init(UART_TX, UART_RX, BAUD_RATE, NUM_UART, &uart_queue);

    // Crear la tarea UART para procesar eventos
    xTaskCreate( uart_task, "UART Task", 4096, NULL, 10, NULL);

}