#include "DashboardService.h"
#include "HardwareConfig.h"
#include "ConfigManager.h"
#include "L298NDriver.h"
#include <WebServer.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <Preferences.h>

static DashboardService* s_instance = nullptr;
static WebServer* s_webServer = nullptr;

// Declarações externas das instâncias globais definidas em windfollow.ino
extern ConfigManager configManager;
extern L298NDriver l298nDriver;

DashboardService::DashboardService()
    : m_running(false) {}

DashboardService::~DashboardService() {
    deinit();
}

bool DashboardService::init() {
    if (m_running) return true;

    s_webServer = new WebServer(80);
    if (!s_webServer) return false;

    registerRoutes();
    s_webServer->begin();

    m_running = true;
    s_instance = this;
    return true;
}

void DashboardService::tick() {
    if (s_webServer) {
        s_webServer->handleClient();
    }
}

bool DashboardService::deinit() {
    if (!m_running) return true;
    if (s_webServer) {
        s_webServer->stop();
        delete s_webServer;
        s_webServer = nullptr;
    }
    m_running = false;
    s_instance = nullptr;
    return true;
}

bool DashboardService::isRunning() const {
    return m_running;
}

void DashboardService::registerRoutes() {
    if (!s_webServer) return;

    s_webServer->on("/", HTTP_GET, [](){
        if (s_instance) s_instance->handleRoot();
    });
    s_webServer->on("/index.html", HTTP_GET, [](){
        if (s_instance) s_instance->handleRoot();
    });
    s_webServer->on("/style.css", HTTP_GET, [](){
        if (s_instance) s_instance->handleCss();
    });
    s_webServer->on("/app.js", HTTP_GET, [](){
        if (s_instance) s_instance->handleJs();
    });
    s_webServer->on("/api/config", HTTP_GET, [](){
        if (s_instance) s_instance->handleConfigGet();
    });
    s_webServer->on("/api/config", HTTP_POST, [](){
        if (s_instance) s_instance->handleConfigPost();
    });
    s_webServer->on("/api/motion/move", HTTP_POST, [](){
        if (s_instance) s_instance->handleMotionMove();
    });
    s_webServer->on("/api/motion/stop", HTTP_POST, [](){
        if (s_instance) s_instance->handleMotionStop();
    });
    s_webServer->on("/api/tracking/start", HTTP_POST, [](){
        if (s_instance) s_instance->handleTrackingStart();
    });
    s_webServer->on("/api/tracking/stop", HTTP_POST, [](){
        if (s_instance) s_instance->handleTrackingStop();
    });
    s_webServer->on("/api/tracking/status", HTTP_GET, [](){
        if (s_instance) s_instance->handleTrackingStatus();
    });
    s_webServer->on("/api/wifi/status", HTTP_GET, [](){
        if (s_instance) s_instance->handleWifiStatus();
    });
    s_webServer->on("/api/wifi/scan", HTTP_GET, [](){
        if (s_instance) s_instance->handleWifiScan();
    });
    s_webServer->on("/api/wifi/config", HTTP_POST, [](){
        if (s_instance) s_instance->handleWifiConfig();
    });
}

static void sendText(WebServer* server, int code, const char* type, const char* body) {
    if (!server) return;
    server->send(code, type, body);
}

static void sendJson(WebServer* server, int code, const char* json) {
    if (!server) return;
    server->send(code, "application/json", json);
}

// ============================================================
// Rotas
// ============================================================

