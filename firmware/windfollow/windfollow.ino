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

// ============================================================
// SDK / Core
// ============================================================
#include "HardwareConfig.h"
#include "ConfigManager.h"
#include "L298NDriver.h"
#include "MotionEngine.h"
#include "MotionDetector.h"
#include "DashboardService.h"

// ============================================================
// Instâncias globais
// ============================================================
ConfigManager   configManager;
L298NDriver     l298nDriver;
MotionEngine    motionEngine;
MotionDetector  motionDetector;
DashboardService dashboardService;

SystemState g_systemState = SystemState::Boot;

// ============================================================
// Declarações
// ============================================================
void setupLogger();
void setupStorage();
void setupConfig();
void setupCore();
void setupNetwork();
void setupCamera();
void setupMotion();
void setupTracking();
void setupDashboard();

void loopLogger();
void loopStorage();
void loopConfig();
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

    // Desativa WDT do timer group 1 (pode disparar durante init da câmera)
    TIMERG1.wdtwprotect.val = TIMG_WDT_WKEY_V;
    TIMERG1.wdtconfig0.val = 0;
    TIMERG1.wdtwprotect.val = 0;

    Serial.println();
    Serial.println(F("WindFollow v1.0.0"));
    Serial.println(F("Platform: ESP32-S3 / LilyGO T-SIMCAM"));
    Serial.println(F("Driver: L298N + Motorredutor DC"));

    pinMode(1, OUTPUT);
    digitalWrite(1, HIGH);

    g_systemState = SystemState::Init;

    // 1. Logger
    setupLogger();
    delay(100);

    // 2. Storage (Preferences/NVS)
    setupStorage();
    delay(100);

    // 3. Config (carrega pinos GPIO, tensão, zona morta, etc.)
    setupConfig();
    delay(100);

    // 4. Core (estado do sistema)
    setupCore();
    delay(100);

    // 5. Rede Wi-Fi (AP ou STA)
    setupNetwork();
    delay(100);

    // 6. Câmera (OV2640 / OV5640)
    setupCamera();
    delay(100);

    // 7. Motor L298N (usa pinos do ConfigManager)
    setupMotion();
    delay(100);

    // 8. Rastreamento por movimento
    setupTracking();
    delay(100);

    // 9. Dashboard Web (serve HTML/CSS/JS + rotas REST)
    setupDashboard();
    delay(100);

    // Conecta Wi-Fi ou entra em modo AP
    networkService.connectOrFallback();

    g_systemState = SystemState::Ready;
    loggerService.info("System", "WindFollow ready — all systems initialized");
}

// ============================================================
// loop()
// ============================================================
void loop() {
    loopLogger();
    loopStorage();
    loopConfig();
    loopCore();
    loopNetwork();
    loopCamera();
    loopMotion();
    loopTracking();
    loopDashboard();
}

// ============================================================
// Stub implementations (serão preenchidas à medida que cada módulo for implementado)
// ============================================================
void setupLogger()   { /* implementado em logger */ }
void setupStorage()  { /* implementado em storage */ }
void setupConfig()   {
    configManager.begin();
}
void setupCore()     { /* estado do sistema */ }
void setupNetwork()  { networkService.init(); }
void setupCamera()   {
    // TODO: inicializar câmera OV2640/OV5640
}
void setupMotion()   {
    HardwareConfig cfg = configManager.getConfig();
    if (cfg.gpio_in1 >= 0 && cfg.gpio_in2 >= 0 && cfg.gpio_ena >= 0) {
        if (l298nDriver.begin(cfg)) {
            motionEngine.begin();
            Serial.println(F("L298N motor initialized"));
        } else {
            Serial.println(F("L298N init failed — check GPIO config"));
        }
    } else {
        Serial.println(F("L298N GPIOs not configured — use web settings"));
    }
}
void setupTracking() {
    motionDetector.begin(320, 240); // resolução de detecção
    Serial.println(F("Motion detector ready"));
}
void setupDashboard() {
    dashboardService.init();
}

void loopLogger()   { /* stub */ }
void loopStorage()  { /* stub */ }
void loopConfig()   { /* stub */ }
void loopCore()     { delay(5); }
void loopNetwork()  { networkService.tick(); }
void loopCamera()   { /* stub */ }
void loopMotion()   { motionEngine.update(); }
void loopTracking() {
    // TODO: loop de detecção de movimento + acionamento do motor
}
void loopDashboard() { dashboardService.tick(); }
