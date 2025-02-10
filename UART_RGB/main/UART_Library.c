#include "UART_Library.h"

// Definir las colas para datos UART
QueueHandle_t uart_data_queue;
QueueHandle_t uart_event_queue;

// Inicializar la estructura con valores predeterminados
ranges_config_t ranges_config = {
    .temp_red_min   = R_MIN,
    .temp_red_max   = R_MAX,
    .temp_green_min = G_MIN,
    .temp_green_max = G_MAX,
    .temp_blue_min  = B_MIN,
    .temp_blue_max  = B_MAX
};

void print_help_message (void) {
    printf ( "\n================ COMMAND LIST ================\n" );
    printf ( "SET_INT_RED X      -> Set Red intensity (0-100)\n" );
    printf ( "SET_INT_GREEN X    -> Set Green intensity (0-100)\n" );
    printf ( "SET_INT_BLUE X     -> Set Blue intensity (0-100)\n" );
    printf ( "SET_RANGE_BLUE X Y -> Set BLUE Ranges (0-120)\n" );
    printf ( "SET_RANGE_GREEN X Y -> Set GREEN Ranges (0-120)\n" );
    printf ( "SET_RANGE_RED X Y -> Set RED Ranges (0-120)\n" );
    printf ( "HELP           -> Show this help message\n" );
    printf ( "==============================================\n\n" );
}

float validate_intensity ( float value ) {
    if ( value < LED_OFF ) return LED_OFF;
    if ( value > LED_ON ) return LED_ON;
    return value;
}

float validate_temp ( float value ) {
    if ( value < -10 ) return -10;
    if ( value > 120 ) return 120;
    return value;
}

/**
 * @brief Convierte una cadena a mayúsculas.
 */
void to_uppercase(char *str) {
    while (*str) {
        if (isalpha((unsigned char)*str)) {
            *str = toupper((unsigned char)*str);
        }
        str++;
    }
}

/**
 * @brief Inicializa la UART con los parámetros especificados.
 */
void uart_init(int tx_pin, int rx_pin, int baud_rate, int uart_num, QueueHandle_t *uart_queue) {
    uart_config_t uart_config = {
        .baud_rate = baud_rate,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };

    if (uart_data_queue == NULL) {
        uart_data_queue = xQueueCreate(UART_QUEUE_SIZE, UART_BUFFER_SIZE);
    }
    if (uart_event_queue == NULL) {
        uart_event_queue = xQueueCreate(UART_QUEUE_SIZE, sizeof(uart_event_t));
    }

    if (uart_data_queue == NULL || uart_event_queue == NULL) {
        printf("Failed to create UART queues!\n");
        return;
    }

    if (uart_driver_install(uart_num, UART_BUFFER_SIZE, UART_BUFFER_SIZE, UART_QUEUE_SIZE, &uart_event_queue, 0) != ESP_OK) {
        printf("UART driver installation failed!\n");
        return;
    }
    
    uart_param_config(uart_num, &uart_config);
    uart_set_pin(uart_num, tx_pin, rx_pin, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

    *uart_queue = uart_event_queue;
    printf("UART initialized on TX: %d, RX: %d, Baudrate: %d\n", tx_pin, rx_pin, baud_rate);
}

/**
 * @brief Procesa eventos UART y envía los datos a una cola.
 */
void uart_process_event(void *arg) {
    uart_event_params_t *params = (uart_event_params_t *)arg;
    int uart_num = params->uart_num;
    QueueHandle_t queue = params->uart_queue;

    uart_event_t event;
    uint8_t data[UART_BUFFER_SIZE];

    while (1) {
        if (xQueueReceive(queue, &event, portMAX_DELAY)) {
            switch (event.type) {
                case UART_DATA: {
                    int len = uart_read_bytes(uart_num, data, sizeof(data) - 1, portMAX_DELAY);
                    if (len > 0) {
                        data[len] = '\0'; // Terminar cadena
                        printf("Raw UART data: '%s'\n", data); // Para depuración
                        if (xQueueSend(uart_data_queue, data, portMAX_DELAY) != pdPASS) {
                            printf("Error: Failed to enqueue UART data.\n");
                        }
                    } else {
                        printf("Error: No data received from UART.\n");
                    }
                    break;
                }
                default:
                    printf("UART event type: %d\n", event.type);
                    break;
            }
        }
    }
}



/**
 * @brief Ejecuta un comando recibido.
 */
void uart_execute_command(const char *received_command, ranges_config_t *config, bool *print_temp, float *red, float *green, float *blue) {
    char command[32] = {0};
    float one_value = 0.0f, two_value = 0.0f;

    // Limpiar los caracteres extraños del comando recibido
    strncpy(command, received_command, sizeof(command) - 1);
    char *newline = strchr(command, '\n');
    if (newline) *newline = '\0'; // Remover salto de línea
    newline = strchr(command, '\r');
    if (newline) *newline = '\0'; // Remover retorno de carro

    printf("Processing command: '%s'\n", command); // Depuración

    int parsed_values = sscanf(command, "%31s %f %f", command, &one_value, &two_value);
    to_uppercase(command);

    if (parsed_values < 1) {
        printf("Invalid command format.\n");
        return;
    }

    // Procesar el comando...
}
