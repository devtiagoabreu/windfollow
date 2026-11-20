#include <WiFi.h>
#include <esp_system.h>
#include <ESPmDNS.h>
#include <Preferences.h>

#include "HardwareConfig.h"
#include "L298NDualDriver.h"
#include "MotorEngine.h"
#include "ConfigManager.h"
#include "DashboardService.h"
#include "SensorService.h"
#include "AutoNavigator.h"
#include "EnvironmentAdvisor.h"
#include "WebAssets.h"

// --- Access Point ---
static const char* AP_SSID   = "Irobot-Setup";
static const char* AP_PASS   = "irobot1234";

// --- Instâncias globais ---
ConfigManager   configMgr;
L298NDualDriver motorDriver;
MotorEngine     motorEngine;
SensorService   sensorService;
AutoNavigator   autoNav;
EnvironmentAdvisor envAdvisor;
DashboardService dashboard;

// --- Configuração global ---
RobotConfig cfg;

// --- Variáveis de heartbeat ---
unsigned long _lastHeartbeat = 0;
unsigned long _lastHealth    = 0;

void setup() {
  // Segura os enables antes de tudo: com ENA/ENB flutuando no boot os motores
  // podem girar e derrubar a alimentacao (brownout) antes do AP subir.
  pinMode(27, OUTPUT);
  digitalWrite(27, LOW);
  pinMode(14, OUTPUT);
  digitalWrite(14, LOW);

  Serial.begin(115200);
  Serial.println();
  esp_reset_reason_t rr = esp_reset_reason();
  Serial.print("Reset anterior: ");
  Serial.print(static_cast<int>(rr));
  switch (rr) {
    case ESP_RST_POWERON:  Serial.println(" (power-on)"); break;
    case ESP_RST_EXT:      Serial.println(" (reset externo)"); break;
    case ESP_RST_SW:       Serial.println(" (software)"); break;
    case ESP_RST_PANIC:    Serial.println(" (panic/crash)"); break;
    case ESP_RST_INT_WDT:  Serial.println(" (watchdog de interrupcao)"); break;
    case ESP_RST_TASK_WDT: Serial.println(" (watchdog de task)"); break;
    case ESP_RST_WDT:      Serial.println(" (watchdog)"); break;
    case ESP_RST_BROWNOUT: Serial.println(" (BROWNOUT - queda de tensao)"); break;
    case ESP_RST_DEEPSLEEP:Serial.println(" (deep sleep)"); break;
    default:               Serial.println(" (desconhecido)"); break;
  }
  Serial.println("=== iRobot Firmware ===");
  Serial.println("Controle diferencial de 2 motores com PWM");
  Serial.println("GPIO: M1(26/25/ENA=27)  M2(32/33/ENB=14)");
  if (rr == ESP_RST_BROWNOUT) {
    Serial.println("AVISO: reset por brownout. Confira 5V/VIN, capacitor no VIN");
    Serial.println("e pull-down de 10k em GPIO27/GPIO14 (ENA/ENB).");
  }
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

  // Wi-Fi: AP por padrao; modo cliente (sta) se salvo na configuracao
  WiFi.setSleep(false);
  WiFi.setTxPower(WIFI_POWER_11dBm);

  Preferences wifiPrefs;
  wifiPrefs.begin("irobot-wifi", true);
  String wifiMode = wifiPrefs.getString("mode", "ap");
  String wifiSsid = wifiPrefs.getString("ssid", "");
  String wifiPass = wifiPrefs.getString("pass", "");
  wifiPrefs.end();

  bool staOk = false;
  if (wifiMode == "sta" && wifiSsid.length() > 0) {
    WiFi.mode(WIFI_STA);
    WiFi.begin(wifiSsid.c_str(), wifiPass.c_str());
    Serial.print("Wi-Fi STA conectando: "); Serial.println(wifiSsid);
    uint32_t t0 = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - t0 < 12000) {
      delay(250);
      Serial.print(".");
    }
    Serial.println();
    staOk = WiFi.status() == WL_CONNECTED;
    if (staOk) {
      Serial.print("Wi-Fi conectado. IP: "); Serial.println(WiFi.localIP());
    } else {
      Serial.println("Falha ao conectar no Wi-Fi - voltando para AP");
    }
  }

  if (!staOk) {
    WiFi.mode(WIFI_AP);
    WiFi.softAP(AP_SSID, AP_PASS);
    IPAddress apIP = WiFi.softAPIP();
    Serial.print("AP SSID: "); Serial.println(AP_SSID);
    Serial.print("AP IP: "); Serial.println(apIP);
  }

  // mDNS
  if (!MDNS.begin("irobot")) {
    Serial.println("Falha ao iniciar mDNS");
  } else {
    MDNS.addService("http", "tcp", 80);
    Serial.println("mDNS: http://irobot.local");
  }

  // Sensores de ambiente (AHT20 + BMP280 + VL53L0X) e navegacao autonoma
  if (!sensorService.begin()) {
    Serial.println("Nenhum sensor I2C encontrado");
  } else {
    Serial.println("Sensores I2C inicializados");
  }
  if (!autoNav.begin(&motorEngine, &sensorService, &configMgr)) {
    Serial.println("Falha ao iniciar AutoNavigator!");
  }

  // Servidor web + API
  envAdvisor.begin(&sensorService);

  if (!dashboard.begin(&motorEngine, &motorDriver, &configMgr, &sensorService, &autoNav, &envAdvisor)) {
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
  Serial.println("  a  = autonomo ON/OFF");
  Serial.println("  m  = ler ambiente (temp/umid/pressao/distancia)");
  Serial.println();
}

