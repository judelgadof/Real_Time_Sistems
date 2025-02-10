#ifndef NTC_LIBRARY_H
#define NTC_LIBRARY_H

#include "ADC_manager.h"
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Parámetros del NTC (ajusta según el datasheet de tu sensor)
#define NTC_BETA     900000       // Coeficiente Beta del termistor
#define NTC_R25      47       // Resistencia del NTC a 25°C (ohmios)
#define NTC_R_FIXED  200   // Resistencia fija del divisor de voltaje (ohmios)
#define ADC_VOLTAGE  3300    // Voltaje de referencia del ADC en mV

// Variable global para el ADC del NTC
extern adc_t ntc_adc;

// Funciones del NTC
float ntc_read_temperature ( void );
void ntc_task ( void *arg );

#endif 