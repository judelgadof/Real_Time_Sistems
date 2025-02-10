#include "NTC_Library.h"


// Variable global del ADC para el NTC (declarada en otro archivo)
adc_t ntc_adc;

// Inicializar la estructura con valores predeterminados


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
    float resistance = (float) NTC_R_FIXED * (ADC_VOLTAGE - voltage) / voltage ;

    // Aplicar la ecuación de Beta para calcular la temperatura en °C
    float temperature = (( 1.0 / ((( log(resistance / NTC_R25) ) / NTC_BETA ) + ( 1.0 / T_KELVIN ))) - 273.15) ;  // Convertir a °C

    // Imprimir valores para depuración con mayor precisión
    printf(" Temp: %.4f °C\n",
            temperature);

    return temperature;
}


