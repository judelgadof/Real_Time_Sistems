#include "tasks.h"



// Cola de mensajes para gestionar eventos de botón
QueueHandle_t button_queue;
QueueHandle_t server_data_queue;

// Estructura de configuración del ADC para el potenciómetro
adc_t adc_two;

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
void rgb_select_color_http(int red, int green, int blue) {
    // Normalizar los valores RGB (0-255) a un rango de 0-100%
    float red_normalized   = (float)red   / 255.0f * 100.0f;
    float green_normalized = (float)green / 255.0f * 100.0f;
    float blue_normalized  = (float)blue  / 255.0f * 100.0f;

    // Llamar a rgb_select_color con los valores normalizados
    rgb_select_color(&led1_config, red_normalized, green_normalized, blue_normalized);
}

// Selecciona el color del LED RGB 2 basado en el sensor NTC
void rgb_select_color_ntc ( float red, float green, float blue ) {
    rgb_select_color ( &led2_config, red, green, blue );
}

static void time_toggle(int *tim_on, int *tim_off) {
    static int copy_Q_on  = 0;
    static int copy_Q_off = 0;
    static bool flag = false;

    server_data_t server_data;

    // Revisar la cola solo si hay datos disponibles
    if (xQueuePeek(server_data_queue, &server_data, pdMS_TO_TICKS(100)) == pdPASS) {
        if (server_data.time_on != copy_Q_on) {
            copy_Q_on = server_data.time_on;
            flag = true;
        }
        if (server_data.time_off != copy_Q_off) {
            copy_Q_off = server_data.time_off;
            flag = true;
        }
    }

    if (use_web_values) {
        // Usar valores de la web
        *tim_on  = on_time_val;
        *tim_off = off_time_val;
        use_web_values = false;  // Restablecer la flag después de usarlos
    } 
    if(flag == true){
        // Usar los valores actualizados de la cola
        *tim_on  = copy_Q_on;
        *tim_off = copy_Q_off;
        flag = false;
    }
}



void print_temperature_task(void *arg) {
    server_data_t server_data;          // Estructura para almacenar datos de la cola
    TickType_t last_toggle_time = xTaskGetTickCount();  // Tiempo del último cambio de estado del LED
    bool led_state = false;             // Estado actual del LED (encendido/apagado)
    int tim_on, tim_off;                // Tiempos de encendido y apagado del LED

    while (1) {
        // --- Lectura de la temperatura ---
        if (temp_printing_enabled) {
            // Leer la temperatura desde la cola (si está disponible)
            if (xQueuePeek(server_data_queue, &server_data, pdMS_TO_TICKS(100))) {
                printf("Temperature: %.2f°C\n", server_data.temperature_http);  // Imprimir en la terminal
            }
        }

        // --- Obtener los tiempos de encendido y apagado ---
        time_toggle(&tim_on, &tim_off);  // Actualizar tim_on y tim_off
        printf("on: %d, off: %d", tim_on, tim_off);

        // --- Control del LED RGB ---
        TickType_t current_time = xTaskGetTickCount();  // Obtener el tiempo actual

        if (led_state) {
            // Si el LED está encendido, verificar si es hora de apagarlo
            if ((current_time - last_toggle_time) >= (tim_on * 1000 / portTICK_PERIOD_MS)) {
                rgb_select_color_http(LED_OFF, LED_OFF, LED_OFF);  // Apagar el LED
                led_state = false;                                 // Cambiar el estado del LED
                last_toggle_time = current_time;                   // Actualizar el tiempo del último cambio
            }
        } else {
            // Si el LED está apagado, verificar si es hora de encenderlo
            if ((current_time - last_toggle_time) >= (tim_off * 1000 / portTICK_PERIOD_MS)) {
                rgb_select_color_http(red_val_http, green_val_http, blue_val_http);  // Encender el LED
                led_state = true;                                                    // Cambiar el estado del LED
                last_toggle_time = current_time;                                     // Actualizar el tiempo del último cambio
            }
        }

        // --- Esperar un breve período ---
        vTaskDelay(10 / portTICK_PERIOD_MS);  // Evitar el uso excesivo de la CPU
    }
}

