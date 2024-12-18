#include "ADC_manager.h"
#include "esp_log.h"

// Inicialización del ADC
void adc_init( adc_t *adc, adc_unit_t unit, adc_channel_t channel, adc_atten_t attenuation ) {
    adc->channel     = channel;
    adc->attenuation = attenuation;
    adc->unit        = unit;

    // Configuración e inicialización del ADC
    adc_oneshot_unit_init_cfg_t init_cfg = {
        .unit_id = unit
    };

    ESP_ERROR_CHECK( adc_oneshot_new_unit ( &init_cfg, &adc->handle ));

    adc_oneshot_chan_cfg_t chan_cfg = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten    = attenuation
    };

    ESP_ERROR_CHECK ( adc_oneshot_config_channel ( adc->handle, channel, &chan_cfg ));

    // Inicializar calibración usando los datos de la estructura
    adc->calibrated = example_adc_calibration_init ( adc );
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
            ESP_LOGI ( ADC_TAG, "Calibración exitosa" );
        } else {
            ESP_LOGW ( ADC_TAG, "Calibración no soportada o eFuse no quemado" );
        }

        return calibrated;
}

// Leer valor crudo desde el ADC
int adc_read_raw ( adc_t *adc ) {
    int raw_value = 0;
    ESP_ERROR_CHECK ( adc_oneshot_read ( adc->handle, adc->channel, &raw_value ));
    return raw_value;
}

// Leer voltaje calibrado desde el ADC
int adc_read_voltage ( adc_t *adc ) {
    int raw_value = adc_read_raw ( adc );
    int voltage = 0;

    if ( adc->calibrated ) {
        ESP_ERROR_CHECK ( adc_cali_raw_to_voltage ( adc->cali_handle, raw_value, &voltage ));
    } else {
        ESP_LOGW( ADC_TAG, "ADC no calibrado, retornando valor crudo" );
        voltage = raw_value;
    }

    return voltage;
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

// Leer voltaje del potenciómetro
int potentiometer_read_voltage ( adc_t *adc ) {
    return adc_read_voltage( adc );
}
