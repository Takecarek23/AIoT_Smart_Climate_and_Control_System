#include "temp_humi_monitor.h"

// Khai báo biến toàn cục
DHT20 dht20;
LiquidCrystal_I2C lcd(33, 16, 2); // Lưu ý: Kiểm tra kỹ địa chỉ 33 (0x21)
SemaphoreHandle_t xI2CMutex = NULL;

// Hàm setup riêng cho phần này (được gọi 1 lần từ main.cpp)
void setup_monitoring_system() {
    // 1. Tạo Mutex trước
    xI2CMutex = xSemaphoreCreateMutex();
    
    // 2. Khởi tạo phần cứng
    Wire.begin(11, 12);
    dht20.begin();
    
    lcd.begin();      
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print("System Booting..");
}

void temp_humi_monitor(void *pvParameters){

    while (1){

        if (xSemaphoreTake(xI2CMutex, portMAX_DELAY) == pdTRUE) {    
            
            // --- ĐỌC CẢM BIẾN ---
            dht20.read();
            float temperature = dht20.getTemperature();
            float humidity = dht20.getHumidity();

            if (isnan(temperature) || isnan(humidity)) {
                Serial.println("Failed to read DHT!");
                // Nếu lỗi, gán đại giá trị 0
                temperature = 0; 
                humidity = 0; 
            } else {
                // Cập nhật biến toàn cục
                glob_temperature = temperature;
                glob_humidity = humidity;
            }

            // --- IN RA SERIAL ---
            Serial.printf("Hum: %.1f%%  Temp: %.1fC\n", humidity, temperature);
            
            // --- HIỂN THỊ LCD ---
            lcd.setCursor(0, 0);
            char buffer[17];
            snprintf(buffer, sizeof(buffer), "T:%.1f H:%.1f%%", temperature, humidity);
            lcd.print(buffer);

            if (!isnan(glob_temperature) && !isnan(glob_humidity)) {
                // 2. Tạo chuỗi JSON
                // Format: {"temp": 28.5, "humi": 65.0}
                String json = "{\"temp\":";
                json += String(glob_temperature, 1);
                json += ",\"humi\":";
                json += String(glob_humidity, 1);
                json += "}";

                // 3. Gửi lên Web Server
                Webserver_sendata(json); 
                Serial.println(json); // Debug
            }

            // In các cảnh báo 
            lcd.setCursor(0, 1);
            if (temperature >= 40.0) {
                lcd.print("!! CRITICAL !!  "); 
            } 
            else if (temperature >= 30.0) {
                lcd.print("WARN: High Temp "); 
            } 
            else {
                lcd.print("System: Normal  "); 
            }
            
            // QUAN TRỌNG: Trả chìa khóa ngay sau khi dùng xong I2C
            xSemaphoreGive(xI2CMutex);
        }
        
        // Delay nằm ngoài IF để đảm bảo Task luôn ngủ, tránh treo CPU
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}