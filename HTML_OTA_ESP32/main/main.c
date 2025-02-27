#include <stdio.h>
#include "main.h"
#include "RGB_Library.h"
#include "ADC_manager.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "Tasks.h"
#include "NTC_Library.h"
#include "UART_Library.h"
#include "nvs_flash.h"
#include "wifi_app.h"
#include "http_server.h"
#include "driver/gpio.h"

void app_main ( void ) {  

    server_data_queue = xQueueCreate(1, sizeof(server_data_t));

    adc_init_mutex ();

    /* Initialize NVS*/
    esp_err_t ret = nvs_flash_init();
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
	{
		ESP_ERROR_CHECK(nvs_flash_erase());
		ret = nvs_flash_init();
	}
    
    ESP_ERROR_CHECK(ret);

    // Inicializar ADC para el NTC
    adc_init_multi(&adc_two, ADC_UNIT_1, ADC_CHANNEL_6, ADC_CHANNEL_7, ADC_ATTEN_DB_12);

    // Inicializar LED1 (Potenciómetro)
    rgb_led_init ( &led1_config, LED_RED_PIN, LED_GREEN_PIN, LED_BLUE_PIN );
    
    // INiciar LED NTC
    rgb_led_init ( &led2_config, LED2_RED_PIN, LED2_GREEN_PIN, LED2_BLUE_PIN );
    
    // Inicializar UART con una cola para recibir datos
    uart_init ( UART_TX, UART_RX, BAUD_RATE, NUM_UART, &uart_queue );

    /*Start Wifi*/
    init_obtain_time();
	configure_led();
    wifi_app_start();
    
    // Crear tareas
    xTaskCreate ( print_temperature_task, "Temperature Task", 12288, NULL, 2, NULL );
    xTaskCreate ( ntc_uart_task, "NTC UART Task", 12288, NULL, 2, NULL);
}
