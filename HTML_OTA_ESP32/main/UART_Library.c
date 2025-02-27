#include "UART_Library.h"

// Cola de eventos para la UART, utilizada para manejar eventos recibidos
QueueHandle_t uart_queue;

// Configuración inicial de los rangos de temperatura para los colores RGB
ranges_config_t ranges_config = {
    .temp_red_min   = RE_MIN,  
    .temp_red_max   = RE_MAX,  
    .temp_green_min = GR_MIN,  
    .temp_green_max = GR_MAX,  
    .temp_blue_min  = BL_MIN,  
    .temp_blue_max  = BL_MAX   
};

// Muestra los comandos disponibles en la UART
void print_help_message ( void ) {
    printf ( "\n================ COMMAND LIST ================\n" );
    printf ( "SET_INT_RED X      -> Set Red intensity (0-100)\n"  );
    printf ( "SET_INT_GREEN X    -> Set Green intensity (0-100)\n");
    printf ( "SET_INT_BLUE X     -> Set Blue intensity (0-100)\n" );
    printf ( "SET_RANGE_BLUE X Y -> Set BLUE Ranges (0-120)\n"    );
    printf ( "SET_RANGE_GREEN X Y -> Set GREEN Ranges (0-120)\n"  );
    printf ( "SET_RANGE_RED X Y -> Set RED Ranges (0-120)\n"      );
    printf ( "TEMP_ON           -> Enable temperature logging\n"  );
    printf ( "TEMP_OFF          -> Disable temperature logging\n" );
    printf ( "HELP              -> Show this help message\n"      );
    printf ( "==============================================\n\n" );
}

// Valida la temperatura asegurando que esté en el rango permitido (-10°C a 120°C)
float validate_temp ( float value ) {
    if ( value < -10 ) return -10;
    if ( value > 120 ) return 120;
    return value;
}

float validate_intensity ( float value ) {
    if ( value < -10 ) return -10;
    if ( value > 100 ) return 100;
    return value;
}

// Convierte una cadena de texto a mayúsculas
void to_uppercase ( char *str ) {
    while ( *str ) {
        if ( isalpha ( ( unsigned char ) *str )) {  
            *str = toupper ( ( unsigned char ) *str ); 
        }
        str++; 
    }
}

// Inicialización de la UART con parámetros específicos
void uart_init ( int tx_pin, int rx_pin, int baud_rate, int uart_num, QueueHandle_t *uart_queue ) {
   // Configuración de la UART con los parámetros de velocidad, paridad, flujo, etc. 
    uart_config_t uart_config = {
        .baud_rate = baud_rate,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };

    // Crea una cola de eventos para recibir información de la UART
    *uart_queue = xQueueCreate ( UART_QUEUE_SIZE, sizeof ( uart_event_t ));
    if ( *uart_queue == NULL ) {
        printf ( "Failed to create UART event queue!\n" );
        return;
    }

    // Inicializa la UART con los parámetros configurados
    uart_driver_install ( uart_num, UART_BUFFER_SIZE, UART_BUFFER_SIZE, UART_QUEUE_SIZE, uart_queue, 0 );
    uart_param_config ( uart_num, &uart_config );
    uart_set_pin ( uart_num, tx_pin, rx_pin, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE );

    // Habilita la detección de patrones en la UART
    uart_enable_pattern_det_baud_intr ( uart_num, '+', 3, 9, 0, 0 );
    uart_pattern_queue_reset ( uart_num, 20 );

    // Mensaje de bienvenida en la UART
    printf ( "\n\n===============================\n" );
    printf ( "Write HELP to see the commands\n" );
    printf ( "===============================\n\n" );
}


