#include <WiFi.h>
#include <ESPmDNS.h>

#include "HardwareConfig.h"
#include "L298NDualDriver.h"
#include "MotorEngine.h"
#include "ConfigManager.h"
#include "DashboardService.h"
#include "WebAssets.h"

// --- Access Point ---
static const char* AP_SSID   = "Irobot-Setup";
static const char* AP_PASS   = "irobot1234";

// --- Instâncias globais ---
ConfigManager   configMgr;
L298NDualDriver motorDriver;
MotorEngine     motorEngine;
DashboardService dashboard;

// --- Configuração global ---
RobotConfig cfg;

// --- Variáveis de heartbeat ---
unsigned long _lastHeartbeat = 0;

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("=== iRobot Firmware ===");
  Serial.println("Controle diferencial de 2 motores com PWM");
  Serial.println("GPIO: M1(26/25/ENA=27)  M2(32/33/ENB=14)");
  Serial.println();

  // Carrega configuração NVS
  if (configMgr.begin()) {
    cfg = configMgr.load();
    Serial.println("Configuracao carregada do NVS");
  } else {
    Serial.println("Falha ao iniciar NVS, usando defaults");
  }

  // Inicializa driver L298N com PWM
  cfg = configMgr.load();
  if (!motorDriver.begin(cfg)) {
    Serial.println("Falha ao inicializar L298N!");
  } else {
    Serial.println("L298N inicializado com PWM");
  }

  // Inicializa motor engine
  if (!motorEngine.begin(&motorDriver, cfg)) {
    Serial.println("Falha ao inicializar MotorEngine!");
  } else {
    Serial.println("MotorEngine inicializado");
  }

  // Wi-Fi Access Point
  WiFi.mode(WIFI_AP);
  WiFi.softAP(AP_SSID, AP_PASS);
  IPAddress apIP = WiFi.softAPIP();
  Serial.print("AP SSID: "); Serial.println(AP_SSID);
  Serial.print("AP IP: "); Serial.println(apIP);

  // mDNS
  if (!MDNS.begin("irobot")) {
    Serial.println("Falha ao iniciar mDNS");
  } else {
    MDNS.addService("http", "tcp", 80);
    Serial.println("mDNS: http://irobot.local");
  }

  // Servidor web + API
  if (!dashboard.begin(&motorEngine, &motorDriver, &configMgr)) {
    Serial.println("Falha ao iniciar DashboardService!");
  } else {
    Serial.println("DashboardService ativo na porta 80");
  }

  Serial.println();
  Serial.println("Comandos seriais:");
  Serial.println("  f  = frente");
  Serial.println("  t  = tras");
  Serial.println("  e  = esquerda");
  Serial.println("  d  = direita");
  Serial.println("  s  = stop");
  Serial.println("  1-9 = velocidade (10%..90%)");
  Serial.println("  i  = status");
  Serial.println("  r  = reiniciar");
  Serial.println();
}

void loop() {
  dashboard.loop();

  // Comandos seriais
  if (Serial.available()) {
    char cmd = Serial.read();
    switch (cmd) {
      case 'f':
        motorEngine.move(RobotDirection::FRENTE, cfg.pwmDefault);
        Serial.println("FRENTE");
        break;
      case 't':
        motorEngine.move(RobotDirection::TRAS, cfg.pwmDefault);
        Serial.println("TRAS");
        break;
      case 'e':
        motorEngine.move(RobotDirection::ESQUERDA, cfg.pwmDefault);
        Serial.println("ESQUERDA");
        break;
      case 'd':
        motorEngine.move(RobotDirection::DIREITA, cfg.pwmDefault);
        Serial.println("DIREITA");
        break;
      case 's':
        motorEngine.stop();
        Serial.println("STOP");
        break;
      case '1': case '2': case '3': case '4': case '5':
      case '6': case '7': case '8': case '9': {
        uint8_t speed = (cmd - '0') * 255 / 9;
        motorEngine.move(RobotDirection::FRENTE, speed);
        Serial.print("Velocidade: "); Serial.println(speed);
        break;
      }
      case 'i': {
        Serial.print("Direcao: "); Serial.println(static_cast<int>(motorEngine.getCurrentDirection()));
        Serial.print("Esquerda: "); Serial.println(motorEngine.getLeftSpeed());
        Serial.print("Direita: "); Serial.println(motorEngine.getRightSpeed());
        Serial.print("Movendo: "); Serial.println(motorEngine.isMoving() ? "sim" : "nao");
        break;
      }
      case 'r':
        Serial.println("Reiniciando...");
        delay(100);
        ESP.restart();
        break;
    }
  }

  // Heartbeat a cada 2 segundos
  unsigned long now = millis();
  if (now - _lastHeartbeat > 2000) {
    _lastHeartbeat = now;
    Serial.print(".");
  }
}
