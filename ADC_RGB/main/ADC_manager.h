#ifndef ADC_MANAGER_H
#define ADC_MANAGER_H

#include <stdio.h>
#include <stdint.h>
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "esp_log.h"

// Estructura para manejar el ADC
typedef struct {
    adc_oneshot_unit_handle_t handle;
    adc_cali_handle_t cali_handle;
    bool calibrated;
    adc_channel_t channel;
    adc_atten_t attenuation;
} adc_t;

// Funciones públicas
void adc_init(adc_t *adc, adc_unit_t unit, adc_channel_t channel, adc_atten_t attenuation);
int adc_read_raw(adc_t *adc);
int adc_read_voltage(adc_t *adc);
void adc_deinit(adc_t *adc);

// Función específica para potenciómetro
int potentiometer_read_voltage(adc_t *adc);

#endif