#include "tasks.h"

// Cola de mensajes para gestionar eventos de botón
QueueHandle_t button_queue;

// Estructura de configuración del ADC para el potenciómetro
adc_t potentiometer_adc;

// Configuración del LED RGB 1 (asociado al potenciómetro)
rgb_led_config_t led1_config = {
    .timer_num              = LEDC_TIMER_0,
    .red_channel_num        = LEDC_CHANNEL_0,
    .green_channel_num      = LEDC_CHANNEL_1,
    .blue_channel_num       = LEDC_CHANNEL_2,
    .duty_resolution_bits   = DUTY_RESOLUTION,
    .frequency_hz           = FREQUENCY,
};

// Configuración del LED RGB 2 (asociado al NTC)
rgb_led_config_t led2_config = {
    .timer_num              = LEDC_TIMER_1,
    .red_channel_num        = LEDC_CHANNEL_3,
    .green_channel_num      = LEDC_CHANNEL_4,
    .blue_channel_num       = LEDC_CHANNEL_5,
    .duty_resolution_bits   = DUTY_RESOLUTION,
    .frequency_hz           = FREQUENCY,
};

// Selecciona el color del LED RGB 1 basado en el potenciómetro
void rgb_select_color_potentiometer ( float red, float green, float blue ) {
    rgb_select_color ( &led1_config, red, green, blue );
}

// Selecciona el color del LED RGB 2 basado en el sensor NTC
void rgb_select_color_ntc ( float red, float green, float blue ) {
    rgb_select_color ( &led2_config, red, green, blue );
}

// Tarea que ajusta el LED RGB en función de la temperatura y el potenciómetro
void potentiometer_RGB_task ( void *arg ) {
    int brightness = 0;

    while ( 1 ) {
        // Leer la temperatura del sensor NTC
        float temperature = ntc_read_temperature();

        // Leer y normalizar el valor del potenciómetro (0-100)
        int raw_value = adc_read_raw ( &potentiometer_adc );
        brightness = ( raw_value * 100 ) / ( ( 1 << ADC_BITWIDTH_12 ) - 1 );

        // Selección de color basada en la temperatura
        if ( temperature >= ranges_config.temp_red_min && temperature < ranges_config.temp_red_max ) {
            rgb_select_color_potentiometer ( brightness, LED_OFF, LED_OFF ); // Rojo
        } 

        if ( temperature >= ranges_config.temp_green_min && temperature < ranges_config.temp_green_max ) {
            rgb_select_color_potentiometer ( LED_OFF, brightness, LED_OFF ); // Verde
            
            // Amarillo (mezcla de rojo y verde) para temperaturas intermedias
            if ( temperature < ranges_config.temp_green_max && temperature > ranges_config.temp_red_min ) {
                rgb_select_color_potentiometer ( brightness, brightness, LED_OFF ); 
            }
        } 

        if ( temperature >= ranges_config.temp_blue_min && temperature < ranges_config.temp_blue_max ) {
            rgb_select_color_potentiometer ( LED_OFF, LED_OFF, brightness ); // Azul

            // Cian (mezcla de verde y azul) para temperaturas intermedias
            if ( temperature < ranges_config.temp_blue_max && temperature > ranges_config.temp_green_min ) {
                rgb_select_color_potentiometer ( LED_OFF, brightness, brightness );
            }
        } 

        // Espera 100ms antes de la próxima actualización
        vTaskDelay ( pdMS_TO_TICKS ( 100 ));
    }
}

// Tarea que maneja la comunicación UART y el LED RGB según la temperatura del NTC
void ntc_uart_task ( void *arg ) {
    char received_data[UART_BUFFER_SIZE];
    bool print_temp = false; 
    bool was_printing = false;
    int print_freq_temp = 1;

    // Intensidades iniciales del LED RGB
    float red_intensity = LED_ON, green_intensity = LED_ON, blue_intensity = LED_ON;
    TickType_t last_print_time = xTaskGetTickCount();

    while ( 1 ) {
        // Leer datos desde la UART
        int length = uart_read_bytes ( NUM_UART, ( uint8_t * ) received_data, sizeof ( received_data ) - 1, pdMS_TO_TICKS ( 10 ));
        
        if ( length > 0 ) {
            received_data[length] = '\0'; // Asegurar que la cadena termina correctamente
            uart_execute_command ( received_data, &ranges_config, &print_temp, &print_freq_temp, &red_intensity, &green_intensity, &blue_intensity );
        }

        // Leer la temperatura del sensor NTC
        float temperature = ntc_read_temperature();

        if ( temperature != -1000.0 ) {
            // Controlar la impresión de la temperatura
            if ( print_temp ) {
                if ( xTaskGetTickCount() - last_print_time >= pdMS_TO_TICKS ( print_freq_temp * 1000 )) {
                    printf ( "Temperature: %.2f %cC\n", temperature, '\xB0' );
                    last_print_time = xTaskGetTickCount();
                }
                was_printing = true;
            } else {
                if ( was_printing ) {
                    printf ( "Temperature logging disabled.\n" );
                    was_printing = false;
                }
            }

            // Ajustar el color del LED RGB basado en la temperatura
            if ( temperature >= ranges_config.temp_red_min && temperature < ranges_config.temp_red_max ) {
                rgb_select_color_ntc ( red_intensity, LED_OFF, LED_OFF ); // Rojo
            } 

            if ( temperature >= ranges_config.temp_green_min && temperature < ranges_config.temp_green_max ) {
                rgb_select_color_ntc ( LED_OFF, green_intensity, LED_OFF ); // Verde
                
                // Amarillo (mezcla de rojo y verde) para temperaturas intermedias
                if ( temperature < ranges_config.temp_green_max && temperature > ranges_config.temp_red_min ) {
                    rgb_select_color_ntc ( red_intensity, green_intensity, LED_OFF );
                }
            } 

            if ( temperature >= ranges_config.temp_blue_min && temperature < ranges_config.temp_blue_max ) {
                rgb_select_color_ntc ( LED_OFF, LED_OFF, blue_intensity ); // Azul
                
                // Cian (mezcla de verde y azul) para temperaturas intermedias
                if ( temperature < ranges_config.temp_blue_max && temperature > ranges_config.temp_green_min ) {
                    rgb_select_color_ntc ( LED_OFF, green_intensity, blue_intensity );
                }
            }
        } else {
            printf ( "Error en la lectura del NTC\n" );
        }

        // Espera 100ms antes de la próxima actualización
        vTaskDelay ( pdMS_TO_TICKS ( 100 ));
    }
}
