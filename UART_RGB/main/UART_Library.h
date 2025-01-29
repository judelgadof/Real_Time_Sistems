#ifndef UART_LIBRARY_H
#define UART_LIBRARY_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/uart.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "esp_log.h"
#include <ctype.h>

#define UART_BUFFER_SIZE  1024            
#define UART_QUEUE_SIZE   20  
#define LED_ON            100
#define LED_OFF           0
#define UNCHANGED         -1


/**
 * @brief Inicializa la UART con los parámetros especificados.
 * @param tx_pin Pin GPIO para la transmisión (TX).
 * @param rx_pin Pin GPIO para la recepción (RX).
 * @param baud_rate Velocidad en baudios (ej., 115200).
 * @param uart_num Número de UART a usar (ej., UART_NUM_0).
 * @param uart_queue Puntero a la cola de eventos UART (será creada dentro de la función).
 */
void uart_init(int tx_pin, int rx_pin, int baud_rate, int uart_num, QueueHandle_t *uart_queue);

/**
 * @brief Procesa los eventos UART en un bucle infinito.
 * @param set_color_callback Función de callback para actualizar el color de los LEDs.
 * @param uart_num Número de UART a usar.
 * @param uart_queue Cola de eventos UART.
 */
void uart_process_event(void (*set_color_callback)(float, float, float), int uart_num, QueueHandle_t uart_queue);

/**
 * @brief Ejecuta un comando recibido por UART.
 * @param received_command Comando recibido en formato de cadena.
 * @param set_color_callback Función de callback para actualizar el color de los LEDs.
 */
void uart_execute_command(const char *received_command, void (*set_color_callback)(float, float, float));

#endif // MY_UART_H