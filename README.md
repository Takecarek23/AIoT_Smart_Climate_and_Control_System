<div align="center">

  <img src="https://raw.githubusercontent.com/Tarikul-Islam-Anik/Animated-Fluent-Emojis/master/Emojis/Objects/Satellite%20Antenna.png" alt="Satellite Antenna" width="100" />
  
  # 🌍 Smart IoT Environmental Station & Control System
  
  **ESP32-S3 | FreeRTOS | MQTT | CoreIoT | Remote Control**

  [![PlatformIO](https://img.shields.io/badge/PlatformIO-Core-orange?logo=platformio&style=for-the-badge)](https://platformio.org/)
  [![ESP32-S3](https://img.shields.io/badge/Hardware-ESP32--S3-red?logo=espressif&style=for-the-badge)](https://www.espressif.com/)
  [![FreeRTOS](https://img.shields.io/badge/OS-FreeRTOS-green?logo=freertos&style=for-the-badge)](https://www.freertos.org/)
  [![MQTT](https://img.shields.io/badge/Protocol-MQTT-blue?logo=mqtt&style=for-the-badge)](https://mqtt.org/)
  [![C++](https://img.shields.io/badge/Language-C++17-00599C?logo=c%2B%2B&style=for-the-badge)](https://isocpp.org/)

  <p align="center">
    Hệ thống giám sát môi trường đa nhiệm thời gian thực, tích hợp điều khiển từ xa qua Cloud với cơ chế đồng bộ hóa tài nguyên an toàn.
    <br />
    <a href="#-demo"><strong>Xem Demo</strong></a> · 
    <a href="#-tính-năng-nổi-bật"><strong>Tính năng</strong></a> · 
    <a href="#-cài-đặt"><strong>Cài đặt</strong></a>
  </p>
</div>

---

## 🚀 Giới Thiệu (Overview)

Dự án này biến **ESP32-S3** thành một trạm quan trắc thông minh. Thay vì sử dụng vòng lặp tuần tự đơn giản (`super-loop`), hệ thống tận dụng sức mạnh của **FreeRTOS** để chạy song song nhiều tác vụ: đọc cảm biến, duy trì kết nối MQTT, và xử lý lệnh điều khiển từ xa với độ trễ thấp.

Dữ liệu được trực quan hóa trên nền tảng **CoreIoT** (dựa trên ThingsBoard), cho phép giám sát và điều khiển thiết bị từ bất kỳ đâu trên thế giới.

## 📸 Demo

<div align="center">
  <img src="https://user-images.githubusercontent.com/placeholder/dashboard-demo.png" alt="CoreIoT Dashboard" width="80%" style="border-radius: 10px; box-shadow: 0 4px 8px 0 rgba(0, 0, 0, 0.2);"/>
  <br><i>Giao diện Dashboard trên CoreIoT (Minh họa)</i>
</div>

---

## ✨ Tính Năng Nổi Bật

* 🌡️ **Giám sát Thời gian thực:** Thu thập Nhiệt độ & Độ ẩm (DHT22/20) với chu kỳ tùy chỉnh.
* 🧠 **Hệ điều hành FreeRTOS:** Quản lý đa nhiệm (Multi-tasking) chuyên nghiệp.
* ☁️ **Kết nối IoT Cloud:** Giao tiếp 2 chiều với **CoreIoT** qua giao thức MQTT.
    * **Telemetry:** Gửi dữ liệu cảm biến lên Cloud.
    * **RPC (Remote Procedure Call):** Nhận lệnh điều khiển thiết bị từ Cloud tức thì.
* 🛡️ **Resource Protection:** Sử dụng **Semaphore/Mutex** để giải quyết xung đột tài nguyên (Race Condition) giữa nút nhấn vật lý và lệnh điều khiển từ xa.
* 🔋 **Smart Connectivity:** Cơ chế tự động kết nối lại (Auto-reconnect) khi mất WiFi hoặc MQTT.

---

## 🛠️ Tech Stack (Công Nghệ)

| Lĩnh vực | Công cụ / Công nghệ |
| :--- | :--- |
| **Hardware** | ESP32-S3 DevKitC-1, DHT22 Sensor, LEDs, Buttons |
| **Firmware** | C/C++, Arduino Framework (trên nền ESP-IDF) |
| **IDE** | VS Code + PlatformIO Extension |
| **OS** | FreeRTOS (Tasks, Semaphores, Queues) |
| **Protocol** | MQTT, JSON (ArduinoJson) |
| **Cloud** | CoreIoT (ThingsBoard fork) |

---

## 🏗️ Kiến Trúc Hệ Thống (System Architecture)

Sơ đồ luồng hoạt động của các Task trong FreeRTOS:

```mermaid
graph TD
    subgraph ESP32-S3 [ESP32-S3 FreeRTOS]
        T1[<b>Task 1: Sensor Reading</b><br>Đọc DHT22 mỗi 2s]
        T2[<b>Task 2: CoreIoT MQTT</b><br>Gửi Telemetry & Nhận RPC]
        T3[<b>Task 3: Manual Button</b><br>Nút nhấn vật lý]
        
        Mutex{<b>Binary Semaphore</b><br>Bảo vệ LED}
        
        HW_LED((<b>LED Actuator</b>))
        HW_DHT((<b>DHT Sensor</b>))
    end

    subgraph Cloud [CoreIoT Cloud]
        DB[(Database)]
        Dash[Dashboard UI]
    end

    %% Connections
    T1 -->|I2C/GPIO| HW_DHT
    T1 -->|Queue/Global| T2
    
    T2 <-->|MQTT Pub/Sub| Cloud
    Cloud <--> Dash
    
    T3 -->|Give Token| Mutex
    T2 -->|Give Token (RPC)| Mutex
    
    Mutex -->|Control| HW_LED
    
    style Mutex fill:#f96,stroke:#333,stroke-width:2px
    style ESP32-S3 fill:#e1f5fe,stroke:#01579b
    style Cloud fill:#fff3e0,stroke:#ff6f00
