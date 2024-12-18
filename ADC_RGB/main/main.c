#include <stdio.h>
#include "main.h"
#include "RGB_Library.h"
#include "ADC_manager.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"

static const char *TAG = "MAIN";

void app_main(void) {
    adc_t potentiometer_adc;

    // Inicializar el ADC para el potenciómetro
    adc_init ( &potentiometer_adc, ADC_UNIT_1, ADC_CHANNEL_6, ADC_ATTEN_DB_12 );
    rgb_led_init ( LED_RED_PIN, LED_GREEN_PIN, LED_BLUE_PIN );
    rgb_select_color( 10, 0, 80 );

    while (1) {
        int voltage = potentiometer_read_voltage ( &potentiometer_adc );
        ESP_LOGI ( TAG, "Potentiometer Voltage: %d mV", voltage );

        vTaskDelay ( pdMS_TO_TICKS ( 1000 ));
    }

    // Liberar recursos del ADC
    adc_deinit ( &potentiometer_adc );
}