// Procesa eventos de la UART como recepción de datos y errores
void uart_process_event (
    int uart_num, 
    QueueHandle_t uart_queue 
) {
    uart_event_t event;
    uint8_t data[UART_BUFFER_SIZE];
    size_t buffered_size;
    int len;

    while ( 1 ) {
        if ( xQueueReceive ( uart_queue, &event, portMAX_DELAY )) {
            switch ( event.type ) {
                case UART_DATA: //  Se recibieron datos en la UART
                    len = uart_read_bytes ( uart_num, data, event.size, portMAX_DELAY );
                    data[len] = '\0';
                    //uart_execute_command ( ( char * ) data, config, print_temp, print_freq_temp, red, green, blue );
                    break;

                case UART_FIFO_OVF: //  Desbordamiento de buffer UART
                    printf ( "UART FIFO Overflow detected! Clearing buffer...\n" );
                    uart_driver_delete ( uart_num );
                    uart_flush_input ( uart_num );
                    xQueueReset ( uart_queue );
                    break;

                case UART_BUFFER_FULL: //  Buffer de la UART lleno
                    printf ( "UART Buffer Full! Resetting buffer...\n" );
                    uart_driver_delete ( uart_num );
                    uart_flush_input ( uart_num );
                    xQueueReset ( uart_queue );
                    break;

                    case UART_BREAK: //  Señal de break en la UART
                    printf("UART RX Break detected!\n");
                    break;

                case UART_PARITY_ERR: // Error de paridad
                    printf("UART Parity Error!\n");
                    break;

                case UART_FRAME_ERR: //  Error de encuadre en la UART
                    printf("UART Frame Error!\n");
                    break;

                case UART_PATTERN_DET: //  Se detectó un patrón en la UART
                    uart_get_buffered_data_len(uart_num, &buffered_size);
                    int pos = uart_pattern_pop_pos(uart_num);
                    if (pos == -1) {
                        uart_flush_input(uart_num);
                    } else {
                        if ( pos >= sizeof ( data ) ) {
                            pos = sizeof ( data ) - 1;
                        }

                        int bytesRead = uart_read_bytes(uart_num, data, pos, pdMS_TO_TICKS ( 100 ));
                        if ( bytesRead > 0 ) {
                            data[pos] = '\0';
                            printf("Pattern detected: %s\n", data);
                        }
                        
                    }
                    break;

                default: //  Evento desconocido
                    printf ( "Unknown UART event: %d\n", event.type );
                    break;
            }
        }
    }
}



// Ejecuta un comando recibido a través de la UART y configura los valores correspondientes
void uart_execute_command (
    const char *received_command, 
    ranges_config_t *config, 
    bool *print_temp, 
    int *print_freq_temp, 
    float *red, float *green, float *blue,
    int *t_on, int *t_off )
    
    {
    char command[32] = { 0 };
    float one_value = 0.0f, two_value = 0.0f;
    char command_name[32] = { 0 };

    // Copia el comando recibido a una variable local y limpia caracteres extra
    strncpy ( command, received_command, sizeof ( command ) - 1 );

    // Remueve caracteres de nueva línea y retorno de carro si están presentes
    char *newline = strchr ( command, '\n' );
    if ( newline ) *newline = '\0';
    newline = strchr ( command, '\r' );
    if ( newline ) *newline = '\0';

    // Parsea el comando recibido extrayendo el nombre y los posibles valores numéricos
    int parsed_values = sscanf ( command, "%31s %f %f", command_name, &one_value, &two_value );
    to_uppercase ( command_name );

    if ( parsed_values < 1 ) {
        printf ( "Invalid command format.\n" );
        return;
    }

    // Comandos que no requieren valores numéricos
    if ( strcmp ( command_name, "HELP" ) == 0 ) {
        print_help_message();
    } else if ( strcmp ( command_name, "TEMP_ON" ) == 0 ) {
        *print_temp = true;
    } else if ( strcmp ( command_name, "TEMP_OFF" ) == 0 ) {
        *print_temp = false;
        printf ( "Temperature logging deactivated.\n" );

    // Comandos con un valor (ej. intensidad de color)
    } else if ( parsed_values == 2 ) {
        one_value = validate_intensity ( one_value );

        if ( strcmp ( command_name, "SET_INT_RED" ) == 0 ) {
            *red = one_value;
        } else if ( strcmp ( command_name, "SET_INT_GREEN" ) == 0 ) {
            *green = one_value;
        } else if ( strcmp ( command_name, "SET_INT_BLUE" ) == 0 ) {
            *blue = one_value;
        }  else if ( strcmp ( command_name, "ON_TIME" ) == 0 ) {
            *t_on = one_value;
        } else if ( strcmp ( command_name, "OFF_TIME" ) == 0 ) {
            *t_off = one_value;
        } else {
            printf ( "Unknown command: %s\n", command_name );
        }

    // Comandos con dos valores (ej. rango de temperatura)
    } else if ( parsed_values == 3 ) {
        one_value = validate_temp ( one_value );
        two_value = validate_temp ( two_value );

        if ( one_value >= two_value ) {
            printf ( "Invalid range! Min must be lower than Max.\n" );
            return;
        }

        if ( strcmp ( command_name, "SET_RANGE_RED" ) == 0 ) {
            config->temp_red_min = one_value;
            config->temp_red_max = two_value;
        } else if ( strcmp ( command_name, "SET_RANGE_GREEN" ) == 0 ) {
            config->temp_green_min = one_value;
            config->temp_green_max = two_value;
        } else if ( strcmp ( command_name, "SET_RANGE_BLUE" ) == 0 ) {
            config->temp_blue_min = one_value;
            config->temp_blue_max = two_value;
        } else {
            printf ( "Unknown command: %s\n", command_name );
        }
    }
}

