#ifndef ADC_MANAGER_H
#define ADC_MANAGER_H

#include <stdio.h>
#include <stdint.h>
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "esp_log.h"
#include "esp_mac.h"

#define ADC_TAG "ADC_MANAGER"

typedef struct {
    adc_oneshot_unit_handle_t handle;
    adc_channel_t channel;
    adc_atten_t attenuation;
    adc_cali_handle_t cali_handle;
    bool calibrated;
    adc_unit_t unit;
} adc_t;

// Función para inicializar el ADC
void adc_init ( adc_t *adc, adc_unit_t unit, adc_channel_t channel, adc_atten_t attenuation );

// Función para calibración del ADC
bool example_adc_calibration_init ( adc_t *adc );

// Función para leer valor crudo del ADC
int adc_read_raw ( adc_t *adc );

// Función para leer voltaje calibrado del ADC
int adc_read_voltage ( adc_t *adc );

// Liberar recursos del ADC
void adc_deinit ( adc_t *adc );

int potentiometer_read_voltage ( adc_t *adc );

#endif