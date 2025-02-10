/**
 * @file UART_Library.h
 * @brief Librería para la comunicación UART en ESP32..
 *
 * Proporciona funciones para inicializar la UART, procesar eventos y ejecutar 
 * comandos recibidos. También maneja la validación de datos y la conversión de texto.
 */

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
 #include "tasks.h"
 
 // Configuración de UART
 #define UART_BUFFER_SIZE  2048  ///< Tamaño del buffer de UART
 #define UART_QUEUE_SIZE   20    ///< Tamaño de la cola de mensajes UART
 #define LED_ON            100   ///< Valor para encender el LED (100%)
 #define LED_OFF           0     ///< Valor para apagar el LED (0%)
 
 // Rango de colores
 #define R_MAX            100  ///< Máximo valor para el canal rojo
 #define R_MIN            30   ///< Mínimo valor para el canal rojo
 #define G_MAX            30   ///< Máximo valor para el canal verde
 #define G_MIN            18   ///< Mínimo valor para el canal verde
 #define B_MAX            18   ///< Máximo valor para el canal azul
 #define B_MIN            0    ///< Mínimo valor para el canal azul
 
 /**
  * @brief Estructura para manejar los rangos de temperatura del LED.
  */
 typedef struct {
     float temp_red_min;   ///< Límite inferior para color rojo
     float temp_red_max;   ///< Límite superior para color rojo
     float temp_green_min; ///< Límite inferior para color verde
     float temp_green_max; ///< Límite superior para color verde
     float temp_blue_min;  ///< Límite inferior para color azul
     float temp_blue_max;  ///< Límite superior para color azul
 } ranges_config_t;
 
 extern ranges_config_t ranges_config;
 
 /** 
  * @brief Inicializa la UART con los parámetros especificados.
  * 
  * @param tx_pin Pin de transmisión (TX).
  * @param rx_pin Pin de recepción (RX).
  * @param baud_rate Velocidad de transmisión en baudios.
  * @param uart_num Número de UART utilizada.
  * @param uart_queue Puntero a la cola de mensajes UART.
  */
 void uart_init ( int tx_pin, int rx_pin, int baud_rate, int uart_num, QueueHandle_t *uart_queue );
 
 /**
  * @brief Procesa eventos UART en un bucle infinito.
  * 
  * @param config Configuración de rangos de temperatura.
  * @param print_temp Puntero a bandera de impresión de temperatura.
  * @param print_freq_temp Puntero a la frecuencia de impresión de temperatura.
  * @param red Puntero a la intensidad del LED rojo.
  * @param green Puntero a la intensidad del LED verde.
  * @param blue Puntero a la intensidad del LED azul.
  * @param uart_num Número de UART utilizada.
  * @param uart_queue Cola de eventos de UART.
  */
 void uart_process_event ( ranges_config_t *config, bool *print_temp, int *print_freq_temp, 
                           float *red, float *green, float *blue, int uart_num, QueueHandle_t uart_queue );
 
 /**
  * @brief Ejecuta un comando recibido por UART.
  * 
  * @param received_command Comando recibido.
  * @param config Configuración de rangos de temperatura.
  * @param print_temp Puntero a bandera de impresión de temperatura.
  * @param print_freq_temp Puntero a la frecuencia de impresión de temperatura.
  * @param red Puntero a la intensidad del LED rojo.
  * @param green Puntero a la intensidad del LED verde.
  * @param blue Puntero a la intensidad del LED azul.
  */
 void uart_execute_command ( const char *received_command, ranges_config_t *config, 
                             bool *print_temp, int *print_freq_temp, float *red, float *green, float *blue );
 
 /**
  * @brief Muestra los comandos disponibles en la UART.
  */
 void print_help_message ( void );
 
 /**
  * @brief Valida la intensidad de los LEDs (0-100).
  * 
  * @param value Valor a validar.
  * @return Valor corregido dentro del rango permitido.
  */
 float validate_intensity ( float value );
 
 /**
  * @brief Valida la temperatura en el rango permitido (-10 a 120).
  * 
  * @param value Valor de temperatura a validar.
  * @return Temperatura corregida dentro del rango permitido.
  */
 float validate_temp ( float value );
 
 /**
  * @brief Convierte una cadena de texto a mayúsculas.
  * 
  * @param str Puntero a la cadena de texto a convertir.
  */
 void to_uppercase ( char *str );
 
 #endif // UART_LIBRARY_H
 