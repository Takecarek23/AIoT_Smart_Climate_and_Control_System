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
    updateFanUI();
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

var fanConfig = [
    { mode: 0, name: "Dừng",   gpio: 6, state: false }, // Không có GPIO riêng
    { mode: 1, name: "Mức 1",  gpio: 6, state: false }, // GPIO cho Mức 1
    { mode: 2, name: "Mức 2",  gpio: 6, state: false }, // GPIO cho Mức 2
    { mode: 3, name: "Auto",   gpio: 6, state: false }  // GPIO cho Auto
];

// 1. Hàm cập nhật giao diện (Chỉ 1 nút sáng, nút Dừng luôn tắt)
function updateFanUI() {
    fanConfig.forEach((item, index) => {
        if (index === 0) return; // Bỏ qua nút Dừng

        var btn = document.getElementById(`btn-fan-${index}`);
        if (btn) {
            if (item.active) {
                btn.classList.add("on");
                btn.innerHTML = "ĐANG CHẠY";
            } else {
                btn.classList.remove("on");
                btn.innerHTML = "CHỌN";
            }
        }
    });
}

// 2. Hàm xử lý logic chính
function controlFan(selectedIndex) {
    
    // TRƯỜNG HỢP 1: Bấm nút DỪNG (Index 0)
    if (selectedIndex === 0) {
        console.log("🛑 Dừng toàn bộ hệ thống quạt");
        
        // Tắt trạng thái tất cả chế độ trong phần mềm
        fanConfig.forEach(f => f.state = false);
        
        // Gửi lệnh TẮT xuống TẤT CẢ các GPIO của quạt (để đảm bảo an toàn)
        fanConfig.forEach(item => {
            if (item.gpio !== null) {
                sendDeviceCommand(item.name, item.gpio, "OFF");
            }
        });

        // Hiệu ứng nháy nút Dừng cho người dùng biết đã nhận lệnh
        var stopBtn = document.getElementById("btn-fan-0");
        stopBtn.innerHTML = "ĐÃ DỪNG";
        setTimeout(() => stopBtn.innerHTML = "KÍCH HOẠT", 1000);
    } 
    
    // TRƯỜNG HỢP 2: Bấm nút Chế độ (1, 2, 3)
    else {
        // Tắt các chế độ khác, chỉ bật chế độ được chọn
        fanConfig.forEach((item, index) => {
            if (index === 0) return; // Bỏ qua nút dừng

            if (index === selectedIndex) {
                // Đây là chế độ vừa bấm -> BẬT
                if (!item.state) { // Chỉ gửi lệnh nếu nó chưa bật
                    item.state = true;
                    sendDeviceCommand(item.mode, item.gpio, "ON");
                }
            } else {
                // Đây là chế độ khác -> Phải TẮT (để đảm bảo chỉ 1 mức chạy)
                if (item.state) {
                    item.state = false;
                    sendDeviceCommand(item.mode, item.gpio, "OFF");
                }
            }
        });
    }

    // Cập nhật màu sắc nút bấm
    updateFanUI();
}

// Hàm phụ trợ để gửi JSON (Code cũ dùng lại)
function sendDeviceCommand(mode, gpio, status) {
    var payload = JSON.stringify({
        page: "device",
        value: {
            name: mode,
            gpio: String(gpio),
            status: status
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
