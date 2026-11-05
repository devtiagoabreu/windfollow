/*
 * WindFollow — Câmera Seguidora por Movimento
 * Hardware: LilyGO T-SIMCAM ESP32-S3 V1.3 + L298N + Motorredutor DC
 * Framework: Arduino ESP32
 *
 * Entrada do firmware. Inicializa todos os módulos e o loop principal.
 */

#include <Arduino.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <Preferences.h>

// ============================================================
// Modulos do projeto
// ============================================================
#include "HardwareConfig.h"
#include "ConfigManager.h"
#include "L298NDriver.h"
#include "MotionEngine.h"
#include "MotionDetector.h"
#include "CameraService.h"
#include "RuntimeState.h"
#include "DashboardService.h"

// ============================================================
// Parametros de deteccao de movimento
// ============================================================
#define WF_DETECT_W 160
#define WF_DETECT_H 120

// ============================================================
// Instâncias globais
// ============================================================
ConfigManager   configManager;
L298NDriver     l298nDriver;
MotionEngine    motionEngine;
MotionDetector  motionDetector;
DashboardService dashboardService;

SystemState g_systemState = SystemState::Boot;

TrackingStatus g_trackingStatus;
CameraStatus   g_cameraStatus;

// ============================================================
// Declarações
// ============================================================
void setupConfig();
void setupCore();
void setupNetwork();
void setupCamera();
void setupMotion();
void setupTracking();
void setupDashboard();

void loopCore();
void loopNetwork();
void loopCamera();
void loopMotion();
void loopTracking();
void loopDashboard();

// ============================================================
// setup()
// ============================================================
void setup() {
    Serial.begin(115200);
    delay(100);

    Serial.println();
    Serial.println(F("WindFollow v1.0.0"));
    Serial.println(F("Platform: ESP32-S3 / LilyGO T-SIMCAM"));
    Serial.println(F("Driver: L298N + Motorredutor DC"));

    pinMode(1, OUTPUT);
    digitalWrite(1, HIGH);

    g_systemState = SystemState::Init;

    setupConfig();
    delay(100);

    setupCore();

    setupNetwork();
    setupCamera();

    setupMotion();
    setupTracking();
    setupDashboard();

    g_systemState = SystemState::Ready;
    Serial.println(F("WindFollow ready - all systems initialized"));
}

// ============================================================
// loop()
// ============================================================
void loop() {
    loopCore();
    loopNetwork();
    loopCamera();
    loopMotion();
    loopTracking();
    loopDashboard();
}

// ============================================================
// Setup
// ============================================================
void setupConfig() {
    configManager.begin();
    HardwareConfig cfg = configManager.getConfig();
    g_trackingStatus.tracking = cfg.auto_tracking;
    Serial.printf("Config: IN1=%d IN2=%d ENA=%d V=%.1f\n",
        cfg.gpio_in1, cfg.gpio_in2, cfg.gpio_ena, cfg.motor_voltage);
}

void setupCore() {
    // Estado do sistema inicializado em g_systemState
}

void setupNetwork() {
    Preferences prefs;
    prefs.begin("windfollow", true);
    String ssid = prefs.getString("wifi_ssid", "");
    String password = prefs.getString("wifi_password", "");
    prefs.end();

    // AP sempre disponivel como fallback de configuracao.
    WiFi.mode(WIFI_AP_STA);
    WiFi.softAP("WindFollow-Setup", "windfollow123");
    WiFi.setSleep(false);

    Serial.print(F("AP SSID: WindFollow-Setup  IP: "));
    Serial.println(WiFi.softAPIP());

    if (ssid.length() > 0) {
        Serial.print(F("Conectando a rede salva: "));
        Serial.println(ssid);
        WiFi.begin(ssid.c_str(), password.c_str());

        unsigned long start = millis();
        while (WiFi.status() != WL_CONNECTED && (millis() - start) < 15000) {
            delay(250);
            Serial.print('.');
        }
        Serial.println();
        if (WiFi.isConnected()) {
            Serial.print(F("Conectado. IP: "));
            Serial.println(WiFi.localIP());
        } else {
            Serial.println(F("Falha ao conectar. Mantendo AP de configuracao."));
        }
    }

    if (MDNS.begin("windfollow")) {
        MDNS.addService("http", "tcp", 80);
        Serial.println(F("mDNS: http://windfollow.local"));
    } else {
        Serial.println(F("Falha ao iniciar mDNS"));
    }
}

void setupCamera() {
    bool ok = cameraService.begin();
    g_cameraStatus.ready = ok;
    g_cameraStatus.available = cameraService.isAvailable();
    g_cameraStatus.enabled = cameraService.isEnabled();
    g_cameraStatus.width = cameraService.width();
    g_cameraStatus.height = cameraService.height();
    strncpy(g_cameraStatus.sensor, cameraService.sensorName(),
            sizeof(g_cameraStatus.sensor) - 1);
}