/*
// Procesa eventos de la UART como recepción de datos y errores
void uart_process_event ( int uart_num, QueueSetHandle_t uart_queue ) {

    uart_event_t event;
    uint8_t data[UART_BUFFER_SIZE];
    size_t buffered_size;
    int len;

    while ( 1 ) {
        printf ( "Esperando evento" );
        if ( xQueueReceive ( uart_queue, &event, portMAX_DELAY )) {
            switch ( event.type ) {
                case UART_DATA: //  Se recibieron datos en la UART
                    printf ( "Entre al la evento" );
                    len = uart_read_bytes ( uart_num, data, event.size, portMAX_DELAY );
                    data[len] = '\0';

                    // Enviar los datos a la cola de mensajes
                    if ( xQueueSend ( uart_data_queue, data, portMAX_DELAY ) != pdPASS ) {
                        printf ( "Error: No se pudo enviar el mensaje a la cola.\n" );
                    }
                    break;

                case UART_FIFO_OVF: //  Desbordamiento de buffer UART
                    printf ( "UART FIFO Overflow detected! Clearing buffer...\n" );
                    uart_driver_delete ( uart_num );
                    uart_flush_input ( uart_num );
                    xQueueReset ( uart_queue );
                    break;

                case UART_BUFFER_FULL: //  Buffer de la UART lleno
                    printf ( "UART Buffer Full! Resetting buffer...\n" );
                    uart_driver_delete ( uart_num );
                    uart_flush_input ( uart_num );
                    xQueueReset ( uart_queue );
                    break;

                    case UART_BREAK: //  Señal de break en la UART
                    printf("UART RX Break detected!\n");
                    break;

                case UART_PARITY_ERR: // Error de paridad
                    printf("UART Parity Error!\n");
                    break;

                case UART_FRAME_ERR: //  Error de encuadre en la UART
                    printf("UART Frame Error!\n");
                    break;

                case UART_PATTERN_DET: //  Se detectó un patrón en la UART
                    uart_get_buffered_data_len(uart_num, &buffered_size);
                    int pos = uart_pattern_pop_pos(uart_num);
                    if (pos == -1) {
                        uart_flush_input(uart_num);
                    } else {
                        if ( pos >= sizeof ( data ) ) {
                            pos = sizeof ( data ) - 1;
                        }

                        int bytesRead = uart_read_bytes(uart_num, data, pos, pdMS_TO_TICKS ( 100 ));
                        if ( bytesRead > 0 ) {
                            data[pos] = '\0';
                            printf("Pattern detected: %s\n", data);
                        }
                        
                    }
                    break;

                default: //  Evento desconocido
                    printf ( "Unknown UART event: %d\n", event.type );
                    break;
            }
        }
    }
}
*/