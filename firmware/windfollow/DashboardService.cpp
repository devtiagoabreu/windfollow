#include "DashboardService.h"
#include "HardwareConfig.h"
#include "ConfigManager.h"
#include "L298NDriver.h"
#include "MotionEngine.h"
#include "CameraService.h"
#include "RuntimeState.h"
#include "WebAssets.h"

#include <WebServer.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <Preferences.h>
#include <ctype.h>

static DashboardService* s_instance = nullptr;
static WebServer* s_webServer = nullptr;

// Instancias globais definidas em windfollow.ino
extern ConfigManager configManager;
extern L298NDriver l298nDriver;
extern MotionEngine motionEngine;

// ============================================================
// Utilitarios JSON
// ============================================================

static String jsonEscape(const String& in) {
    String out;
    out.reserve(in.length() + 8);
    for (size_t i = 0; i < in.length(); i++) {
        char c = in[i];
        if (c == '"' || c == '\\') {
            out += '\\';
            out += c;
        } else if ((unsigned char)c < 0x20) {
            char tmp[8];
            snprintf(tmp, sizeof(tmp), "\\u%04x", (unsigned int)(unsigned char)c);
            out += tmp;
        } else {
            out += c;
        }
    }
    return out;
}

static int jsonValueStart(const String& body, const char* key) {
    String pattern = String("\"") + key + "\"";
    int k = body.indexOf(pattern);
    if (k < 0) return -1;
    int colon = body.indexOf(':', k + pattern.length());
    if (colon < 0) return -1;
    int i = colon + 1;
    while (i < (int)body.length() && isspace((unsigned char)body[i])) i++;
    return i;
}

static bool jsonGetString(const String& body, const char* key, String& out) {
    int i = jsonValueStart(body, key);
    if (i < 0 || i >= (int)body.length() || body[i] != '"') return false;
    i++;
    String value;
    while (i < (int)body.length()) {
        char c = body[i++];
        if (c == '\\' && i < (int)body.length()) {
            value += body[i++];
        } else if (c == '"') {
            out = value;
            return true;
        } else {
            value += c;
        }
    }
    return false;
}

static long jsonGetLong(const String& body, const char* key, long defaultValue) {
    int i = jsonValueStart(body, key);
    if (i < 0 || i >= (int)body.length()) return defaultValue;
    int j = i;
    if (body[j] == '-' || body[j] == '+') j++;
    while (j < (int)body.length() && isdigit((unsigned char)body[j])) j++;
    if (j == i) return defaultValue;
    return body.substring(i, j).toInt();
}

static float jsonGetFloat(const String& body, const char* key, float defaultValue) {
    int i = jsonValueStart(body, key);
    if (i < 0 || i >= (int)body.length()) return defaultValue;
    int j = i;
    if (body[j] == '-' || body[j] == '+') j++;
    while (j < (int)body.length() &&
           (isdigit((unsigned char)body[j]) || body[j] == '.')) j++;
    if (j == i) return defaultValue;
    return body.substring(i, j).toFloat();
}

static bool jsonGetBool(const String& body, const char* key, bool defaultValue) {
    int i = jsonValueStart(body, key);
    if (i < 0 || i >= (int)body.length()) return defaultValue;
    if (body.startsWith("true", i)) return true;
    if (body.startsWith("false", i)) return false;
    return defaultValue;
}

static void sendJson(WebServer* server, int code, const char* json) {
    if (!server) return;
    server->send(code, "application/json", json);
}

static void sendJsonError(WebServer* server, int code, const char* message) {
    if (!server) return;
    String escaped = jsonEscape(String(message));
    static char buf[320];
    snprintf(buf, sizeof(buf), "{\"status\":\"error\",\"error\":\"%s\"}", escaped.c_str());
    server->send(code, "application/json", buf);
}

// ============================================================
// Ciclo de vida
// ============================================================

DashboardService::DashboardService()
    : m_running(false) {}

DashboardService::~DashboardService() {
    deinit();
}