// Tarea que maneja la comunicación UART y el LED RGB según la temperatura del NTC
void ntc_uart_task ( void *arg ) {
    char received_data[UART_BUFFER_SIZE];
    bool print_temp = false; 
    bool was_printing = false;
    int print_freq_temp = 1;
    server_data_t server_data;

    // Intensidades iniciales del LED RGB
    float red_intensity = LED_ON, green_intensity = LED_ON, blue_intensity = LED_ON;
    int t_on = 1, t_off = 1;
    TickType_t last_print_time = xTaskGetTickCount();

    while ( 1 ) {
        // Leer datos desde la UART
        int length = uart_read_bytes ( NUM_UART, ( uint8_t * ) received_data, sizeof ( received_data ) - 1, pdMS_TO_TICKS ( 10 ));
        
        if ( length > 0 ) {
            received_data[length] = '\0'; // Asegurar que la cadena termina correctamente
            uart_execute_command ( received_data, &ranges_config, &print_temp, &print_freq_temp, &red_intensity, &green_intensity, &blue_intensity, &t_on, &t_off );
        }

        // Leer la temperatura del sensor NTC
        float temperature = ntc_read_temperature();
        // Leer y normalizar el valor del potenciómetro (0-100)
        int raw_value = adc_read_raw_multi ( &adc_two, 0 );
        
        float voltage = (raw_value * MAX_VOLTAGE) / ((1 << ADC_BITWIDTH_12) - 1);
        int brightness = (raw_value * MAX_INTENSITY) / ((1 << ADC_BITWIDTH_12) - 1);
        //ESP_LOGI("POTENTIOMETER", "Voltage: %.2f, Brightness: %d", voltage, brightness);

        // Obtener la última estructura de la cola (si existe)
        xQueuePeek(server_data_queue, &server_data, 0);
        server_data.voltage_http = voltage;  // Actualizar solo el voltaje
        server_data.time_on  = t_on;
        server_data.time_off = t_off;
        // Enviar la estructura actualizada a la cola
        xQueueOverwrite(server_data_queue, &server_data);

        if ( temperature != -1000.0 ) {

            xQueuePeek(server_data_queue, &server_data, 0);
            server_data.temperature_http = temperature;  // Actualizar solo la temperatura

            // Enviar la estructura actualizada a la cola
            xQueueOverwrite(server_data_queue, &server_data);

            // Controlar la impresión de la temperatura
            if ( print_temp ) {
                if ( xTaskGetTickCount() - last_print_time >= pdMS_TO_TICKS ( print_freq_temp * 1000 )) {
                    ESP_LOGI ( "TEMPERATURE", "%.2f %cC", temperature, '\xB0' );
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
                //rgb_select_color_potentiometer ( brightness, LED_OFF, LED_OFF );
                rgb_select_color_ntc ( brightness, LED_OFF, LED_OFF );
            } 

            if ( temperature >= ranges_config.temp_green_min && temperature < ranges_config.temp_green_max ) {
                rgb_select_color_ntc ( LED_OFF, green_intensity, LED_OFF ); // Verde
                //rgb_select_color_potentiometer ( LED_OFF, brightness, LED_OFF );
                rgb_select_color_ntc ( LED_OFF, brightness, LED_OFF );
                
                // Amarillo (mezcla de rojo y verde) para temperaturas intermedias
                if ( temperature < ranges_config.temp_green_max && temperature > ranges_config.temp_red_min ) {
                    rgb_select_color_ntc ( red_intensity, green_intensity, LED_OFF );
                    //rgb_select_color_potentiometer ( brightness, brightness, LED_OFF );
                    rgb_select_color_ntc ( brightness, brightness, LED_OFF );
                }
            } 

            if ( temperature >= ranges_config.temp_blue_min && temperature < ranges_config.temp_blue_max ) {
                rgb_select_color_ntc ( LED_OFF, LED_OFF, blue_intensity ); // Azul
               // rgb_select_color_potentiometer ( LED_OFF, LED_OFF, brightness );
                rgb_select_color_ntc ( LED_OFF, LED_OFF, brightness );
                
                // Cian (mezcla de verde y azul) para temperaturas intermedias
                if ( temperature < ranges_config.temp_blue_max && temperature > ranges_config.temp_green_min ) {
                    rgb_select_color_ntc ( LED_OFF, green_intensity, blue_intensity );
                    //rgb_select_color_potentiometer ( LED_OFF, brightness, brightness );
                    rgb_select_color_ntc ( LED_OFF, brightness, brightness );
                }
            }
        } else {
            printf ( "Error en la lectura del NTC\n" );
        }

        // Espera 100ms antes de la próxima actualización
        vTaskDelay ( pdMS_TO_TICKS ( 100 ));
    }
}

void configure_led(void) {
    gpio_reset_pin(BLINK_GPIO);
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);
    gpio_set_level(BLINK_GPIO, 0);  // Inicializar el LED apagado
    ESP_LOGI(TAG_TO, "LED configured on GPIO %d", BLINK_GPIO);  // Log para depuración
}

/*
void uart_process_task ( void *arg ) {

    uint8_t data[UART_BUFFER_SIZE];
    bool print_temp = false; 
    bool was_printing = false;
    int print_freq_temp = 1;

    // Intensidades iniciales del LED RGB
    float red_intensity = LED_OFF, green_intensity = LED_OFF, blue_intensity = LED_OFF;
    TickType_t last_print_time = xTaskGetTickCount();

    while ( 1 ) {

        if ( xQueueReceive ( uart_data_queue, data, portMAX_DELAY ) ){
            printf ( "Entre al la cola" );
            //Procesar comando  UART
            uart_execute_command ( (char *)data, &ranges_config, &print_temp, &print_freq_temp,
                                     &red_intensity, &green_intensity, &blue_intensity );

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
*/