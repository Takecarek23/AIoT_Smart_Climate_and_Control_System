// ==================== WEBSOCKET & GLOBAL VARS ====================
var gateway = `ws://${window.location.hostname}/ws`;
var websocket;
var gaugeTemp; 
var gaugeHumi;

// Chạy khi trang tải xong
window.addEventListener('load', onLoad);

function onLoad(event) {
    initWebSocket();
    initGauges();
    updateButtons();
}

function initWebSocket() {
    console.log('Trying to open a WebSocket connection…');
    websocket = new WebSocket(gateway);
    websocket.onopen = onOpen;
    websocket.onclose = onClose;
    websocket.onmessage = onMessage;
}

function onOpen(event) {
    console.log('Connection opened');
}

function onClose(event) {
    console.log('Connection closed');
    setTimeout(initWebSocket, 2000);
}

function onMessage(event) {
    console.log("📩 Nhận:", event.data);
    try {
        var data = JSON.parse(event.data);

        // Cập nhật Đồng hồ Nhiệt độ (Kiểm tra biến toàn cục đã sẵn sàng chưa)
        if (data.temp !== undefined && gaugeTemp) {
            gaugeTemp.refresh(data.temp);
        }
        
        // Cập nhật Đồng hồ Độ ẩm
        if (data.humi !== undefined && gaugeHumi) {
            gaugeHumi.refresh(data.humi);
        }

    } catch (e) {
        console.warn("Không phải JSON hợp lệ hoặc lỗi update:", e);
    }
}

function Send_Data(data) {
    if (websocket && websocket.readyState === WebSocket.OPEN) {
        websocket.send(data);
        console.log("📤 Gửi:", data);
    } else {
        console.warn("⚠️ WebSocket chưa sẵn sàng!");
        alert("⚠️ WebSocket chưa kết nối!");
    }
}

// ==================== UI NAVIGATION ====================
let relayList = [];
let deleteTarget = null;

function showSection(id, event) {
    document.querySelectorAll('.section').forEach(sec => sec.style.display = 'none');
    document.getElementById(id).style.display = id === 'settings' ? 'flex' : 'block';
    document.querySelectorAll('.nav-item').forEach(i => i.classList.remove('active'));
    if(event) event.currentTarget.classList.add('active');
}

// ==================== HOME GAUGES ====================
function initGauges() {
    gaugeTemp = new JustGage({
        id: "gauge_temp",
        value: 0, // Giá trị mặc định
        min: -10,
        max: 50,
        donut: true,
        pointer: false,
        gaugeWidthScale: 0.25,
        gaugeColor: "transparent",
        levelColorsGradient: true,
        levelColors: ["#00BCD4", "#4CAF50", "#FFC107", "#F44336"]
    });

    gaugeHumi = new JustGage({
        id: "gauge_humi",
        value: 0,
        min: 0,
        max: 100,
        donut: true,
        pointer: false,
        gaugeWidthScale: 0.25,
        gaugeColor: "transparent",
        levelColorsGradient: true,
        levelColors: ["#42A5F5", "#00BCD4", "#0288D1"]
    });
}

// ==================== DEVICE FUNCTIONS (4 NÚT CỐ ĐỊNH) ====================

// 1. Cấu hình danh sách thiết bị cố định
var fixedDevices = [
    { id: 0, name: "Dừng hoạt động",    gpio: 4,  state: false }, 
    { id: 1, name: "Mức 1",             gpio: 5,  state: false }, 
    { id: 2, name: "Mức 2",             gpio: 18, state: false }, 
    { id: 3, name: "Chế độ tự động",    gpio: 19, state: false }  
];

// 2. Hàm cập nhật màu sắc nút bấm trên Web
function updateButtons() {
    fixedDevices.forEach((device, index) => {
        var btn = document.getElementById(`btn-${index}`); // Tìm nút theo ID btn-0, btn-1...
        if (btn) {
            if (device.state) {
                btn.classList.add("on"); // Thêm class màu xanh
                btn.innerHTML = "ON";
            } else {
                btn.classList.remove("on"); // Bỏ class màu xanh (về màu xám)
                btn.innerHTML = "OFF";
            }
        }
    });
}

// 3. Hàm xử lý khi nhấn nút
function toggleFixedDevice(index) {
    var device = fixedDevices[index];
    
    // Đảo trạng thái (True thành False và ngược lại)
    device.state = !device.state;

    // Cập nhật màu nút ngay lập tức
    updateButtons();

    // Đóng gói dữ liệu JSON gửi xuống ESP32
    // Format khớp với code C++: {"page":"device", "value":{"gpio":"4", "status":"ON"}}
    var payload = JSON.stringify({
        page: "device",
        value: {
            name: device.name,
            gpio: String(device.gpio), 
            status: device.state ? "ON" : "OFF"
        }
    });
    
    Send_Data(payload);
}

// ==================== SETTINGS FORM ====================
document.getElementById("settingsForm").addEventListener("submit", function (e) {
    e.preventDefault();

    const ssid = document.getElementById("ssid").value.trim();
    const password = document.getElementById("password").value.trim();
    const token = document.getElementById("token").value.trim();
    const server = document.getElementById("server").value.trim();
    const port = document.getElementById("port").value.trim();

    const settingsJSON = JSON.stringify({
        page: "setting",
        value: {
            ssid: ssid,
            password: password,
            token: token,
            server: server,
            port: port
        }
    });

    Send_Data(settingsJSON);
    alert("✅ Cấu hình đã được gửi đến thiết bị!");
});