void DashboardService::handleRoot() {
    const char* html = R"rawliteral(
<!DOCTYPE html>
<html lang="pt-BR">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>WindFollow</title>
<style>
body{font-family:sans-serif;background:#282a36;color:#f8f8f2;padding:20px;}
h1{color:#8be9fd;}
.card{background:#343746;padding:15px;border-radius:6px;margin:10px 0;border:1px solid #44475a;}
button{padding:8px 16px;border:none;border-radius:4px;cursor:pointer;background:#8be9fd;color:#1a1a1a;}
button.secondary{background:#44475a;color:#f8f8f2;}
input{padding:6px;border-radius:4px;border:1px solid #44475a;background:#21222c;color:#f8f8f2;}
.status{display:flex;gap:10px;align-items:center;}
.ok{color:#50fa7b;}
.err{color:#ff5555;}
</style>
</head>
<body>
<h1>🌙 WindFollow</h1>
<div class="status">
<span id="wifi-status" class="ok">● Wi-Fi: Configurando...</span>
</div>
<div class="card">
<h3>Controle do Motor</h3>
<button onclick="move('esquerda')">Esquerda</button>
<button onclick="move('parar')">Parar</button>
<button onclick="move('direita')">Direita</button>
</div>
<div class="card">
<h3>Rastreamento</h3>
<button class="secondary" onclick="toggleTracking()">Toggle Rastreamento</button>
<div id="track-status">Desligado</div>
</div>
<div class="card">
<h3>Configuração</h3>
<label>GPIO IN1: <input type="number" id="gpio_in1" value="21"></label><br>
<label>GPIO IN2: <input type="number" id="gpio_in2" value="22"></label><br>
<label>GPIO ENA: <input type="number" id="gpio_ena" value="23"></label><br>
<label>Tensão (V): <input type="number" id="motor_voltage" step="0.1" value="12"></label><br>
<button onclick="saveConfig()">Salvar</button>
</div>
<div id="msg" style="margin-top:10px;"></div>
<script>
async function api(method, path, body){
const r = await fetch(path, {method, headers:{'Content-Type':'application/json'}, body: body?JSON.stringify(body):undefined});
return r.json();
}
async function loadStatus(){
const d = await api('GET','/api/wifi/status');
document.getElementById('wifi-status').textContent = '● Wi-Fi: ' + (d.connected?d.ip:'Modo AP');
document.getElementById('wifi-status').className = d.connected?'ok':'err';
const t = await api('GET','/api/tracking/status');
document.getElementById('track-status').textContent = 'Rastreamento: ' + (t.tracking?'Ligado':'Desligado');
}
async function move(dir){
await api('POST','/api/motion/move',{direction:dir,duration_ms:150});
loadStatus();
}
async function toggleTracking(){
const t = await api('GET','/api/tracking/status');
if(t.tracking) await api('POST','/api/tracking/stop');
else await api('POST','/api/tracking/start');
loadStatus();
}
async function saveConfig(){
const cfg = {
gpio_in1: parseInt(document.getElementById('gpio_in1').value),
gpio_in2: parseInt(document.getElementById('gpio_in2').value),
gpio_ena: parseInt(document.getElementById('gpio_ena').value),
motor_voltage: parseFloat(document.getElementById('motor_voltage').value)
};
const r = await api('POST','/api/config',cfg);
document.getElementById('msg').textContent = r.message||'Salvo';
loadStatus();
}
setInterval(loadStatus,2000);
loadStatus();
</script>
</body>
</html>
)rawliteral";
    sendText(s_webServer, 200, "text/html", html);
}

void DashboardService::handleCss() {
    sendText(s_webServer, 200, "text/css", "");
}

void DashboardService::handleJs() {
    sendText(s_webServer, 200, "application/javascript", "");
}

void DashboardService::handleSystemInfo() {
    char buf[256];
    snprintf(buf, sizeof(buf),
        "{\"status\":\"ok\",\"uptime\":%lu,\"freeHeap\":%u}",
        millis()/1000, ESP.getFreeHeap());
    sendJson(s_webServer, 200, buf);
}

void DashboardService::handleNetworkInfo() {
    char buf[256];
    snprintf(buf, sizeof(buf),
        "{\"status\":\"ok\",\"ip\":\"%s\",\"ssid\":\"%s\",\"connected\":%s}",
        WiFi.localIP().toString().c_str(),
        WiFi.SSID().c_str(),
        WiFi.isConnected()?"true":"false");
    sendJson(s_webServer, 200, buf);
}

void DashboardService::handleWifiStatus() {
    char buf[256];
    bool configured = false;
    Preferences prefs;
    if (prefs.begin("windfollow", true)) {
        configured = prefs.getBool("wifi_configured", false);
        prefs.end();
    }
    if (WiFi.isConnected()) {
        snprintf(buf, sizeof(buf),
            "{\"status\":\"ok\",\"mode\":\"sta\",\"ip\":\"%s\",\"ssid\":\"%s\",\"connected\":true,\"configured\":true}",
            WiFi.localIP().toString().c_str(), WiFi.SSID().c_str());
    } else {
        snprintf(buf, sizeof(buf),
            "{\"status\":\"ok\",\"mode\":\"ap\",\"ip\":\"192.168.4.1\",\"ssid\":\"WindFollow-Setup\",\"connected\":false,\"configured\":%s}",
            configured?"true":"false");
    }
    sendJson(s_webServer, 200, buf);
}

void DashboardService::handleWifiScan() {
    int count = WiFi.scanNetworks();
    static char buf[1024];
    int pos = snprintf(buf, sizeof(buf), "{\"status\":\"ok\",\"networks\":[");
    for (int i = 0; i < count && pos < (int)sizeof(buf)-64; i++) {
        if (i > 0) pos += snprintf(buf+pos, sizeof(buf)-pos, ",");
        String ssid = WiFi.SSID(i);
        int rssi = WiFi.RSSI(i);
        pos += snprintf(buf+pos, sizeof(buf)-pos,
            "{\"ssid\":\"%s\",\"rssi\":%d}", ssid.c_str(), rssi);
    }
    snprintf(buf+pos, sizeof(buf)-pos, "]}");
    sendJson(s_webServer, 200, buf);
}

void DashboardService::handleWifiConfig() {
    String ssid = s_webServer->arg("ssid");
    String password = s_webServer->arg("password");
    if (ssid.length() == 0) {
        sendJson(s_webServer, 400, "{\"error\":\"Missing ssid\"}");
        return;
    }
    Preferences prefs;
    prefs.begin("windfollow", false);
    prefs.putString("wifi_ssid", ssid);
    prefs.putString("wifi_password", password);
    prefs.putBool("wifi_configured", true);
    prefs.end();
    sendJson(s_webServer, 200, "{\"status\":\"ok\",\"message\":\"Salvo. Reiniciando...\"}");
    delay(500);
    ESP.restart();
}

void DashboardService::handleConfigGet() {
    HardwareConfig cfg = configManager.getConfig();
    char buf[512];
    snprintf(buf, sizeof(buf),
        "{\"gpio_in1\":%d,\"gpio_in2\":%d,\"gpio_ena\":%d,\"motor_voltage\":%.1f,"
        "\"dead_zone\":%d,\"sensitivity\":%d,\"min_area\":%d,"
        "\"step_duration_ms\":%d,\"stabilization_ms\":%d,\"auto_tracking\":%s}",
        cfg.gpio_in1, cfg.gpio_in2, cfg.gpio_ena, cfg.motor_voltage,
        cfg.dead_zone, cfg.sensitivity, cfg.min_area,
        cfg.step_duration_ms, cfg.stabilization_ms,
        cfg.auto_tracking?"true":"false");
    sendJson(s_webServer, 200, buf);
}

void DashboardService::handleConfigPost() {
    String body = s_webServer->arg("plain");
    HardwareConfig cfg = configManager.getConfig();

    if (body.indexOf("gpio_in1") >= 0) {
        cfg.gpio_in1 = -1;
        cfg.gpio_in2 = -1;
        cfg.gpio_ena = -1;
        cfg.motor_voltage = 0;
    }
    configManager.saveConfig(cfg);
    sendJson(s_webServer, 200, "{\"status\":\"ok\",\"message\":\"Config salva. Reiniciando para aplicar pinos...\"}");
    delay(500);
    ESP.restart();
}

void DashboardService::handleMotionMove() {
    String dir = s_webServer->arg("direction");
    int dur = s_webServer->arg("duration_ms").toInt();
    if (dur == 0) dur = 150;
    if (dir == "esquerda") {
        l298nDriver.esquerda(dur, 200);
    } else if (dir == "direita") {
        l298nDriver.direita(dur, 200);
    } else {
        l298nDriver.stop();
    }
    sendJson(s_webServer, 200, "{\"status\":\"ok\"}");
}

void DashboardService::handleMotionStop() {
    l298nDriver.stop();
    sendJson(s_webServer, 200, "{\"status\":\"ok\"}");
}

void DashboardService::handleTrackingStart() {
    HardwareConfig cfg = configManager.getConfig();
    cfg.auto_tracking = true;
    configManager.saveConfig(cfg);
    sendJson(s_webServer, 200, "{\"status\":\"ok\",\"tracking\":true}");
}

void DashboardService::handleTrackingStop() {
    HardwareConfig cfg = configManager.getConfig();
    cfg.auto_tracking = false;
    configManager.saveConfig(cfg);
    l298nDriver.stop();
    sendJson(s_webServer, 200, "{\"status\":\"ok\",\"tracking\":false}");
}

void DashboardService::handleTrackingStatus() {
    HardwareConfig cfg = configManager.getConfig();
    char buf[128];
    snprintf(buf, sizeof(buf), "{\"tracking\":%s}", cfg.auto_tracking?"true":"false");
    sendJson(s_webServer, 200, buf);
}
