#include "led_blinky.h"

void led_blinky(void *pvParameters){
    pinMode(LED_GPIO, OUTPUT);
    
  while(1) {           
    // thêm đoạn này 
    if (xSemaphoreTake(xBinarySemaphoreInternet, (TickType_t)10) == pdTRUE) {
      
      digitalWrite(LED_GPIO, HIGH); 
      
      // Trả Token lại để người khác dùng
      xSemaphoreGive(xBinarySemaphoreInternet); 
    }
  
    vTaskDelay(10000);

    if (xSemaphoreTake(xBinarySemaphoreInternet, (TickType_t)10) == pdTRUE) {
      digitalWrite(LED_GPIO, LOW);
      xSemaphoreGive(xBinarySemaphoreInternet);
    }
    vTaskDelay(500 / portTICK_PERIOD_MS);
    // hết đoạn thêm             
    // digitalWrite(LED_GPIO, HIGH);  // turn the LED ON
    // vTaskDelay(10000);
    // digitalWrite(LED_GPIO, LOW);  // turn the LED OFF
    // vTaskDelay(10000);
  }
}

