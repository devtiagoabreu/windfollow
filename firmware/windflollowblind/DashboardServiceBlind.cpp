#include "DashboardServiceBlind.h"
#include "HardwareConfig.h"
#include "ConfigManager.h"
#include "L298NDualDriver.h"
#include "MotorEngineBlind.h"
#include "WebAssets.h"

#include <WebServer.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <Preferences.h>
#include <ctype.h>

static DashboardServiceBlind* s_instance = nullptr;
static WebServer* s_webServer = nullptr;

// Instancias globais definidas em windflollowblind.ino
extern ConfigManager configManager;
extern L298NDualDriver l298nDualDriver;
extern MotorEngineBlind motorEngineBlind;

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

DashboardServiceBlind::DashboardServiceBlind()
    : m_running(false) {}

DashboardServiceBlind::~DashboardServiceBlind() {
    deinit();
}

bool DashboardServiceBlind::init() {
    if (m_running) return true;

    s_webServer = new WebServer(80);
    if (!s_webServer) return false;

    s_instance = this;
    registerRoutes();
    s_webServer->begin();

    m_running = true;
    return true;
}

void DashboardServiceBlind::tick() {
    if (s_webServer) {
        s_webServer->handleClient();
    }
}

bool DashboardServiceBlind::deinit() {
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

bool DashboardServiceBlind::isRunning() const {
    return m_running;
}

bool DashboardServiceBlind::servePage(const char* page) {
    handleRoot();
    return true;
}

bool DashboardServiceBlind::registerPage(const char* route, const char* filePath) {
    (void)route;
    (void)filePath;
    return false;
}

bool DashboardServiceBlind::setSystemInfo(const char* key, const char* value) {
    (void)key;
    (void)value;
    return false;
}

bool DashboardServiceBlind::broadcastEvent(const char* eventType, const char* jsonData) {
    (void)eventType;
    (void)jsonData;
    return false;
}

// ============================================================
// Rotas
// ============================================================

void DashboardServiceBlind::registerRoutes() {
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

    s_webServer->on("/api/motor/move", HTTP_POST, []() { if (s_instance) s_instance->handleMotorMove(); });
    s_webServer->on("/api/motor/stop", HTTP_POST, []() { if (s_instance) s_instance->handleMotorStop(); });
    s_webServer->on("/api/motor/status", HTTP_GET, []() { if (s_instance) s_instance->handleMotorStatus(); });

    s_webServer->on("/api/wifi/status", HTTP_GET, []() { if (s_instance) s_instance->handleWifiStatus(); });
    s_webServer->on("/api/wifi/scan", HTTP_GET, []() { if (s_instance) s_instance->handleWifiScan(); });
    s_webServer->on("/api/wifi/config", HTTP_POST, []() { if (s_instance) s_instance->handleWifiConfig(); });

    s_webServer->on("/api/system/info", HTTP_GET, []() { if (s_instance) s_instance->handleSystemInfo(); });
    s_webServer->on("/api/network/info", HTTP_GET, []() { if (s_instance) s_instance->handleNetworkInfo(); });

    // Captive Portal: qualquer requisicao desconhecida redireciona para a pagina inicial.
    // Assim, quando um dispositivo se conecta ao AP e abre o navegador, ele carrega
    // automaticamente a pagina de controle do WindFollowBlind.
    s_webServer->onNotFound([]() {
        if (s_instance) {
            s_webServer->sendHeader("Location", "/");
            s_webServer->send(302, "text/plain", "");
        }
    });
}

// ============================================================
// Paginas
// ============================================================

void DashboardServiceBlind::handleRoot() {
    if (!s_webServer) return;
    s_webServer->sendHeader("Cache-Control", "no-store");
    s_webServer->send_P(200, "text/html", WIND_FOLLOW_INDEX_HTML);
}

void DashboardServiceBlind::handleCss() {
    if (!s_webServer) return;
    s_webServer->sendHeader("Cache-Control", "max-age=3600");
    s_webServer->send_P(200, "text/css", WIND_FOLLOW_STYLE_CSS);
}

void DashboardServiceBlind::handleJs() {
    if (!s_webServer) return;
    s_webServer->sendHeader("Cache-Control", "max-age=3600");
    s_webServer->send_P(200, "application/javascript", WIND_FOLLOW_APP_JS);
}

// ============================================================
// Sistema / rede
// ============================================================

void DashboardServiceBlind::handleSystemInfo() {
    char buf[256];
    snprintf(buf, sizeof(buf),
        "{\"status\":\"ok\",\"uptime\":%lu,\"freeHeap\":%u,\"psram\":%u}",
        millis() / 1000, ESP.getFreeHeap(), ESP.getFreePsram());
    sendJson(s_webServer, 200, buf);
}

void DashboardServiceBlind::handleNetworkInfo() {
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

void DashboardServiceBlind::handleWifiStatus() {
    bool connected = WiFi.isConnected();
    bool configured = false;
    Preferences prefs;
    if (prefs.begin("windflollowblind", true)) {
        configured = prefs.getBool("wifi_configured", false);
        prefs.end();
    }

    String ssid = connected ? WiFi.SSID() : String("WindFollowBlind-Setup");
    String ip = connected ? WiFi.localIP().toString() : WiFi.softAPIP().toString();

    char buf[512];
    snprintf(buf, sizeof(buf),
        "{\"status\":\"ok\",\"mode\":\"%s\",\"ip\":\"%s\",\"ssid\":\"%s\","
        "\"connected\":%s,\"configured\":%s,\"apIp\":\"%s\",\"mdns\":\"windflollowblind.local\"}",
        connected ? "sta" : "ap",
        ip.c_str(),
        jsonEscape(ssid).c_str(),
        connected ? "true" : "false",
        configured ? "true" : "false",
        WiFi.softAPIP().toString().c_str());
    sendJson(s_webServer, 200, buf);
}

void DashboardServiceBlind::handleWifiScan() {
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

void DashboardServiceBlind::handleWifiConfig() {
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
    prefs.begin("windflollowblind", false);
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

void DashboardServiceBlind::handleConfigGet() {
    HardwareConfig cfg = configManager.getConfig();
    char buf[768];
    snprintf(buf, sizeof(buf),
        "{\"gpio_in1\":%d,\"gpio_in2\":%d,\"gpio_ena\":%d,"
        "\"gpio_in3\":%d,\"gpio_in4\":%d,\"gpio_enb\":%d,"
        "\"gpio_btn1\":%d,\"gpio_btn2\":%d,"
        "\"step_duration_ms\":%d,"
        "\"motor1_enabled\":%s,\"motor2_enabled\":%s}",
        cfg.gpio_in1, cfg.gpio_in2, cfg.gpio_ena,
        cfg.gpio_in3, cfg.gpio_in4, cfg.gpio_enb,
        cfg.gpio_btn1, cfg.gpio_btn2,
        cfg.step_duration_ms,
        cfg.motor1_enabled ? "true" : "false",
        cfg.motor2_enabled ? "true" : "false");
    sendJson(s_webServer, 200, buf);
}

void DashboardServiceBlind::handleConfigPost() {
    String body = s_webServer->arg("plain");
    HardwareConfig oldCfg = configManager.getConfig();
    HardwareConfig cfg = oldCfg;

    if (body.length() > 0) {
        cfg.gpio_in1 = (int)jsonGetLong(body, "gpio_in1", cfg.gpio_in1);
        cfg.gpio_in2 = (int)jsonGetLong(body, "gpio_in2", cfg.gpio_in2);
        cfg.gpio_ena = (int)jsonGetLong(body, "gpio_ena", cfg.gpio_ena);
        cfg.gpio_in3 = (int)jsonGetLong(body, "gpio_in3", cfg.gpio_in3);
        cfg.gpio_in4 = (int)jsonGetLong(body, "gpio_in4", cfg.gpio_in4);
        cfg.gpio_enb = (int)jsonGetLong(body, "gpio_enb", cfg.gpio_enb);
        cfg.gpio_btn1 = (int)jsonGetLong(body, "gpio_btn1", cfg.gpio_btn1);
        cfg.gpio_btn2 = (int)jsonGetLong(body, "gpio_btn2", cfg.gpio_btn2);
        cfg.step_duration_ms = (int)jsonGetLong(body, "step_duration_ms", cfg.step_duration_ms);
        cfg.motor1_enabled = jsonGetBool(body, "motor1_enabled", cfg.motor1_enabled);
        cfg.motor2_enabled = jsonGetBool(body, "motor2_enabled", cfg.motor2_enabled);
    }

    bool configuring = (cfg.gpio_in1 >= 0 || cfg.gpio_in2 >= 0 ||
                        cfg.gpio_in3 >= 0 || cfg.gpio_in4 >= 0 ||
                        cfg.gpio_btn1 >= 0 || cfg.gpio_btn2 >= 0);
    if (configuring) {
        String error;
        if (!configManager.validateConfig(cfg, error)) {
            sendJsonError(s_webServer, 400, error.c_str());
            return;
        }
    }

    bool gpioChanged = (oldCfg.gpio_in1 != cfg.gpio_in1) ||
                       (oldCfg.gpio_in2 != cfg.gpio_in2) ||
                       (oldCfg.gpio_ena != cfg.gpio_ena) ||
                       (oldCfg.gpio_in3 != cfg.gpio_in3) ||
                       (oldCfg.gpio_in4 != cfg.gpio_in4) ||
                       (oldCfg.gpio_enb != cfg.gpio_enb) ||
                       (oldCfg.gpio_btn1 != cfg.gpio_btn1) ||
                       (oldCfg.gpio_btn2 != cfg.gpio_btn2);

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

void DashboardServiceBlind::handleConfigReset() {
    configManager.resetConfig();
    sendJson(s_webServer, 200,
        "{\"status\":\"ok\",\"restart\":true,\"message\":\"Defaults restaurados. Reiniciando...\"}");
    delay(1200);
    ESP.restart();
}

// ============================================================
// Motor
// ============================================================

void DashboardServiceBlind::handleMotorMove() {
    if (!motorEngineBlind.isConfigured()) {
        sendJsonError(s_webServer, 400, "Motor nao configurado. Defina os GPIOs.");
        return;
    }

    String body = s_webServer->arg("plain");
    String motor;
    String direction;
    if (!jsonGetString(body, "motor", motor)) {
        motor = s_webServer->arg("motor");
    }
    if (!jsonGetString(body, "direction", direction)) {
        direction = s_webServer->arg("direction");
    }

    long duration = jsonGetLong(body, "duration_ms", 150);
    if (duration <= 0) duration = 150;

    if (motor == "1" || motor == "motor1") {
        if (direction == "esquerda" || direction == "left") {
            motorEngineBlind.motor1Esquerda((int)duration, 200);
        } else if (direction == "direita" || direction == "right") {
            motorEngineBlind.motor1Direita((int)duration, 200);
        } else {
            motorEngineBlind.stopMotor1();
        }
    } else if (motor == "2" || motor == "motor2") {
        if (direction == "esquerda" || direction == "left") {
            motorEngineBlind.motor2Esquerda((int)duration, 200);
        } else if (direction == "direita" || direction == "right") {
            motorEngineBlind.motor2Direita((int)duration, 200);
        } else {
            motorEngineBlind.stopMotor2();
        }
    } else {
        motorEngineBlind.stop();
    }

    char buf[256];
    snprintf(buf, sizeof(buf),
        "{\"status\":\"ok\",\"moving1\":%s,\"moving2\":%s,"
        "\"direction1\":\"%s\",\"direction2\":\"%s\"}",
        motorEngineBlind.isMoving1() ? "true" : "false",
        motorEngineBlind.isMoving2() ? "true" : "false",
        l298nDualDriver.directionName1(),
        l298nDualDriver.directionName2());
    sendJson(s_webServer, 200, buf);
}

void DashboardServiceBlind::handleMotorStop() {
    motorEngineBlind.stop();
    sendJson(s_webServer, 200, "{\"status\":\"ok\",\"moving1\":false,\"moving2\":false}");
}

void DashboardServiceBlind::handleMotorStatus() {
    char buf[320];
    snprintf(buf, sizeof(buf),
        "{\"moving1\":%s,\"moving2\":%s,"
        "\"direction1\":\"%s\",\"direction2\":\"%s\","
        "\"step_duration_ms\":%d}",
        l298nDualDriver.isMoving1() ? "true" : "false",
        l298nDualDriver.isMoving2() ? "true" : "false",
        l298nDualDriver.directionName1(),
        l298nDualDriver.directionName2(),
        motorEngineBlind.stepDurationMs());
    sendJson(s_webServer, 200, buf);
}
