#include "led_blinky.h"

void led_blinky(void *pvParameters){
    pinMode(LED_GPIO, OUTPUT);
  
  while(1) {                        
    digitalWrite(LED_GPIO, HIGH);  // turn the LED ON
    vTaskDelay(10000);
    digitalWrite(LED_GPIO, LOW);  // turn the LED OFF
    vTaskDelay(10000);
  }
}