void setupMotion() {
    HardwareConfig cfg = configManager.getConfig();
    if (cfg.gpio_in1 >= 0 && cfg.gpio_in2 >= 0 && cfg.gpio_ena >= 0) {
        if (l298nDriver.begin(cfg)) {
            motionEngine.begin();
            Serial.println(F("L298N motor initialized"));
        } else {
            Serial.println(F("L298N init failed - check GPIO config"));
        }
    } else {
        Serial.println(F("L298N GPIOs not configured - use web settings"));
    }
}

void setupTracking() {
    motionDetector.begin(WF_DETECT_W, WF_DETECT_H);
    g_trackingStatus.frameWidth = 320;
    g_trackingStatus.frameHeight = 240;
    Serial.println(F("Motion detector ready"));
}

void setupDashboard() {
    dashboardService.init();
    Serial.println(F("Dashboard: http://windfollow.local"));
}

// ============================================================
// Loop
// ============================================================
void loopCore() {
    delay(2);
}

void loopNetwork() {
    dashboardService.tick();
}

void loopCamera() {
    // A camera e capturada sob demanda em /camera/capture e no loop de rastreamento.
}

void loopMotion() {
    motionEngine.update();
    g_trackingStatus.motorMoving = l298nDriver.isMoving();
    g_trackingStatus.motorDirection = l298nDriver.directionName();
}

void loopTracking() {
    static unsigned long lastDetect = 0;
    static unsigned long lastMove = 0;
    static bool havePrev = false;
    static uint8_t grayFrame[WF_DETECT_W * WF_DETECT_H];
    static uint8_t prevFrame[WF_DETECT_W * WF_DETECT_H];

    if (!cameraService.isReady()) return;

    unsigned long now = millis();
    if ((now - lastDetect) < 150) return;
    lastDetect = now;

    int srcW = 0;
    int srcH = 0;
    if (!cameraService.grabGray(grayFrame, WF_DETECT_W, WF_DETECT_H, srcW, srcH)) {
        return;
    }

    g_cameraStatus.frameCount = cameraService.frameCount();
    g_cameraStatus.width = srcW;
    g_cameraStatus.height = srcH;
    g_trackingStatus.frameWidth = srcW;
    g_trackingStatus.frameHeight = srcH;

    HardwareConfig cfg = configManager.getConfig();
    MotionRegion region;
    if (havePrev) {
        region = motionDetector.detect(grayFrame, prevFrame, WF_DETECT_W, WF_DETECT_H,
                                       cfg.sensitivity, cfg.min_area);
    }
    memcpy(prevFrame, grayFrame, sizeof(grayFrame));
    havePrev = true;

    if (region.valid) {
        float scaleX = (float)srcW / (float)WF_DETECT_W;
        float scaleY = (float)srcH / (float)WF_DETECT_H;

        int cx = (int)(region.centerX * scaleX);
        int cy = (int)(region.centerY * scaleY);

        g_trackingStatus.motionDetected = true;
        g_trackingStatus.boxX = (int)(region.x * scaleX);
        g_trackingStatus.boxY = (int)(region.y * scaleY);
        g_trackingStatus.boxW = (int)(region.width * scaleX);
        g_trackingStatus.boxH = (int)(region.height * scaleY);
        g_trackingStatus.centerX = cx;
        g_trackingStatus.centerY = cy;
        g_trackingStatus.errorX = cx - (srcW / 2);
        g_trackingStatus.lastMotionMs = now;

        if (g_trackingStatus.errorX < -cfg.dead_zone) {
            g_trackingStatus.direction = "esquerda";
        } else if (g_trackingStatus.errorX > cfg.dead_zone) {
            g_trackingStatus.direction = "direita";
        } else {
            g_trackingStatus.direction = "centro";
        }

        bool outsideDeadZone = abs(g_trackingStatus.errorX) > cfg.dead_zone;
        if (cfg.auto_tracking && outsideDeadZone &&
            (now - lastMove) >= (unsigned long)cfg.stabilization_ms) {
            if (g_trackingStatus.errorX < 0) {
                motionEngine.esquerda(cfg.step_duration_ms, 200);
            } else {
                motionEngine.direita(cfg.step_duration_ms, 200);
            }
            lastMove = now;
        }
    } else {
        // Mantem o ultimo movimento por um curto periodo para evitar oscilacao na UI.
        if ((now - g_trackingStatus.lastMotionMs) > 800) {
            g_trackingStatus.motionDetected = false;
            g_trackingStatus.boxX = 0;
            g_trackingStatus.boxY = 0;
            g_trackingStatus.boxW = 0;
            g_trackingStatus.boxH = 0;
            g_trackingStatus.errorX = 0;
            g_trackingStatus.direction = "centro";
        }
    }

    g_trackingStatus.tracking = cfg.auto_tracking;
}

void loopDashboard() {
    // O tick do dashboard e chamado em loopNetwork().
}
