#include "led_blinky.h"
#include "global.h"

void safe_led_control(int level, int duration_ms) {
    // Lấy Mutex lock trong 10 ticks
    if (xSemaphoreTake(xLedMutex, (TickType_t)10) == pdTRUE) {
        digitalWrite(LED_GPIO, level);
        xSemaphoreGive(xLedMutex); // Trả 
    }
    // Delay nằm ngoài vùng khóa để các task khác có cơ hội chen vào nếu cần
    vTaskDelay(pdMS_TO_TICKS(duration_ms));
}

void led_blinky(void *pvParameters) {
    pinMode(LED_GPIO, OUTPUT);

    while (1) {
        
        // TRƯỜNG HỢP 1: Mát mẻ (< 28°C)
        // Hành động: Nháy nhanh 2 lần (500ms)
        if (glob_temperature < 28.0) {
            safe_led_control(HIGH, 2000);
            safe_led_control(LOW, 2000);
            safe_led_control(HIGH, 2000);
            safe_led_control(LOW, 2000);
        } 
        // TRƯỜNG HỢP 2: Ấm (28°C - 29°C)
        // Hành động: 1 Nhanh (500ms) + 1 Chậm (1000ms)
        else if (glob_temperature < 29.0) {
            safe_led_control(HIGH, 500);
            safe_led_control(LOW, 500);
            safe_led_control(HIGH, 1000); // Sáng lâu hơn
            safe_led_control(LOW, 500);
        } 
        // TRƯỜNG HỢP 3: Nóng (>= 29°C)
        // Hành động: Nháy chậm cảnh báo (1000ms)
        else {
            safe_led_control(HIGH, 1000);
            safe_led_control(LOW, 500);
            safe_led_control(HIGH, 1000);
            safe_led_control(LOW, 500);
        }

        // Nghỉ 2 giây trước chu kỳ tiếp theo
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}
