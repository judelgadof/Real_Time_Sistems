#include "ADC_manager.h"

// Inicialización del ADC
void adc_init(adc_t *adc, adc_unit_t unit, adc_channel_t channel, adc_atten_t attenuation) {
    adc->channel = channel;
    adc->attenuation = attenuation;

    // Configuración del ADC
    adc_oneshot_unit_init_cfg_t init_cfg = {
        .unit_id = unit,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_cfg, &adc->handle));

    adc_oneshot_chan_cfg_t chan_cfg = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = attenuation,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc->handle, channel, &chan_cfg));

    // Intentar calibración
    adc->calibrated = false;
#if ADC_CALI_SCHEME_CURVE_FITTING_SUPPORTED
    adc_cali_curve_fitting_config_t cali_cfg = {
        .unit_id = unit,
        .chan = channel,
        .atten = attenuation,
        .bitwidth = ADC_BITWIDTH_DEFAULT,
    };
    esp_err_t ret = adc_cali_create_scheme_curve_fitting(&cali_cfg, &adc->cali_handle);
    if (ret == ESP_OK) {
        adc->calibrated = true;
        ESP_LOGI(TAG, "Calibración activada");
    } else {
        ESP_LOGW(TAG, "Calibración no soportada o fallida");
    }
#endif
}

// Leer valor crudo
int adc_read_raw(adc_t *adc) {
    int raw_value = 0;
    ESP_ERROR_CHECK(adc_oneshot_read(adc->handle, adc->channel, &raw_value));
    return raw_value;
}

// Leer voltaje calibrado
int adc_read_voltage(adc_t *adc) {
    int raw_value = adc_read_raw(adc);
    int voltage = 0;

    if (adc->calibrated) {
        ESP_ERROR_CHECK(adc_cali_raw_to_voltage(adc->cali_handle, raw_value, &voltage));
    }
    return voltage;
}

// Liberar recursos
void adc_deinit(adc_t *adc) {
    ESP_ERROR_CHECK(adc_oneshot_del_unit(adc->handle));
    if (adc->calibrated) {
#if ADC_CALI_SCHEME_CURVE_FITTING_SUPPORTED
        ESP_ERROR_CHECK(adc_cali_delete_scheme_curve_fitting(adc->cali_handle));
#endif
    }
}

// Leer voltaje del potenciómetro
int potentiometer_read_voltage(adc_t *adc) {
    return adc_read_voltage(adc);
}