#ifndef NTC_LIBRARY_H
#define NTC_LIBRARY_H

#include "ADC_manager.h"
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Parámetros del NTC (ajusta según el datasheet de tu sensor)
#define NTC_BETA     3950    // Coeficiente Beta del termistor
#define NTC_R25      10000   // Resistencia del NTC a 25°C (ohmios)
#define NTC_R_FIXED  10000   // Resistencia fija del divisor de voltaje (ohmios)
#define ADC_VOLTAGE  3300    // Voltaje de referencia del ADC en mV
#define T_KELVIN     298.15  // Temperatura Kelvin


// Variable global para el ADC del NTC
extern adc_t ntc_adc;


// Funciones del NTC
float ntc_read_temperature ( void );
void ntc_task ( void *arg );

#endif 