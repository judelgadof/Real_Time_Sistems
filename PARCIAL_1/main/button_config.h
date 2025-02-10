/**
 * @file button_config.h
 * @brief Configuración de botones en GPIO.
 *
 * Esta librería proporciona una función para inicializar un botón 
 * configurando su pin como entrada con resistencia pull-up.
 */

 #ifndef BUTTON_CONFIG_H
 #define BUTTON_CONFIG_H
 
 #include "driver/gpio.h"
 
 /**
  * @brief Inicializa un botón configurando su pin como entrada con pull-up.
  * 
  * @param button_pin Pin GPIO asignado al botón.
  */
 void button_init ( const uint8_t button_pin );
 
 #endif // BUTTON_CONFIG_H
 