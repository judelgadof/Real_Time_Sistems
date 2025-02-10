#ifndef UART_LIBRARY_H
#define UART_LIBRARY_H

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "driver/uart.h"
#include <ctype.h>  


#define UART_NUM             UART_NUM_0
#define UART_QUEUE_SIZE     10
#define UART_BUFFER_SIZE    256
#define LED_ON              100
#define LED_OFF             0
#define R_MAX               100
#define R_MIN               30
#define G_MAX               30
#define G_MIN               18
#define B_MAX               18
#define B_MIN               0


typedef struct {
    int uart_num;
    QueueHandle_t uart_queue;
} uart_event_params_t;

typedef struct {
    float temp_red_min;
    float temp_red_max;
    float temp_green_min;
    float temp_green_max;
    float temp_blue_min;
    float temp_blue_max;
} ranges_config_t;



// Definir la cola para datos UART
extern QueueHandle_t uart_data_queue;
extern QueueHandle_t uart_queue;

/**
 * @brief Inicializa la UART con los parámetros especificados.
 * @param tx_pin Pin de transmisión.
 * @param rx_pin Pin de recepción.
 * @param baud_rate Velocidad de transmisión en baudios.
 * @param uart_num Número de UART.
 * @param uart_queue Puntero a la cola de eventos UART.
 */
void uart_init(int tx_pin, int rx_pin, int baud_rate, int uart_num, QueueHandle_t *uart_queue);

/**
 * @brief Procesa eventos UART y envía los datos a una cola.
 * @param arg Parámetros de la UART.
 */
void uart_process_event(void *arg);

void uart_execute_command(const char *received_command, ranges_config_t *config, bool *print_temp, float *red, float *green, float *blue);


#endif // UART_LIBRARY_H