bool DashboardService::init() {
    if (m_running) return true;

    s_webServer = new WebServer(80);
    if (!s_webServer) return false;

    s_instance = this;
    registerRoutes();
    s_webServer->begin();

    m_running = true;
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

bool DashboardService::servePage(const char* page) {
    handleRoot();
    return true;
}

bool DashboardService::registerPage(const char* route, const char* filePath) {
    (void)route;
    (void)filePath;
    return false;
}

bool DashboardService::setSystemInfo(const char* key, const char* value) {
    (void)key;
    (void)value;
    return false;
}

bool DashboardService::broadcastEvent(const char* eventType, const char* jsonData) {
    (void)eventType;
    (void)jsonData;
    return false;
}

// ============================================================
// Rotas
// ============================================================

void DashboardService::registerRoutes() {
    if (!s_webServer) return;

    s_webServer->on("/", HTTP_GET, []() { if (s_instance) s_instance->handleRoot(); });
    s_webServer->on("/index.html", HTTP_GET, []() { if (s_instance) s_instance->handleRoot(); });
    s_webServer->on("/setup", HTTP_GET, []() { if (s_instance) s_instance->handleRoot(); });

    s_webServer->on("/css/style.css", HTTP_GET, []() { if (s_instance) s_instance->handleCss(); });
    s_webServer->on("/style.css", HTTP_GET, []() { if (s_instance) s_instance->handleCss(); });
    s_webServer->on("/js/app.js", HTTP_GET, []() { if (s_instance) s_instance->handleJs(); });
    s_webServer->on("/app.js", HTTP_GET, []() { if (s_instance) s_instance->handleJs(); });

    s_webServer->on("/api/config", HTTP_GET, []() { if (s_instance) s_instance->handleConfigGet(); });
    s_webServer->on("/api/config", HTTP_POST, []() { if (s_instance) s_instance->handleConfigPost(); });
    s_webServer->on("/api/config/reset", HTTP_POST, []() { if (s_instance) s_instance->handleConfigReset(); });

    s_webServer->on("/api/motion/move", HTTP_POST, []() { if (s_instance) s_instance->handleMotionMove(); });
    s_webServer->on("/api/motion/stop", HTTP_POST, []() { if (s_instance) s_instance->handleMotionStop(); });

    s_webServer->on("/api/tracking/start", HTTP_POST, []() { if (s_instance) s_instance->handleTrackingStart(); });
    s_webServer->on("/api/tracking/stop", HTTP_POST, []() { if (s_instance) s_instance->handleTrackingStop(); });
    s_webServer->on("/api/tracking/status", HTTP_GET, []() { if (s_instance) s_instance->handleTrackingStatus(); });

    s_webServer->on("/api/wifi/status", HTTP_GET, []() { if (s_instance) s_instance->handleWifiStatus(); });
    s_webServer->on("/api/wifi/scan", HTTP_GET, []() { if (s_instance) s_instance->handleWifiScan(); });
    s_webServer->on("/api/wifi/config", HTTP_POST, []() { if (s_instance) s_instance->handleWifiConfig(); });

    s_webServer->on("/api/camera/start", HTTP_POST, []() { if (s_instance) s_instance->handleCameraStart(); });
    s_webServer->on("/api/camera/stop", HTTP_POST, []() { if (s_instance) s_instance->handleCameraStop(); });
    s_webServer->on("/api/camera/status", HTTP_GET, []() { if (s_instance) s_instance->handleCameraStatus(); });
    s_webServer->on("/camera/capture", HTTP_GET, []() { if (s_instance) s_instance->handleCameraCapture(); });

    s_webServer->on("/api/system/info", HTTP_GET, []() { if (s_instance) s_instance->handleSystemInfo(); });
    s_webServer->on("/api/network/info", HTTP_GET, []() { if (s_instance) s_instance->handleNetworkInfo(); });
}

// ============================================================
// Paginas
// ============================================================

void DashboardService::handleRoot() {
    if (!s_webServer) return;
    s_webServer->sendHeader("Cache-Control", "no-store");
    s_webServer->send_P(200, "text/html", WIND_FOLLOW_INDEX_HTML);
}

void DashboardService::handleCss() {
    if (!s_webServer) return;
    s_webServer->sendHeader("Cache-Control", "max-age=3600");
    s_webServer->send_P(200, "text/css", WIND_FOLLOW_STYLE_CSS);
}

void DashboardService::handleJs() {
    if (!s_webServer) return;
    s_webServer->sendHeader("Cache-Control", "max-age=3600");
    s_webServer->send_P(200, "application/javascript", WIND_FOLLOW_APP_JS);
}

// ============================================================
// Sistema / rede
// ============================================================

void DashboardService::handleSystemInfo() {
    char buf[256];
    snprintf(buf, sizeof(buf),
        "{\"status\":\"ok\",\"uptime\":%lu,\"freeHeap\":%u,\"psram\":%u}",
        millis() / 1000, ESP.getFreeHeap(), ESP.getFreePsram());
    sendJson(s_webServer, 200, buf);
}

void DashboardService::handleNetworkInfo() {
    char buf[384];
    snprintf(buf, sizeof(buf),
        "{\"status\":\"ok\",\"ip\":\"%s\",\"ssid\":\"%s\",\"connected\":%s,\"apIp\":\"%s\"}",
        WiFi.localIP().toString().c_str(),
        jsonEscape(WiFi.SSID()).c_str(),
        WiFi.isConnected() ? "true" : "false",
        WiFi.softAPIP().toString().c_str());
    sendJson(s_webServer, 200, buf);
}

// ============================================================
// Wi-Fi
// ============================================================

void DashboardService::handleWifiStatus() {
    bool connected = WiFi.isConnected();
    bool configured = false;
    Preferences prefs;
    if (prefs.begin("windfollow", true)) {
        configured = prefs.getBool("wifi_configured", false);
        prefs.end();
    }

    String ssid = connected ? WiFi.SSID() : String("WindFollow-Setup");
    String ip = connected ? WiFi.localIP().toString() : WiFi.softAPIP().toString();

    char buf[512];
    snprintf(buf, sizeof(buf),
        "{\"status\":\"ok\",\"mode\":\"%s\",\"ip\":\"%s\",\"ssid\":\"%s\","
        "\"connected\":%s,\"configured\":%s,\"apIp\":\"%s\",\"mdns\":\"windfollow.local\"}",
        connected ? "sta" : "ap",
        ip.c_str(),
        jsonEscape(ssid).c_str(),
        connected ? "true" : "false",
        configured ? "true" : "false",
        WiFi.softAPIP().toString().c_str());
    sendJson(s_webServer, 200, buf);
}

void DashboardService::handleWifiScan() {
    int count = WiFi.scanNetworks();
    String json;
    json.reserve(1024);
    json = "{\"status\":\"ok\",\"networks\":[";
    int added = 0;

    for (int i = 0; i < count && added < 20; i++) {
        String ssid = WiFi.SSID(i);
        if (ssid.length() == 0) continue;
        if (added > 0) json += ',';
        json += "{\"ssid\":\"";
        json += jsonEscape(ssid);
        json += "\",\"rssi\":";
        json += String((int)WiFi.RSSI(i));
        json += ",\"encrypted\":";
        json += (WiFi.encryptionType(i) != WIFI_AUTH_OPEN) ? "true" : "false";
        json += '}';
        added++;
    }

    json += "]}";
    WiFi.scanDelete();
    sendJson(s_webServer, 200, json.c_str());
}

void DashboardService::handleWifiConfig() {
    String body = s_webServer->arg("plain");
    String ssid;
    String password;

    bool hasJson = jsonGetString(body, "ssid", ssid);
    if (hasJson) {
        jsonGetString(body, "password", password);
    } else {
        ssid = s_webServer->arg("ssid");
        password = s_webServer->arg("password");
    }

    if (ssid.length() == 0) {
        sendJsonError(s_webServer, 400, "Informe o SSID da rede");
        return;
    }

    Preferences prefs;
    prefs.begin("windfollow", false);
    prefs.putString("wifi_ssid", ssid);
    prefs.putString("wifi_password", password);
    prefs.putBool("wifi_configured", true);
    prefs.end();

    sendJson(s_webServer, 200,
        "{\"status\":\"ok\",\"message\":\"Rede salva. Reiniciando para conectar...\"}");
    delay(1200);
    ESP.restart();
}

// ============================================================
// Configuracao de hardware
// ============================================================

void DashboardService::handleConfigGet() {
    HardwareConfig cfg = configManager.getConfig();
    char buf[512];
    snprintf(buf, sizeof(buf),
        "{\"gpio_in1\":%d,\"gpio_in2\":%d,\"gpio_ena\":%d,\"motor_voltage\":%.1f,"
        "\"motor_enabled\":%s,\"dead_zone\":%d,\"sensitivity\":%d,\"min_area\":%d,"
        "\"step_duration_ms\":%d,\"stabilization_ms\":%d,\"auto_tracking\":%s}",
        cfg.gpio_in1, cfg.gpio_in2, cfg.gpio_ena, cfg.motor_voltage,
        cfg.motor_enabled ? "true" : "false",
        cfg.dead_zone, cfg.sensitivity, cfg.min_area,
        cfg.step_duration_ms, cfg.stabilization_ms,
        cfg.auto_tracking ? "true" : "false");
    sendJson(s_webServer, 200, buf);
}

void DashboardService::handleConfigPost() {
    String body = s_webServer->arg("plain");
    HardwareConfig oldCfg = configManager.getConfig();
    HardwareConfig cfg = oldCfg;

    if (body.length() > 0) {
        cfg.gpio_in1 = (int)jsonGetLong(body, "gpio_in1", cfg.gpio_in1);
        cfg.gpio_in2 = (int)jsonGetLong(body, "gpio_in2", cfg.gpio_in2);
        cfg.gpio_ena = (int)jsonGetLong(body, "gpio_ena", cfg.gpio_ena);
        cfg.motor_voltage = jsonGetFloat(body, "motor_voltage", cfg.motor_voltage);
        cfg.dead_zone = (int)jsonGetLong(body, "dead_zone", cfg.dead_zone);
        cfg.sensitivity = (int)jsonGetLong(body, "sensitivity", cfg.sensitivity);
        cfg.min_area = (int)jsonGetLong(body, "min_area", cfg.min_area);
        cfg.step_duration_ms = (int)jsonGetLong(body, "step_duration_ms", cfg.step_duration_ms);
        cfg.stabilization_ms = (int)jsonGetLong(body, "stabilization_ms", cfg.stabilization_ms);
        cfg.auto_tracking = jsonGetBool(body, "auto_tracking", cfg.auto_tracking);
    }

    bool configuring = (cfg.gpio_in1 >= 0 || cfg.gpio_in2 >= 0 || cfg.gpio_ena >= 0);
    if (configuring) {
        String error;
        if (!configManager.validateConfig(cfg, error)) {
            sendJsonError(s_webServer, 400, error.c_str());
            return;
        }
        cfg.motor_enabled = true;
    } else {
        cfg.motor_voltage = 0.0f;
        cfg.motor_enabled = false;
    }

    bool gpioChanged = (oldCfg.gpio_in1 != cfg.gpio_in1) ||
                       (oldCfg.gpio_in2 != cfg.gpio_in2) ||
                       (oldCfg.gpio_ena != cfg.gpio_ena);

    configManager.saveConfig(cfg);

    char buf[256];
    snprintf(buf, sizeof(buf),
        "{\"status\":\"ok\",\"restart\":%s,\"message\":\"%s\"}",
        gpioChanged ? "true" : "false",
        gpioChanged
            ? "Config salva. Reiniciando para aplicar os pinos..."
            : "Configuracao salva e aplicada.");
    sendJson(s_webServer, 200, buf);

    if (gpioChanged) {
        delay(1200);
        ESP.restart();
    }
}

void DashboardService::handleConfigReset() {
    configManager.resetConfig();
    sendJson(s_webServer, 200,
        "{\"status\":\"ok\",\"restart\":true,\"message\":\"Defaults restaurados. Reiniciando...\"}");
    delay(1200);
    ESP.restart();
}

// ============================================================
// Movimento
// ============================================================

void DashboardService::handleMotionMove() {
    if (!motionEngine.isConfigured()) {
        sendJsonError(s_webServer, 400, "Motor nao configurado. Defina IN1, IN2 e ENA.");
        return;
    }

    String body = s_webServer->arg("plain");
    String direction;
    if (!jsonGetString(body, "direction", direction)) {
        direction = s_webServer->arg("direction");
    }

    long duration = jsonGetLong(body, "duration_ms", 150);
    if (duration <= 0) duration = 150;

    if (direction == "esquerda" || direction == "left") {
        motionEngine.esquerda((int)duration, 200);
    } else if (direction == "direita" || direction == "right") {
        motionEngine.direita((int)duration, 200);
    } else {
        motionEngine.stop();
    }

    char buf[160];
    snprintf(buf, sizeof(buf),
        "{\"status\":\"ok\",\"moving\":%s,\"direction\":\"%s\"}",
        motionEngine.isMoving() ? "true" : "false",
        l298nDriver.directionName());
    sendJson(s_webServer, 200, buf);
}

void DashboardService::handleMotionStop() {
    motionEngine.stop();
    sendJson(s_webServer, 200, "{\"status\":\"ok\",\"moving\":false}");
}

// ============================================================
// Rastreamento
// ============================================================

void DashboardService::handleTrackingStart() {
    cameraService.ensureStarted();

    HardwareConfig cfg = configManager.getConfig();
    cfg.auto_tracking = true;
    configManager.saveConfig(cfg);
    g_trackingStatus.tracking = true;

    char buf[160];
    snprintf(buf, sizeof(buf),
        "{\"status\":\"ok\",\"tracking\":true,\"camera\":%s}",
        cameraService.isReady() ? "true" : "false");
    sendJson(s_webServer, 200, buf);
}

void DashboardService::handleTrackingStop() {
    HardwareConfig cfg = configManager.getConfig();
    cfg.auto_tracking = false;
    configManager.saveConfig(cfg);
    motionEngine.stop();
    g_trackingStatus.tracking = false;
    sendJson(s_webServer, 200, "{\"status\":\"ok\",\"tracking\":false}");
}

void DashboardService::handleTrackingStatus() {
    bool connected = WiFi.isConnected();
    String ssid = connected ? WiFi.SSID() : String("WindFollow-Setup");
    String ip = connected ? WiFi.localIP().toString() : WiFi.softAPIP().toString();

    static char buf[1024];
    snprintf(buf, sizeof(buf),
        "{\"tracking\":%s,\"motion\":%s,"
        "\"centerX\":%d,\"centerY\":%d,\"errorX\":%d,"
        "\"frameWidth\":%d,\"frameHeight\":%d,"
        "\"box\":{\"x\":%d,\"y\":%d,\"w\":%d,\"h\":%d},"
        "\"direction\":\"%s\","
        "\"motor\":{\"moving\":%s,\"direction\":\"%s\"},"
        "\"camera\":{\"ready\":%s,\"enabled\":%s,\"sensor\":\"%s\",\"frames\":%d},"
        "\"wifi\":{\"connected\":%s,\"ip\":\"%s\",\"ssid\":\"%s\","
        "\"apIp\":\"%s\",\"mdns\":\"windfollow.local\"}}",
        g_trackingStatus.tracking ? "true" : "false",
        g_trackingStatus.motionDetected ? "true" : "false",
        g_trackingStatus.centerX, g_trackingStatus.centerY, g_trackingStatus.errorX,
        g_trackingStatus.frameWidth, g_trackingStatus.frameHeight,
        g_trackingStatus.boxX, g_trackingStatus.boxY,
        g_trackingStatus.boxW, g_trackingStatus.boxH,
        g_trackingStatus.direction,
        l298nDriver.isMoving() ? "true" : "false",
        l298nDriver.directionName(),
        g_cameraStatus.ready ? "true" : "false",
        g_cameraStatus.enabled ? "true" : "false",
        g_cameraStatus.sensor,
        g_cameraStatus.frameCount,
        connected ? "true" : "false",
        ip.c_str(),
        jsonEscape(ssid).c_str(),
        WiFi.softAPIP().toString().c_str());

    sendJson(s_webServer, 200, buf);
}

// ============================================================
// Camera
// ============================================================

void DashboardService::handleCameraStart() {
    if (!cameraService.ensureStarted()) {
        sendJson(s_webServer, 200,
            "{\"status\":\"error\",\"ready\":false,"
            "\"message\":\"Camera indisponivel. Confira a pinagem da T-SIMCAM e o log serial.\"}");
        return;
    }

    cameraService.setEnabled(true);

    char buf[192];
    snprintf(buf, sizeof(buf),
        "{\"status\":\"ok\",\"ready\":true,\"sensor\":\"%s\",\"width\":%d,\"height\":%d}",
        cameraService.sensorName(), cameraService.width(), cameraService.height());
    sendJson(s_webServer, 200, buf);
}

void DashboardService::handleCameraStop() {
    cameraService.setEnabled(false);
    sendJson(s_webServer, 200, "{\"status\":\"ok\",\"ready\":true,\"enabled\":false}");
}

void DashboardService::handleCameraStatus() {
    char buf[256];
    snprintf(buf, sizeof(buf),
        "{\"status\":\"ok\",\"ready\":%s,\"available\":%s,\"enabled\":%s,"
        "\"sensor\":\"%s\",\"width\":%d,\"height\":%d,\"frames\":%d}",
        cameraService.isReady() ? "true" : "false",
        cameraService.isAvailable() ? "true" : "false",
        cameraService.isEnabled() ? "true" : "false",
        cameraService.sensorName(),
        cameraService.width(), cameraService.height(),
        cameraService.frameCount());
    sendJson(s_webServer, 200, buf);
}

void DashboardService::handleCameraCapture() {
    if (!cameraService.isReady() || !cameraService.isEnabled()) {
        sendJsonError(s_webServer, 503, "Camera desativada ou indisponivel");
        return;
    }

    size_t length = 0;
    const char* base64 = cameraService.captureBase64(length);
    if (!base64 || length == 0) {
        sendJsonError(s_webServer, 500, "Falha ao capturar imagem");
        return;
    }

    g_cameraStatus.frameCount = cameraService.frameCount();
    g_cameraStatus.width = cameraService.width();
    g_cameraStatus.height = cameraService.height();

    s_webServer->sendHeader("Cache-Control", "no-store, no-cache, must-revalidate");
    s_webServer->send(200, "text/plain", base64);
}
