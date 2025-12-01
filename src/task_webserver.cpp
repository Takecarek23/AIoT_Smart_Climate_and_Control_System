#include "task_webserver.h"
#include "global.h"
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

bool webserver_isrunning = false;

void Webserver_sendata(String data)
{
    if (ws.count() > 0)
    {
        ws.textAll(data); // Gửi đến tất cả client đang kết nối
        Serial.println("📤 Đã gửi dữ liệu qua WebSocket: " + data);
    }
    else
    {
        Serial.println("⚠️ Không có client WebSocket nào đang kết nối!");
    }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len)
{
    if (type == WS_EVT_CONNECT)
    {
        Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
    }
    else if (type == WS_EVT_DISCONNECT)
    {
        Serial.printf("WebSocket client #%u disconnected\n", client->id());
    }
    else if (type == WS_EVT_DATA)
    {
        AwsFrameInfo *info = (AwsFrameInfo *)arg;

        if (info->opcode == WS_TEXT)
        {
            String message;
            message += String((char *)data).substring(0, len);
            // parseJson(message, true);
            handleWebSocketMessage(message);
        }
    }
}

void connnectWSV()
{
    ws.onEvent(onEvent);
    server.addHandler(&ws);
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(LittleFS, "/index.html", "text/html"); });
    server.on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(LittleFS, "/script.js", "application/javascript"); });
    server.on("/styles.css", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(LittleFS, "/styles.css", "text/css"); });
    server.on("/raphael.min.js", HTTP_GET, [](AsyncWebServerRequest *request)
              {request->send(LittleFS, "/raphael.min.js", "application/javascript"); });
    server.on("/justgage.min.js", HTTP_GET, [](AsyncWebServerRequest *request)
              {request->send(LittleFS, "/justgage.min.js", "application/javascript"); });
    server.on("/all.min.css", HTTP_GET, [](AsyncWebServerRequest *request)
              {request->send(LittleFS, "/all.min.css", "text/css"); });
    server.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest *request)
              {request->send(404); });

    if (MDNS.begin("vinhandtrong")) { 
        Serial.println("✅ mDNS responder started");
        MDNS.addService("http", "tcp", 80);
    } else {
        Serial.println("❌ Error setting up MDNS responder!");
    }

    server.begin();
    ElegantOTA.begin(&server);
    webserver_isrunning = true;
}

void Webserver_stop()
{
    ws.closeAll();
    server.end();
    webserver_isrunning = false;
}

void Webserver_reconnect()
{
    if (!webserver_isrunning)
    {
        connnectWSV();
    }
    ElegantOTA.loop();
}


// void handleWebSocketMessage(String message) {
//     // 1. Tạo vùng nhớ JSON
//     DynamicJsonDocument doc(1024);
//     DeserializationError error = deserializeJson(doc, message);

//     if (error) {
//         Serial.print(F("deserializeJson() failed: "));
//         Serial.println(error.f_str());
//         return;
//     }

//     String page = doc["page"];

//     if (page == "device") {
//         int gpio = doc["value"]["gpio"];
//         String status = doc["value"]["status"];

//         Serial.printf("Web Control -> GPIO: %d | Status: %s\n", gpio, status.c_str());

//         // Chỉ xử lý nếu trạng thái là ON (người dùng chọn chế độ đó)
//         // Logic 4 chế độ quạt dựa trên GPIO ảo (4, 5, 18, 19)
//         if (status == "ON") {
//             switch (gpio) {
//                 case 4:  // Web gửi GPIO 4 -> Chế độ Tắt
//                     fanMode = 0;
//                     Serial.println("=> MODE: Fan OFF");
//                     break;
//                 case 5:  // Web gửi GPIO 5 -> Mức 1
//                     fanMode = 1;
//                     Serial.println("=> MODE: Fan Level 1");
//                     break;
//                 case 18: // Web gửi GPIO 18 -> Mức 2
//                     fanMode = 2;
//                     Serial.println("=> MODE: Fan Level 2");
//                     break;
//                 case 19: // Web gửi GPIO 19 -> Auto
//                     fanMode = 3;
//                     Serial.println("=> MODE: Fan Auto");
//                     break;
//                 default:
//                     Serial.println("=> Unknown GPIO command");
//                     break;
//             }
//         }
//     }
// }