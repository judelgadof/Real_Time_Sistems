#include "ADC_manager.h"
#include "esp_log.h"

// Declarar el semáforo como variable global
SemaphoreHandle_t adc_mutex = NULL;

void adc_init_mutex ( void ) {
  // Inicializar el semáforo
  adc_mutex = xSemaphoreCreateMutex();
  if (adc_mutex == NULL) {
      ESP_LOGE(ADC_TAG, "Error al crear el semáforo");
      return;
  }
}

// Inicialización del ADC
void adc_init ( adc_t *adc, adc_unit_t unit, adc_channel_t channel, adc_atten_t attenuation ) {
    adc->channel     = channel;
    adc->attenuation = attenuation;
    adc->unit        = unit;

    // Configuración e inicialización del ADC
    adc_oneshot_unit_init_cfg_t init_cfg = {
        .unit_id = unit,
    };

    ESP_ERROR_CHECK ( adc_oneshot_new_unit ( &init_cfg, &adc->handle ));

    adc_oneshot_chan_cfg_t chan_cfg = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten    = attenuation
    };

    ESP_ERROR_CHECK ( adc_oneshot_config_channel ( adc->handle, channel, &chan_cfg ));

    // Inicializar calibración usando los datos de la estructura
    adc->calibrated = example_adc_calibration_init ( adc );
}

// Inicialización del ADC para múltiples canales
void adc_init_multi(adc_t *adc, adc_unit_t unit, adc_channel_t channel1, adc_channel_t channel2, adc_atten_t attenuation) {
    adc->unit = unit;
    adc->channels[0] = channel1;
    adc->channels[1] = channel2;
    adc->attenuation = attenuation;

    // Configuración e inicialización del ADC
    adc_oneshot_unit_init_cfg_t init_cfg = {
        .unit_id = unit,
    };

    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_cfg, &adc->handle));

    adc_oneshot_chan_cfg_t chan_cfg = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = attenuation
    };

    // Configurar ambos canales dentro de la misma instancia del ADC
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc->handle, channel1, &chan_cfg));
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc->handle, channel2, &chan_cfg));

    // Inicializar calibración
    adc->calibrated = example_adc_calibration_init((adc_t *)adc);
}



// Calibración del ADC (usa directamente la estructura adc_t)
bool example_adc_calibration_init ( adc_t *adc ) 
{
    adc_cali_handle_t handle = NULL;
    esp_err_t ret = ESP_FAIL;
    bool calibrated = false;

    #if ADC_CALI_SCHEME_CURVE_FITTING_SUPPORTED
        if ( !calibrated ) {
            ESP_LOGI ( ADC_TAG, "Usando calibración: Curve Fitting" );
            adc_cali_curve_fitting_config_t cali_config = {
                .unit_id  = adc->unit,
                .chan     = adc->channel,
                .atten    = adc->attenuation,
                .bitwidth = ADC_BITWIDTH_DEFAULT
            };

            ret = adc_cali_create_scheme_curve_fitting ( &cali_config, &handle );
            if ( ret == ESP_OK ) {
                calibrated = true;
            }
        }
    #endif

    #if ADC_CALI_SCHEME_LINE_FITTING_SUPPORTED
        if ( !calibrated ) {
            ESP_LOGI ( ADC_TAG, "Usando calibración: Line Fitting" );
            adc_cali_line_fitting_config_t cali_config = {
                .unit_id  = adc->unit,
                .atten    = adc->attenuation,
                .bitwidth = ADC_BITWIDTH_DEFAULT
            };

            ret = adc_cali_create_scheme_line_fitting ( &cali_config, &handle );
            if ( ret == ESP_OK ) {
                calibrated = true;
            }
        }
    #endif

        adc->cali_handle = handle;

        if ( calibrated ) {
            ESP_LOGI ( ADC_TAG, "Calibración exitosa " );
        } else {
            ESP_LOGW ( ADC_TAG, "Calibración no soportada o eFuse no quemado" );
        }

        return calibrated;
}

// Leer valor crudo desde el ADC
int adc_read_raw ( adc_t *adc ) {
    int raw_value = 0;

    //Tomar mutex antes de iniciar ADC
    if (xSemaphoreTake ( adc_mutex, pdMS_TO_TICKS ( 50 )) == pdTRUE ) {

        esp_err_t ret = adc_oneshot_read ( adc -> handle, adc -> channel, &raw_value );
        if ( ret != ESP_OK ) {
            ESP_LOGE ( ADC_TAG, "Error reading ADC: %s", esp_err_to_name ( ret ) );
        } else {

            ESP_LOGI(ADC_TAG, "Valor leído del ADC : %d", raw_value);
        }

        //Liberar el mutex después de usar ADC
        xSemaphoreGive ( adc_mutex );
    } else {
        ESP_LOGE ( ADC_TAG, "Timeout when taking the mutex for the ADC" );
    }

    return raw_value;
}

// Leer valor crudo desde el ADC para un canal específico
int adc_read_raw_multi(adc_t *adc, int channel_index) {
    int raw_value = 0;

    // Verificar que el índice del canal sea válido (0 o 1)
    if (channel_index < 0 || channel_index > 1) {
        ESP_LOGE(ADC_TAG, "Índice de canal fuera de rango: %d", channel_index);
        return -1;
    }

    // Tomar mutex antes de iniciar ADC
    if (xSemaphoreTake(adc_mutex, pdMS_TO_TICKS(50)) == pdTRUE) {
        esp_err_t ret = adc_oneshot_read(adc->handle, adc->channels[channel_index], &raw_value);
        if (ret != ESP_OK) {
            ESP_LOGE(ADC_TAG, "Error leyendo ADC: %s", esp_err_to_name(ret));
        } /*else {
            ESP_LOGI(ADC_TAG, "Valor leído del ADC (canal %d): %d", adc->channels[channel_index], raw_value);
        }*/

        // Liberar el mutex después de usar ADC
        xSemaphoreGive(adc_mutex);
    } else {
        ESP_LOGE(ADC_TAG, "Timeout al tomar el mutex del ADC");
    }

    return raw_value;
}

// Liberar recursos del ADC
void adc_deinit ( adc_t *adc ) {
    ESP_ERROR_CHECK ( adc_oneshot_del_unit ( adc->handle ));

    if ( adc->calibrated ) {
#if ADC_CALI_SCHEME_CURVE_FITTING_SUPPORTED
        ESP_ERROR_CHECK ( adc_cali_delete_scheme_curve_fitting ( adc->cali_handle ));
#elif ADC_CALI_SCHEME_LINE_FITTING_SUPPORTED
        ESP_ERROR_CHECK ( adc_cali_delete_scheme_line_fitting ( adc->cali_handle ));
#endif
    }
}
