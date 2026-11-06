/*
 * WindFollowBlind — Controle de 2 motores DC via L298N (sem camera)
 * Hardware: ESP32 DevKit + L298N + 2 motores DC + 2 botoes NA
 * Documentacao: docs/documentacao_hardware_esp32_l298n.md
 * Framework: Arduino ESP32
 *
 * Cada botao NA controla seu proprio motor pela sequencia:
 *   esquerda -> parado -> direita -> parado -> (repete)
 * Detecta borda de pressionamento (nao avisa enquanto segurado).
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
#include "L298NDualDriver.h"
#include "MotorEngineBlind.h"
#include "DashboardServiceBlind.h"

// ============================================================
// Instancias globais
// ============================================================
ConfigManager          configManager;
L298NDualDriver        l298nDualDriver;
MotorEngineBlind       motorEngineBlind;
DashboardServiceBlind  dashboardServiceBlind;

SystemState g_systemState = SystemState::Boot;

// ============================================================
// Declaracoes
// ============================================================
void setupConfig();
void setupCore();
void setupNetwork();
void setupMotion();
void setupDashboard();

void loopCore();
void loopSerial();
void loopNetwork();
void loopMotion();
void loopButtons();
void loopDashboard();

// ============================================================
// setup()
// ============================================================
void setup() {
    Serial.begin(115200);
    delay(100);

    Serial.println();
    Serial.println(F("WindFollowBlind v1.0.0"));
    Serial.println(F("Platform: ESP32 / ESP32-DevKit"));
    Serial.println(F("Driver: L298N + 2 Motores DC + 2 Botoes NA"));

    // Botoes NA: INPUT_PULLUP (solto=HIGH, pressionado=LOW)
    HardwareConfig cfg = configManager.getConfig(); // le defaults se nao houver NVS ainda
    if (cfg.gpio_btn1 >= 0) pinMode(cfg.gpio_btn1, INPUT_PULLUP);
    if (cfg.gpio_btn2 >= 0) pinMode(cfg.gpio_btn2, INPUT_PULLUP);

    g_systemState = SystemState::Init;

    setupConfig();
    delay(100);

    setupCore();

    setupNetwork();
    setupMotion();
    setupDashboard();

    g_systemState = SystemState::Ready;
    Serial.println(F("WindFollowBlind ready - all systems initialized"));
}

// ============================================================
// loop()
// ============================================================
void loop() {
    loopCore();
    loopSerial();
    loopNetwork();
    loopMotion();
    loopButtons();
    loopDashboard();
}

// ============================================================
// Setup
// ============================================================
void setupConfig() {
    configManager.begin();
    HardwareConfig cfg = configManager.getConfig();
    Serial.printf("Config: M1(IN1=%d,IN2=%d,ENA=%d) M2(IN3=%d,IN4=%d,ENB=%d) "
                  "BT1=%d BT2=%d step=%d\n",
        cfg.gpio_in1, cfg.gpio_in2, cfg.gpio_ena,
        cfg.gpio_in3, cfg.gpio_in4, cfg.gpio_enb,
        cfg.gpio_btn1, cfg.gpio_btn2, cfg.step_duration_ms);
}

void setupCore() {
    // Estado do sistema inicializado em g_systemState
}

void setupNetwork() {
    Preferences prefs;
    prefs.begin("windflollowblind", true);
    String ssid = prefs.getString("wifi_ssid", "");
    String password = prefs.getString("wifi_password", "");
    prefs.end();

    WiFi.persistent(false);
    WiFi.mode(WIFI_AP_STA);
    delay(100);

    bool apOk = false;
    for (int attempt = 0; attempt < 3 && !apOk; attempt++) {
        apOk = WiFi.softAP("WindFollowBlind-Setup", "windfollow123");
        if (!apOk) {
            Serial.printf("Falha ao subir o AP (tentativa %d), reiniciando WiFi...\n", attempt + 1);
            WiFi.mode(WIFI_OFF);
            delay(250);
            WiFi.mode(WIFI_AP_STA);
            delay(250);
        }
    }
    WiFi.setSleep(false);

    Serial.printf("AP %s  SSID: WindFollowBlind-Setup  IP: %s  MAC: %s\n",
        apOk ? "OK" : "FALHOU",
        WiFi.softAPIP().toString().c_str(),
        WiFi.softAPmacAddress().c_str());

    if (ssid.length() > 0 && apOk) {
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

    if (MDNS.begin("windfollowblind")) {
        MDNS.addService("http", "tcp", 80);
        Serial.println(F("mDNS: http://windflollowblind.local"));
    } else {
        Serial.println(F("Falha ao iniciar mDNS"));
    }
}

void setupMotion() {
    HardwareConfig cfg = configManager.getConfig();
    if (l298nDualDriver.begin(cfg)) {
        motorEngineBlind.begin();
        motorEngineBlind.setStepDurationMs(cfg.step_duration_ms);
        Serial.println(F("L298N dual motor initialized"));
    } else {
        Serial.println(F("L298N init failed - check GPIO config"));
    }
}

void setupDashboard() {
    dashboardServiceBlind.init();
    Serial.println(F("Dashboard: http://windflollowblind.local"));
}

// ============================================================
// Loop
// ============================================================
void loopCore() {
    static unsigned long lastBeat = 0;
    unsigned long now = millis();

    if (now - lastBeat >= 5000) {
        lastBeat = now;
        Serial.printf("[hb] up=%lus sta=%d heap=%u psram=%u ap=%s mov1=%s mov2=%s\n",
            now / 1000,
            (int)WiFi.status(),
            (unsigned)ESP.getFreeHeap(),
            (unsigned)ESP.getFreePsram(),
            WiFi.softAPIP().toString().c_str(),
            motorEngineBlind.isMoving1() ? "sim" : "nao",
            motorEngineBlind.isMoving2() ? "sim" : "nao");
    }

    delay(2);
}

void loopNetwork() {
    dashboardServiceBlind.tick();
}

void loopSerial() {
    while (Serial.available() > 0) {
        int c = Serial.read();
        if (c == 'i' || c == 'I') {
            Serial.printf("[cmd] sta=%d heap=%u psram=%u ap=%s mov1=%s mov2=%s\n",
                (int)WiFi.status(),
                (unsigned)ESP.getFreeHeap(),
                (unsigned)ESP.getFreePsram(),
                WiFi.softAPIP().toString().c_str(),
                motorEngineBlind.isMoving1() ? "sim" : "nao",
                motorEngineBlind.isMoving2() ? "sim" : "nao");
        } else if (c == 'r' || c == 'R') {
            Serial.println(F("[cmd] reiniciando..."));
            delay(100);
            ESP.restart();
        } else if (c == '1') {
            Serial.println(F("[cmd] motor1 esquerda"));
            motorEngineBlind.motor1Esquerda(150, 200);
        } else if (c == '2') {
            Serial.println(F("[cmd] motor1 direita"));
            motorEngineBlind.motor1Direita(150, 200);
        } else if (c == '3') {
            Serial.println(F("[cmd] motor2 esquerda"));
            motorEngineBlind.motor2Esquerda(150, 200);
        } else if (c == '4') {
            Serial.println(F("[cmd] motor2 direita"));
            motorEngineBlind.motor2Direita(150, 200);
        } else if (c == '0') {
            Serial.println(F("[cmd] parar tudo"));
            motorEngineBlind.stop();
        }
    }
}

void loopMotion() {
    motorEngineBlind.update();
}

// Controle dos botoes NA com debounce e sequencia por motor
void loopButtons() {
    static int seq1 = 0;   // 0=parado, 1=esquerda, 2=parado, 3=direita, 4=parado
    static int seq2 = 0;
    static unsigned long lastPress1 = 0;
    static unsigned long lastPress2 = 0;
    static int lastBtn1 = HIGH;
    static int lastBtn2 = HIGH;

    HardwareConfig cfg = configManager.getConfig();
    if (!motorEngineBlind.isConfigured()) return;

    unsigned long now = millis();

    // Botao 1 -> Motor 1
    if (cfg.gpio_btn1 >= 0) {
        int btn1 = digitalRead(cfg.gpio_btn1);
        if (lastBtn1 == HIGH && btn1 == LOW && (now - lastPress1) > 50) {
            lastPress1 = now;
            seq1++;
            if (seq1 > 4) seq1 = 1;
            Serial.print(F("[btn1] toque "));
            Serial.println(seq1);
            switch (seq1) {
                case 1:
                    Serial.println(F("[btn1] motor1 esquerda"));
                    motorEngineBlind.motor1Esquerda(cfg.step_duration_ms, 200);
                    break;
                case 2:
                case 4:
                    Serial.println(F("[btn1] motor1 parado"));
                    motorEngineBlind.stopMotor1();
                    break;
                case 3:
                    Serial.println(F("[btn1] motor1 direita"));
                    motorEngineBlind.motor1Direita(cfg.step_duration_ms, 200);
                    break;
            }
        }
        lastBtn1 = btn1;
    }

    // Botao 2 -> Motor 2
    if (cfg.gpio_btn2 >= 0) {
        int btn2 = digitalRead(cfg.gpio_btn2);
        if (lastBtn2 == HIGH && btn2 == LOW && (now - lastPress2) > 50) {
            lastPress2 = now;
            seq2++;
            if (seq2 > 4) seq2 = 1;
            switch (seq2) {
                case 1:
                    Serial.println(F("[btn2] motor2 esquerda"));
                    motorEngineBlind.motor2Esquerda(cfg.step_duration_ms, 200);
                    break;
                case 2:
                case 4:
                    Serial.println(F("[btn2] motor2 parado"));
                    motorEngineBlind.stopMotor2();
                    break;
                case 3:
                    Serial.println(F("[btn2] motor2 direita"));
                    motorEngineBlind.motor2Direita(cfg.step_duration_ms, 200);
                    break;
            }
        }
        lastBtn2 = btn2;
    }
}

void loopDashboard() {
    // O tick do dashboard e chamado em loopNetwork().
}
