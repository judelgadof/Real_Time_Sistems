/**
 * @file NTC_Library.h
 * @brief Librería para la lectura de temperatura con un sensor NTC.
 *
 * Proporciona funciones para leer la temperatura desde un NTC usando un ADC y 
 * define parámetros clave basados en el datasheet del sensor.
 */

 #ifndef NTC_LIBRARY_H
 #define NTC_LIBRARY_H
 
 #include "ADC_manager.h"
 #include <math.h>
 #include "freertos/FreeRTOS.h"
 #include "freertos/task.h"
 
 // Parámetros del NTC (ajustar según el datasheet del sensor)
 #define NTC_BETA     3950    ///< Coeficiente Beta del termistor
 #define NTC_R25      10000   ///< Resistencia del NTC a 25°C (ohmios)
 #define NTC_R_FIXED  10000   ///< Resistencia fija del divisor de voltaje (ohmios)
 #define ADC_VOLTAGE  3300    ///< Voltaje de referencia del ADC en mV
 #define T_KELVIN     298.15  ///< Temperatura en Kelvin
 
 // Rangos de temperatura para control de colores
 #define R_MAX        100
 #define R_MIN        30
 #define G_MAX        30
 #define G_MIN        18
 #define B_MAX        18
 #define B_MIN        0
 
 // Variable global para el ADC del NTC
 extern adc_t ntc_adc;
 
 // Funciones del NTC
 /**
  * @brief Lee la temperatura en °C desde el sensor NTC.
  * 
  * @return Temperatura en grados Celsius.
  */
 float ntc_read_temperature ( void );
 
 /**
  * @brief Tarea FreeRTOS para la lectura y procesamiento del NTC.
  * 
  * @param arg Parámetro opcional para la tarea (NULL por defecto).
  */
 void ntc_task ( void *arg );
 
 #endif // NTC_LIBRARY_H
 