#include "NTC_Library.h"


// Variable global del ADC para el NTC (declarada en otro archivo)
adc_t ntc_adc;

// Leer la temperatura en °C desde el NTC
float ntc_read_temperature (void) {
    // Leer voltaje en mV desde el ADC calibrado
    int raw_value = adc_read_raw (&ntc_adc);

    // Convertir lectura cruda a voltaje en mV
    float voltage = ((float)raw_value / ((1 << ADC_BITWIDTH_12 ) - 1)) * ADC_VOLTAGE; 
    if (voltage <= 0 || voltage >= ADC_VOLTAGE) {
        return -1000.0; // Valor de error
    }

    // Calcular la resistencia del NTC usando la fórmula del divisor de voltaje
    float resistance = (float)(NTC_R_FIXED * voltage) / (ADC_VOLTAGE - voltage);

    // Aplicar la ecuación de Beta para calcular la temperatura en °C
    float temperature = 1.0 / ((log(resistance / NTC_R25) / NTC_BETA) + (1.0 / 298.15));
    temperature -= 273.15;  // Convertir de Kelvin a Celsius

    return temperature;
}


// Tarea en FreeRTOS para monitorear el NTC
void ntc_task (void *arg) {
    while (1) {
        float temperature = ntc_read_temperature();
        if (temperature != -1000.0) {
            printf("Temperatura: %.2f °C\n", temperature);
        } else {
            printf("Error en la lectura del NTC\n");
        }

        vTaskDelay(pdMS_TO_TICKS(1000));  // Leer cada 1 segundo 
    }
} 