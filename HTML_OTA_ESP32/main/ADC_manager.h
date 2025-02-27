/**
 * @file ADC_manager.h
 * @brief Manejo del ADC en ESP32 con calibración opcional.
 *
 * Esta librería proporciona funciones para la configuración, calibración y lectura 
 * de valores crudos desde el ADC (Analog-to-Digital Converter) en ESP32. 
 * También gestiona la liberación de recursos del ADC.
 *
 * Soporta calibración mediante esquemas de ajuste de curva y ajuste de línea si están disponibles.
 */

 #ifndef ADC_MANAGER_H
 #define ADC_MANAGER_H
 
 #include <stdio.h>
 #include <stdint.h>
 #include "esp_adc/adc_oneshot.h"
 #include "esp_adc/adc_cali.h"
 #include "esp_adc/adc_cali_scheme.h"
 #include "esp_log.h"
 #include "esp_mac.h"
 #include "freertos/FreeRTOS.h"
 #include "semaphore.h"
 //#include "freertos/task.h"
 //#include "freertos/queue.h"
 
 #define ADC_TAG "ADC_MANAGER" // Etiqueta para logs del ADC
 extern SemaphoreHandle_t adc_mutex;
 
 // Declaración de las variables (no reserva memoria)

 /**
  * @brief Estructura para manejar el ADC y su calibración.
  */
 typedef struct {
     adc_oneshot_unit_handle_t handle;  ///< Manejador del ADC
     adc_channel_t channel; 
     adc_channel_t channels[2];          // Array de canales
     adc_atten_t attenuation;           ///< Atenuación configurada
     adc_cali_handle_t cali_handle;     ///< Manejador de calibración
     bool calibrated;                    ///< Indica si está calibrado
     adc_unit_t unit;                    ///< Unidad ADC utilizada
 } adc_t;

 extern adc_t adc_two;
 
 /**
  * @brief Inicializa el ADC con los parámetros especificados.
  * @param adc Puntero a la estructura `adc_t`.
  * @param unit Unidad del ADC.
  * @param channel Canal del ADC.
  * @param attenuation Nivel de atenuación.
  */
 void adc_init ( adc_t *adc, adc_unit_t unit, adc_channel_t channel, adc_atten_t attenuation );
 
 void adc_init_multi ( adc_t *adc, adc_unit_t unit, adc_channel_t channel1, adc_channel_t channel2, adc_atten_t attenuation );

 /**
  * @brief Realiza la calibración del ADC si es compatible.
  * @param adc Puntero a la estructura `adc_t`.
  * @return true si la calibración fue exitosa, false en caso contrario.
  */
 bool example_adc_calibration_init ( adc_t *adc );
 
 /**
  * @brief Lee un valor crudo del ADC.
  * @param adc Puntero a la estructura `adc_t`.
  * @return Valor crudo leído del ADC.
  */
 int adc_read_raw ( adc_t *adc );
 
 /**
  * @brief Libera los recursos utilizados por el ADC.
  * @param adc Puntero a la estructura `adc_t`.
  */
 void adc_deinit ( adc_t *adc );

 int adc_read_raw_multi(adc_t *adc, int channel_index);

 void adc_init_mutex ( void );
 
 #endif // ADC_MANAGER_H
 