void loop() {
  dashboard.loop();
  sensorService.update();
  autoNav.loop();
  envAdvisor.update();

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
      case 'a': {
        bool on = !autoNav.enabled();
        if (autoNav.setEnabled(on)) {
          Serial.print("Autonomo: "); Serial.println(on ? "ON" : "OFF");
        } else {
          Serial.println("Autonomo indisponivel (VL53L0X ausente)");
        }
        break;
      }
      case 'm': {
        EnvironmentReading r = sensorService.reading();
        Serial.print("Temp: ");
        if (isnan(r.temperatureC)) Serial.println("--"); else Serial.println(r.temperatureC, 2);
        Serial.print("Umidade: ");
        if (isnan(r.humidityPct)) Serial.println("--"); else Serial.println(r.humidityPct, 2);
        Serial.print("Pressao: ");
        if (isnan(r.pressureHpa)) Serial.println("--"); else Serial.println(r.pressureHpa, 2);
        Serial.print("Distancia (mm): "); Serial.println(r.distanceMm);
        Serial.print("Distancia bruta (mm): "); Serial.println(sensorService.lastRawMm());
        Serial.print("Autonomo: "); Serial.println(autoNav.stateName());
        EnvAlerts al = envAdvisor.alerts();
        Serial.print("Avisos:");
        bool none = true;
        if (al.fire) { Serial.print(" INCENDIO"); none = false; }
        if (al.rainLikely) { Serial.print(al.rainSensor ? " CHUVA(agora)" : " CHUVA"); none = false; }
        if (al.hot) { Serial.print(" CALOR"); none = false; }
        if (al.cold) { Serial.print(" FRIO"); none = false; }
        if (al.dry) { Serial.print(" AR-SECO"); none = false; }
        if (none) Serial.print(" nenhum");
        Serial.print(" (dP30m="); Serial.print(al.trend30mHpa, 2);
        Serial.print(" dP3h="); Serial.print(al.trend3hHpa, 2);
        Serial.print(" dT1min="); Serial.print(al.tempRiseC, 2);
        Serial.println(")");
        break;
      }
    }
  }

  // Heartbeat a cada 2 segundos
  unsigned long now = millis();
  if (now - _lastHeartbeat > 2000) {
    _lastHeartbeat = now;
    Serial.print(".");
    if (now - _lastHealth > 10000) {
      _lastHealth = now;
      Serial.println();
      Serial.print("[saude] heap="); Serial.print(ESP.getFreeHeap());
      Serial.print(" minHeap="); Serial.print(ESP.getMinFreeHeap());
      Serial.print(" clientesWiFi="); Serial.print(WiFi.softAPgetStationNum());
      Serial.print(" auto="); Serial.print(autoNav.stateName());
      Serial.print(" dist="); Serial.println(sensorService.distanceMm());
    }
  }
}
