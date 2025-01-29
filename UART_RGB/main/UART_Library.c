#include "UART_Library.h"

//static const char *TAG = "UART_LIBRARY";


void print_help_message (void) {
    printf ( "\n================ COMMAND LIST ================\n" );
    printf ( "RED            -> Set color to Red\n" );
    printf ( "GREEN          -> Set color to Green\n" );
    printf ( "BLUE           -> Set color to Blue\n" );
    printf ( "SET_RED X      -> Set Red intensity (0-100)\n" );
    printf ( "SET_GREEN X    -> Set Green intensity (0-100)\n" );
    printf ( "SET_BLUE X     -> Set Blue intensity (0-100)\n" );
    printf ( "Show the current color values\n" );
    printf ( "HELP           -> Show this help message\n" );
    printf ( "==============================================\n\n" );
}

// 🔹 Valida valores de intensidad (0-100)
float validate_intensity ( float value ) {
    if ( value < 0 ) return 0;
    if ( value > 100 ) return 100;
    return value;
}

// 🔹 Convierte una cadena a mayúsculas
void to_uppercase ( char *str ) {
    while ( *str ) {
        *str = toupper (( unsigned char ) *str );
        str++;
    }
}


// Inicialización de UART
void uart_init ( int tx_pin, int rx_pin, int baud_rate, int uart_num, QueueHandle_t *uart_queue ) {
    uart_config_t uart_config = {
        .baud_rate = baud_rate,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };

    *uart_queue = xQueueCreate ( UART_QUEUE_SIZE, sizeof ( uart_event_t ));
    if ( *uart_queue == NULL ) {
        printf ( "Failed to create UART event queue!\n" );
        return;
    }

    uart_driver_install ( uart_num, UART_BUFFER_SIZE, UART_BUFFER_SIZE, UART_QUEUE_SIZE, uart_queue, 0 );
    uart_param_config ( uart_num, &uart_config );
    uart_set_pin ( uart_num, tx_pin, rx_pin, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE );

    // Configurar detección de patrones
    uart_enable_pattern_det_baud_intr ( uart_num, '+', 3, 9, 0, 0 );
    uart_pattern_queue_reset ( uart_num, 20 );

    printf ( "\n\n===============================\n" );
    printf ( "Write HELP to see the commands\n" );
    printf ( "===============================\n\n" );

    //esp_log_level_set ( "UART_LIBRARY", ESP_LOG_NONE );  // Desactivar logs no deseados
}


// 🔹 Procesa eventos UART
void uart_process_event ( void ( *set_color_callback )( float, float, float ), int uart_num, QueueHandle_t uart_queue ) {
    uart_event_t event;
    uint8_t data [ UART_BUFFER_SIZE ];
    size_t buffered_size;

    while (1) {
        if ( xQueueReceive ( uart_queue, &event, portMAX_DELAY )) {
            switch ( event.type ) {
                case UART_DATA: {
                    int len = uart_read_bytes (uart_num, data, event.size, portMAX_DELAY );
                    data [ len ] = '\0';  // Convertir a cadena terminada
                    uart_execute_command (( char * ) data, set_color_callback );
                    break;
                }

                case UART_FIFO_OVF:
                    printf ( "UART FIFO Overflow detected! Clearing buffer...\n" );
                    uart_flush_input ( uart_num );
                    xQueueReset ( uart_queue );
                    break;

                case UART_BUFFER_FULL:
                    printf ( "UART Buffer Full! Resetting buffer...\n" );
                    uart_flush_input ( uart_num );
                    xQueueReset ( uart_queue );
                    break;

                case UART_BREAK:
                    printf ( "UART RX Break detected!\n" );
                    break;

                case UART_PARITY_ERR:
                    printf ( "UART Parity Error!\n" );
                    break;

                case UART_FRAME_ERR:
                    printf ( "UART Frame Error!\n" );
                    break;

                case UART_PATTERN_DET:
                    uart_get_buffered_data_len ( uart_num, &buffered_size );
                    int pos = uart_pattern_pop_pos ( uart_num );
                    if ( pos == -1 ) {
                        uart_flush_input ( uart_num );
                    } else {
                        uart_read_bytes ( uart_num, data, pos, 100 / portTICK_PERIOD_MS );
                        data [ pos ] = '\0';
                        printf ( "Pattern detected: %s\n", data );
                    }
                    break;

                default:
                    printf ( "Unknown UART event: %d\n", event.type );
                    break;
            }
        }
    }
}


// 🔹 Ejecuta un comando recibido
void uart_execute_command ( const char *received_command, void ( *set_color_callback )( float, float, float )) {
    char command [ 32 ];
    char params [ 32 ] = "";  

    sscanf ( received_command, "%31s %31[^\n]", command, params );
    to_uppercase ( command ) ;

    float value = atof ( params );
    value = validate_intensity ( value );  

    if ( strcmp(command, "RED" ) == 0 ) {
        set_color_callback ( LED_ON, LED_OFF, LED_OFF );

    } else if ( strcmp ( command, "GREEN" ) == 0 ) {
        set_color_callback ( LED_OFF, LED_ON, LED_OFF );

    } else if ( strcmp ( command, "BLUE" ) == 0 ) {
        set_color_callback ( LED_OFF, LED_OFF, LED_ON );

    } else if ( strcmp ( command, "SET_RED" ) == 0 ) {
        set_color_callback ( value, UNCHANGED, UNCHANGED );

    } else if ( strcmp ( command, "SET_GREEN" ) == 0 ) {
        set_color_callback ( UNCHANGED, value, UNCHANGED );

    } else if ( strcmp ( command, "SET_BLUE" ) == 0 ) {
        set_color_callback ( UNCHANGED, UNCHANGED, value );

    } else if ( strcmp ( command, "HELP" ) == 0 ) {
        print_help_message ();
   
    } else {
        printf ( "Unknown command: %s", received_command);
    }
}

