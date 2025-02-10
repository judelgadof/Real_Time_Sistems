#include "tasks.h"

//volatile int arg = 100;  // Intensidad global (0-100)
QueueHandle_t button_queue;

// ADC para el potenciómetro
adc_t potentiometer_adc;

void potentiometer_RGB_task ( void *arg ) {
    led_color_t current_color = COLOR_RED;
    int last_button_state = 1;
    int brightness = 0;  // Intensidad global (0-100)

    int red_intensity   = 0; // Guardar la intensidad del rojo
    int green_intensity = 0; // Guardar la intensidad del verde
    int blue_intensity  = 0; // Guardar la intensidad del azul

    while (1) {
        // ---- LECTURA DEL BOTÓN ----
        int current_button_state = gpio_get_level ( BUTTON_PIN );
        if ( last_button_state == 1 && current_button_state == 0 ) {
            current_color = ( current_color + 1 ) % COLOR_COUNT;
            printf( "Button Pressed. New color: %d\n", current_color );
        }
        last_button_state = current_button_state;

        // ---- LECTURA DEL POTENCIÓMETRO ----
        int raw_value = adc_read_raw ( &potentiometer_adc );
        brightness = ( raw_value * 100 ) / (( 1 << ADC_BITWIDTH_12 ) - 1 ); // Escalar a 0-100

        // ---- ACTUALIZACIÓN DEL LED ----
        switch ( current_color ) {
            case COLOR_RED:
                rgb_select_color ( brightness, 0, 0 );
                printf ( "Red Brightness : %d%%\n ", brightness );
                red_intensity = brightness;
                break;

            case COLOR_GREEN:
                rgb_select_color ( 0, brightness, 0 );
                printf ( "Green Brightness : %d%%\n ", brightness );
                green_intensity = brightness;
                break;

            case COLOR_BLUE:
                rgb_select_color ( 0, 0, brightness );
                printf ( "Blue Brightness : %d%%\n ", brightness );
                blue_intensity = brightness;
                break;

            case COLOR_NEUTRO:
                rgb_select_color ( red_intensity, green_intensity, blue_intensity );
                printf ( "Saved Brightness - Red: %d%%, Green: %d%%, Blue: %d%%\n",
                       red_intensity, green_intensity, blue_intensity );
                break;

            default:
                break;
        }

        vTaskDelay ( pdMS_TO_TICKS ( 50 ));
    }
}