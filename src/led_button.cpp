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

// void task_monitor_button_fan(void *pvParameters) {
//     while (1) {
//         if (xSemaphoreTake(xBinarySemaphoreInternet, (TickType_t)10) == pdTRUE) {
//             analogWrite(GPIO_NUM_6, fanState*150);
//             xSemaphoreGive(xBinarySemaphoreInternet); 
//         }
//         vTaskDelay(pdMS_TO_TICKS(10));
//     }
// }

void task_monitor_button_fan(void *pvParameters) {
    // Cấu hình chân quạt
    pinMode(GPIO_NUM_6, OUTPUT);
    analogWrite(GPIO_NUM_6, 0); // Tắt ban đầu

    while (1) {
        // Sử dụng Semaphore nếu cần bảo vệ tài nguyên (tùy chọn)
        // if (xSemaphoreTake(xBinarySemaphoreInternet, (TickType_t)10) == pdTRUE) {
            
            int speed = 0;

            switch (fanMode) {
                case 0: // Dừng hoạt động
                    speed = 0;
                    break;

                case 1: // Mức 1 (Nhẹ)
                    speed = 100; // Giá trị PWM (0-255)
                    break;

                case 2: // Mức 2 (Mạnh)
                    speed = 255; // Tối đa
                    break;

                case 3: // Chế độ Tự động (Dựa vào nhiệt độ)
                    if (glob_temperature > 30.0) {
                        speed = 255; // Nóng quá -> Chạy max
                    } else if (glob_temperature > 25.0) {
                        speed = 150; // Hơi nóng -> Chạy vừa
                    } else {
                        speed = 0;   // Mát -> Tắt
                    }
                    // Debug chế độ Auto
                    // Serial.printf("Auto Mode: Temp=%.1f -> Speed=%d\n", glob_temperature, speed);
                    break;
            }

            // Ghi giá trị ra chân thực tế
            analogWrite(GPIO_NUM_6, speed);

            // Trả Token
            // xSemaphoreGive(xBinarySemaphoreInternet); 
        // }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}