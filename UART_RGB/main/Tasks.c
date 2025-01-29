#include "tasks.h"

//volatile int arg = 100;  // Intensidad global (0-100)
QueueHandle_t button_queue;
QueueHandle_t uart_queue;

// ADC para el potenciómetro
adc_t potentiometer_adc;

void button_task(void *arg) {
    led_color_t color = COLOR_RED;
    int last_button_state = 1;

    while (1) {
        int current_button_state = gpio_get_level ( BUTTON_PIN );

        if (last_button_state == 1 && current_button_state == 0) {
            color = (color + 1) % COLOR_COUNT;
            xQueueSend(button_queue, &color, portMAX_DELAY);
            printf("Button Pressed. New color: %d\n", color);
        }

        last_button_state = current_button_state;
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

void led_task(int *arg) {
    led_color_t current_color = COLOR_RED;
    int red_intensity   = 0;   // Guardar la intensidad del rojo
    int green_intensity = 0; // Guardar la intensidad del verde
    int blue_intensity  = 0;  // Guardar la intensidad del azul

    while (1) {
        if (xQueueReceive(button_queue, &current_color, 0)) {
switch (current_color) {
                case COLOR_RED:
                    rgb_select_color ( 100, 0, 0);
                    vTaskDelay(pdMS_TO_TICKS(800));
                    break;
                case COLOR_GREEN:
                    rgb_select_color ( 0, 100, 0);
                    vTaskDelay(pdMS_TO_TICKS(800));
                    break;
                case COLOR_BLUE:
                    rgb_select_color ( 0, 0, 100);
                    vTaskDelay(pdMS_TO_TICKS(800));
                    break;
                case COLOR_NEUTRO:
                    rgb_select_color(red_intensity, green_intensity, blue_intensity);
                    // Imprimir los valores guardados de cada color
                    printf("Saved Brightness - Red: %d%%, Green: %d%%, Blue: %d%%\n",
                           red_intensity, green_intensity, blue_intensity);
                    break;
                default:
                    break;
            }
        }
            switch (current_color) {
                case COLOR_RED:
                    rgb_select_color ( *arg, 0, 0);
                    printf("Red Brightness : %d%%\n ", *arg);
                    red_intensity = *arg;
                    break;
                case COLOR_GREEN:
                    rgb_select_color (0, *arg, 0);
                    printf("Green Brightness : %d%%\n ", *arg);
                    green_intensity = *arg;
                    break;
                case COLOR_BLUE:
                    rgb_select_color (0, 0, *arg);
                    printf("Blue Brightness : %d%%\n ", *arg);
                    blue_intensity = *arg;
                    break;
                case COLOR_NEUTRO:
                    rgb_select_color(red_intensity, green_intensity, blue_intensity);
                    break;
                default:
                    break;
            }
        
        //printf ( "Intensidad ajustada: %d%% \n", *arg);
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}


/**
 * @brief Tarea para leer el valor del potenciómetro y actualizar la intensidad global.
 */
void potentiometer_task ( int *arg ) {
    *arg = 0;

    while (1) {
        int raw_value = adc_read_raw ( &potentiometer_adc );

        // Mapear el valor crudo al rango 0-100
        *arg = ( raw_value * 100 ) /((1 << ADC_BITWIDTH_12) - 1);

        //printf ( "Intensidad ajustada: %d%% (valor crudo: %d)\n", *arg, raw_value );
        vTaskDelay ( pdMS_TO_TICKS ( 100 ));
    }
}

void uart_task ( void *arg ) {

    uart_process_event ( set_rgb_color , NUM_UART, uart_queue );
}

// 🔹 Callback para actualizar los LEDs según el comando recibido
void set_rgb_color ( float red_t, float green_t, float blue_t ) {
    static float red = LED_OFF, green = LED_OFF, blue = LED_OFF;

    if ( red_t   != UNCHANGED ) red   = red_t;
    if ( green_t != UNCHANGED ) green = green_t;
    if ( blue_t  != UNCHANGED ) blue  = blue_t;

    rgb_select_color(red, green, blue);
    printf("Updated color: R=%.2f, G=%.2f, B=%.2f\n", red, green, blue);
}