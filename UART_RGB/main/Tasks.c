#include "tasks.h"

// Configuración global de rangos de temperatura
static ranges_config_t config = {
    .temp_red_min   = R_MIN,
    .temp_red_max   = R_MAX,
    .temp_green_min = G_MIN,
    .temp_green_max = G_MAX,
    .temp_blue_min  = B_MIN,
    .temp_blue_max  = B_MAX
};

// Definir la cola para recibir comandos UART


/**
 * @brief Tarea para manejar la ejecución de comandos UART y actualizar el LED RGB.
 */
void uart_led_task(void *arg) {
    char received_command[UART_BUFFER_SIZE];
    ranges_config_t *config = (ranges_config_t *)arg;
    float red_intensity = 0.0f, green_intensity = 0.0f, blue_intensity = 0.0f;

    while (1) {
        if (xQueueReceive(uart_queue, received_command, portMAX_DELAY)) {
            printf("Received command: '%s'\n", received_command); // Depuración
            uart_execute_command(received_command, config, NULL, &red_intensity, &green_intensity, &blue_intensity);

            // Actualizar LED RGB
            rgb_select_color((int)red_intensity, (int)green_intensity, (int)blue_intensity);
            printf("LED Updated - R: %d%%, G: %d%%, B: %d%%\n", (int)red_intensity, (int)green_intensity, (int)blue_intensity);
        } else {
            printf("Error: Queue is empty.\n");
        }
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}




