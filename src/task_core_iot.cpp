
#include "task_core_iot.h"
constexpr uint32_t MAX_MESSAGE_SIZE = 1024U;
#define LED_PIN 48

WiFiClient wifiClient;
Arduino_MQTT_Client mqttClient(wifiClient);
ThingsBoard tb(mqttClient, MAX_MESSAGE_SIZE);

constexpr char LED_STATE_ATTR[] = "ledState";
constexpr char BLINKING_INTERVAL_ATTR[] = "blinkingInterval";

volatile int ledMode = 0;
volatile bool ledState = false;

constexpr uint16_t BLINKING_INTERVAL_MS_MIN = 10U;
constexpr uint16_t BLINKING_INTERVAL_MS_MAX = 60000U;
volatile uint16_t blinkingInterval = 1000U;

constexpr int16_t telemetrySendInterval = 1000U;
uint32_t lastTelemetrySend = 0U;

constexpr std::array<const char *, 2U> SHARED_ATTRIBUTES_LIST = {
    LED_STATE_ATTR,
};

void processSharedAttributes(const Shared_Attribute_Data &data)
{
    for (auto it = data.begin(); it != data.end(); ++it)
    {
        if (strcmp(it->key().c_str(), BLINKING_INTERVAL_ATTR) == 0)
        {
            const uint16_t new_interval = it->value().as<uint16_t>();
            if (new_interval >= BLINKING_INTERVAL_MS_MIN && new_interval <= BLINKING_INTERVAL_MS_MAX)
            {
                blinkingInterval = new_interval;
                Serial.print("Blinking interval is set to: ");
                Serial.println(new_interval);
            }
        }
        if (strcmp(it->key().c_str(), LED_STATE_ATTR) == 0)
        {
            ledState = it->value().as<bool>();
            digitalWrite(LED_PIN, ledState);
            Serial.print("LED state is set to: ");
            Serial.println(ledState);
        }
    }
}

RPC_Response setLedSwitchValue(const RPC_Data &data)
{
    Serial.println("Received Switch state");
    bool newState = data;
    // // xin lấy token 
    // if (xSemaphoreTake(xBinarySemaphoreInternet, (TickType_t)10) == pdTRUE) {
    //   // Đã lấy được quyền -> Điều khiển LED
    //   ledState = newState;
    //   digitalWrite(LED_PIN, ledState);
      
    //   // Trả Token lại ngay
    //   // Trả Token lại để người khác dùng
    //   xSemaphoreGive(xBinarySemaphoreInternet); 
    // }
    Serial.print("Switch state change: ");
    Serial.println(newState);
    ledState = newState; // Cập nhật biến trạng thái (để code khác biết)
    //digitalWrite(LED_PIN, ledState); // ĐIỀU KHIỂN LED NGAY LẬP TỨC
    tb.sendAttributeData(LED_STATE_ATTR, ledState); // Cập nhật trạng thái LED lên server
    return RPC_Response("setLedSwitchValue", newState);
}

// Mới thêm
RPC_Response getLedState(const RPC_Data &data)
{
    return RPC_Response("ledState", ledState);
}

// const std::array<RPC_Callback, 2U> callbacks = {
//     RPC_Callback{"setLedSwitchValue", setLedSwitchValue},
//     RPC_Callback{"getLedState", getLedState}};

const std::array<RPC_Callback, 2U> callbacks = {
    RPC_Callback{"led_status", setLedSwitchValue}, // <-- Sửa tên thành "led_status"
    RPC_Callback{"led_status", getLedState}      // <-- Sửa tên thành "led_status"
};

const Shared_Attribute_Callback attributes_callback(&processSharedAttributes, SHARED_ATTRIBUTES_LIST.cbegin(), SHARED_ATTRIBUTES_LIST.cend());
const Attribute_Request_Callback attribute_shared_request_callback(&processSharedAttributes, SHARED_ATTRIBUTES_LIST.cbegin(), SHARED_ATTRIBUTES_LIST.cend());

void CORE_IOT_sendata(String mode, String feed, String data)
{
    if (mode == "attribute")
    {
        tb.sendAttributeData(feed.c_str(), data);
    }
    else if (mode == "telemetry")
    {
        float value = data.toFloat();
        tb.sendTelemetryData(feed.c_str(), value);
    }
    else
    {
        // handle unknown mode
    }
}

void CORE_IOT_reconnect()
{
    if (!tb.connected())
    {
        if (!tb.connect(CORE_IOT_SERVER.c_str(), CORE_IOT_TOKEN.c_str(), CORE_IOT_PORT.toInt()))
        {
            // Serial.println("Failed to connect");
            return;
        }

        tb.sendAttributeData("macAddress", WiFi.macAddress().c_str());

        Serial.println("Subscribing for RPC...");
        if (!tb.RPC_Subscribe(callbacks.cbegin(), callbacks.cend()))
        {
            Serial.println("Failed to subscribe for RPC");
            return;
        }

        if (!tb.Shared_Attributes_Subscribe(attributes_callback))
        {
            Serial.println("Failed to subscribe for shared attribute updates");
            return;
        }

        Serial.println("Subscribe done");

        if (!tb.Shared_Attributes_Request(attribute_shared_request_callback))
        {
            // Serial.println("Failed to request for shared attributes");
            return;
        }
        tb.sendAttributeData("localIp", WiFi.localIP().toString().c_str());
    }
    else if (tb.connected())
    {
        tb.loop();
    }
}

void coreiot_task(void *pvParameters)
{
  (void)pvParameters; // Tắt cảnh báo

  Serial.println("CoreIOT Task: Dang cho WiFi...");
  vTaskDelay(pdMS_TO_TICKS(10000)); 

  Serial.println("CoreIOT Task: Bat dau.");

  while (1)
  {
    // 1. Tự kết nối lại (nếu cần)
    if (!tb.connected())
    {
      CORE_IOT_reconnect();
    }
    
    // 2. Luôn gọi tb.loop() để duy trì kết nối và nhận RPC
    tb.loop();
    
    // 3. Chỉ gửi dữ liệu 10 giây một lần
    if (tb.connected() && (millis() - lastTelemetrySend > telemetrySendInterval))
    {
      tb.sendTelemetryData("led_status", ledState);
      tb.sendTelemetryData("temperature", glob_temperature);
      tb.sendTelemetryData("humidity", glob_humidity);
      lastTelemetrySend = millis();
    }
    
    // 4. Ngủ rất ngắn (10ms) để Task chạy liên tục
    vTaskDelay(pdMS_TO_TICKS(10)); 
  }
}

// void coreiot_task(void *pvParameters)
// {
//   (void)pvParameters; // Tắt cảnh báo

//   Serial.println("CoreIOT Task: Dang cho WiFi...");
//   vTaskDelay(pdMS_TO_TICKS(10000)); 

//   Serial.println("CoreIOT Task: Bat dau.");

//   while (1)
//   {
//     if (tb.connected())
//     {
//       tb.sendTelemetryData("led_status", ledState);
//       tb.sendTelemetryData("temperature", glob_temperature);
//       tb.sendTelemetryData("humidity", glob_humidity);
//     }
    
//     vTaskDelay(pdMS_TO_TICKS(1000)); 
//   }
// }