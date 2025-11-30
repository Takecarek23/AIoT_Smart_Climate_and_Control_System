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

void task_monitor_button_fan(void *pvParameters) {
    while (1) {
        // analogWrite(GPIO_NUM_6, 150);
        // Serial.println("Đã chạy");
        // delay(5000);
        // analogWrite(GPIO_NUM_6, 0);
        // delay(2000);
        if (xSemaphoreTake(xBinarySemaphoreInternet, (TickType_t)10) == pdTRUE) {
            analogWrite(GPIO_NUM_6, fanState*150);
            xSemaphoreGive(xBinarySemaphoreInternet); 
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}