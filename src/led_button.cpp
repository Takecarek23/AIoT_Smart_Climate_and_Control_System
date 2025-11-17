#include "led_button.h"

void task_monitor_button(void *pvParameters)
{
    pinMode(LED_PIN, OUTPUT);
    while (1) {
        // Check if button is pressed (active-low)
        if (xSemaphoreTake(xBinarySemaphoreInternet, (TickType_t)10) == pdTRUE) {
            digitalWrite(LED_PIN, ledState);
            xSemaphoreGive(xBinarySemaphoreInternet); 
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

// Khi nhấn nút thì đèn sáng, không thì led blinky cứ nằm chờ