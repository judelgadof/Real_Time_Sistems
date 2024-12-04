/* GPIO Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"

/**
 * Brief:
 * This test code shows how to configure gpio and how to use gpio interrupt.
 *
 * GPIO status:
 * GPIO18: output (ESP32C2/ESP32H2 uses GPIO8 as the second output pin)
 * GPIO19: output (ESP32C2/ESP32H2 uses GPIO9 as the second output pin)
 * GPIO4:  input, pulled up, interrupt from rising edge and falling edge
 * GPIO5:  input, pulled up, interrupt from rising edge.
 *
 * Note. These are the default GPIO pins to be used in the example. You can
 * change IO pins in menuconfig.
 *
 * Test:
 * Connect GPIO18(8) with GPIO4
 * Connect GPIO19(9) with GPIO5
 * Generate pulses on GPIO18(8)/19(9), that triggers interrupt on GPIO4/5
 *
 */

/*#define GPIO_OUTPUT_IO_0    CONFIG_GPIO_OUTPUT_0
#define GPIO_OUTPUT_IO_1    CONFIG_GPIO_OUTPUT_1
#define GPIO_OUTPUT_PIN_SEL  ((1ULL<<GPIO_OUTPUT_IO_0) | (1ULL<<GPIO_OUTPUT_IO_1))
*/
/*
 * Let's say, GPIO_OUTPUT_IO_0=18, GPIO_OUTPUT_IO_1=19
 * In binary representation,
 * 1ULL<<GPIO_OUTPUT_IO_0 is equal to 0000000000000000000001000000000000000000 and
 * 1ULL<<GPIO_OUTPUT_IO_1 is equal to 0000000000000000000010000000000000000000
 * GPIO_OUTPUT_PIN_SEL                0000000000000000000011000000000000000000
 * */

/*
#define GPIO_INPUT_IO_1     CONFIG_GPIO_INPUT_1
#define GPIO_INPUT_PIN_SEL  ((1ULL<<GPIO_INPUT_IO_0) | (1ULL<<GPIO_INPUT_IO_1))
*/
/*
 * Let's say, GPIO_INPUT_IO_0=4, GPIO_INPUT_IO_1=5
 * In binary representation,
 * 1ULL<<GPIO_INPUT_IO_0 is equal to 0000000000000000000000000000000000010000 and
 * 1ULL<<GPIO_INPUT_IO_1 is equal to 0000000000000000000000000000000000100000
 * GPIO_INPUT_PIN_SEL                0000000000000000000000000000000000110000
 * */
#define GPIO_INPUT_IO_0     GPIO_NUM_0
#define LED_GPIO GPIO_NUM_2
#define CONFIG_BLINK_PERIOD 200

QueueHandle_t xButtonQueue;

// Cola para la comunicación entre tareas
typedef enum {
STATE_ONE,
STATE_TWO
} STATE_LED;

//Tarea para manejar el LED
static void blink_led_task (void)
{
    STATE_LED button_state;
    while(1){
        if (XQueueReceived(xButtonQueue,&button_state, portMAX_DELAY)){
            if(button_state == STATE_ONE){
                while (button_state == STATE_ONE){
                    gpio_set_level(LED_GPIO, STATE_TWO);
                    vTaskDelay(CONFIG_BLINK_PERIOD / portTICK_PERIOD_MS);
                    gpio_set_level(LED_GPIO, STATE_ONE);
                    vTaskDelay(CONFIG_BLINK_PERIOD / portTICK_PERIOD_MS);
                
                
                // Revisar si hay un nuevo estado
                if (xQueueReceive(xButtonQueue, &button_state, STATE_ONE)) {
                break; // Salir del ciclo si el estado cambia
                
                }
            }
        } else
             gpio_set_level(LED_GPIO, 0); //Apagar LED

    }
    
}

void vButton_task(void* parameter) {
    static int last_button_state = STATE_TWO; // Botón inicialmente en estado alto (pull-up)
    static STATE_LED state = STATE_ONE;
    while (1) {
        int current_button_state = gpio_get_level(GPIO_INPUT_IO_0);

        if (last_button_state == STATE_TWO && current_button_state == STATE_ONE) { // Botón presionado
            // Cambiar el estado del LED
            state = (state == STATE_ONE) ? STATE_TWO : STATE_ONE;

            // Enviar el nuevo estado a la cola
            xQueueSend(xButtonQueue, &state, portMAX_DELAY);
        }

        last_button_state = current_button_state; // Actualizar el estado anterior
        vTaskDelay(50 / portTICK_PERIOD_MS); // Retardo para evitar rebotes
    }
}

static void configure_led(void)
{
    gpio_reset_pin(LED_GPIO);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(LED_GPIO, GPIO_MODE_OUTPUT);
}



static void configure_button(void)
{
    //zero-initialize the config structure.
    gpio_config_t io_conf = {};
    //disable interrupt
    io_conf.intr_type = GPIO_INTR_DISABLE;
    //set as output mode
    io_conf.mode = GPIO_MODE_INPUT;
    //bit mask of the pins that you want to set,e.g.GPIO18/19
    io_conf.pin_bit_mask = (1ULL << GPIO_INPUT_IO_0);
    //disable pull-down mode
    io_conf.pull_down_en = 0;
    //disable pull-up mode
    io_conf.pull_up_en = 1;
    //configure GPIO with the given settings
    gpio_config(&io_conf);
}

void app_main(void)
{
    // Crear la cola
    xButtonQueue = xQueueCreate(10, sizeof(STATE_LED));
    if (xButtonQueue == NULL) {
        printf("Error al crear la cola.\n");
        return;
    }

    // Crear las tareas
    xTaskCreate(vButton_task, "Button Task", 2048, NULL, 1, NULL);
    xTaskCreate(vLed_task, "LED Task", 2048, NULL, 1, NULL);
}
