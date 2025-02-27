/**
 * @file main.h
 * @brief Definiciones generales del proyecto.
 *
 * Contiene las macros para la configuración de pines y la comunicación UART.
 */

 #ifndef MAIN_H
 #define MAIN_H
 
 #include <stdio.h>
 #include <stdint.h>

 // Definiciones de pines para LED RGB
 #define LED_RED_PIN                 5
 #define LED_GREEN_PIN               19
 #define LED_BLUE_PIN                4
 
 // Definiciones de pines para segundo LED RGB
 #define LED2_RED_PIN                32
 #define LED2_GREEN_PIN              33
 #define LED2_BLUE_PIN               25
 
 // Definición del pin del botón
 #define BUTTON_PIN                  0
 
 // Configuración UART
 #define UART_TX                     1
 #define UART_RX                     3
 #define BAUD_RATE                   115200
 #define NUM_UART                    UART_NUM_0
 
 #endif // MAIN_H
 