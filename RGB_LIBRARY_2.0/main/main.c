#include <stdio.h>
#include "RGB_Library.h"
#include "main.h"

void app_main() {
    
    //Init RGB LED
    rgb_led_init ( LED_RED_PIN , LED_GREEN_PIN ,LED_BLUE_PIN  );
    
    //Select brightness color 
    rgb_select_color ( 0, 0, 50 );
}