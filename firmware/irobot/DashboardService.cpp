#include "DashboardService.h"
#include "WebAssets.h"
#include <ESPmDNS.h>
#include <WiFi.h>
#include <Preferences.h>

DashboardService::DashboardService() {}

bool DashboardService::begin(MotorEngine* engine, L298NDualDriver* driver, ConfigManager* configMgr,
                             SensorService* sensors, AutoNavigator* autoNav, EnvironmentAdvisor* envAdvisor) {
  _engine = engine;
  _driver = driver;
  _configMgr = configMgr;
  _sensors = sensors;
  _autoNav = autoNav;
  _envAdvisor = envAdvisor;

  _server = new WebServer(80);
  setupRoutes();
  _server->begin();

  return true;
}

void DashboardService::loop() {
  _server->handleClient();
}

void DashboardService::setupRoutes() {
  _server->on("/", HTTP_GET, std::bind(&DashboardService::handleRoot, this));
  _server->on("/teste", HTTP_GET, std::bind(&DashboardService::handleTeste, this));
  _server->on("/style.css", HTTP_GET, std::bind(&DashboardService::handleStyleCss, this));
  _server->on("/app.js", HTTP_GET, std::bind(&DashboardService::handleAppJs, this));
  _server->on("/favicon.ico", HTTP_GET, std::bind(&DashboardService::handleFavicon, this));
  _server->on("/manifest.json", HTTP_GET, std::bind(&DashboardService::handleManifest, this));
  _server->on("/favicon.svg", HTTP_GET, std::bind(&DashboardService::handleFaviconSvg, this));
  _server->on("/favicon.png", HTTP_GET, std::bind(&DashboardService::handleFaviconPng, this));
  _server->on("/icon-192.png", HTTP_GET, std::bind(&DashboardService::handleIcon192, this));
  _server->on("/icon-512.png", HTTP_GET, std::bind(&DashboardService::handleIcon512, this));
  _server->on("/sw.js", HTTP_GET, std::bind(&DashboardService::handleSwJs, this));
  _server->on("/api/motor/status", HTTP_GET, std::bind(&DashboardService::handleMotorStatus, this));
  _server->on("/api/motor/status", HTTP_OPTIONS, std::bind(&DashboardService::handleCorsPreflight, this));
  _server->on("/api/motor/move", HTTP_POST, std::bind(&DashboardService::handleMotorMove, this));
  _server->on("/api/motor/move", HTTP_OPTIONS, std::bind(&DashboardService::handleCorsPreflight, this));
  _server->on("/api/motor/stop", HTTP_POST, std::bind(&DashboardService::handleMotorStop, this));
  _server->on("/api/motor/stop", HTTP_OPTIONS, std::bind(&DashboardService::handleCorsPreflight, this));
  _server->on("/api/config", HTTP_GET, std::bind(&DashboardService::handleConfigGet, this));
  _server->on("/api/config", HTTP_POST, std::bind(&DashboardService::handleConfigPost, this));
  _server->on("/api/config", HTTP_OPTIONS, std::bind(&DashboardService::handleCorsPreflight, this));
  _server->on("/api/sensors", HTTP_GET, std::bind(&DashboardService::handleSensors, this));
  _server->on("/api/sensors", HTTP_OPTIONS, std::bind(&DashboardService::handleCorsPreflight, this));
  _server->on("/api/auto", HTTP_GET, std::bind(&DashboardService::handleAutoGet, this));
  _server->on("/api/auto", HTTP_POST, std::bind(&DashboardService::handleAutoPost, this));
  _server->on("/api/auto", HTTP_OPTIONS, std::bind(&DashboardService::handleCorsPreflight, this));
  _server->on("/api/system/info", HTTP_GET, std::bind(&DashboardService::handleSystemInfo, this));
  _server->on("/api/system/info", HTTP_OPTIONS, std::bind(&DashboardService::handleCorsPreflight, this));
  _server->on("/api/wifi", HTTP_GET, std::bind(&DashboardService::handleWifiGet, this));
  _server->on("/api/wifi", HTTP_POST, std::bind(&DashboardService::handleWifiPost, this));
  _server->on("/api/wifi", HTTP_OPTIONS, std::bind(&DashboardService::handleCorsPreflight, this));
  _server->on("/api/system/restart", HTTP_POST, std::bind(&DashboardService::handleSystemRestart, this));
  _server->on("/api/system/restart", HTTP_OPTIONS, std::bind(&DashboardService::handleCorsPreflight, this));
  _server->onNotFound(std::bind(&DashboardService::handleNotFound, this));
}

void DashboardService::handleCorsPreflight() {
  sendWithCors(204, "text/plain", "");
}

void DashboardService::sendWithCors(int code, const char* contentType, const String& body) {
  _server->sendHeader("Access-Control-Allow-Origin", "*");
  _server->sendHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
  _server->sendHeader("Access-Control-Allow-Headers", "Content-Type");
  _server->send(code, contentType, body);
}

void DashboardService::handleRoot() {
  Serial.println(">>> GET /");
  sendWithCors(200, "text/html", index_html);
}

void DashboardService::handleTeste() {
  Serial.println(">>> GET /teste");
  sendWithCors(200, "text/html", teste_html);
}

void DashboardService::handleStyleCss() {
  sendWithCors(200, "text/css", style_css);
}

void DashboardService::handleAppJs() {
  sendWithCors(200, "application/javascript", app_js);
}

void DashboardService::handleFavicon() {
  _server->sendHeader("Access-Control-Allow-Origin", "*");
  _server->send_P(200, "image/png", reinterpret_cast<const char*>(favicon_png), favicon_png_len);
}

void DashboardService::handleManifest() {
  sendWithCors(200, "application/manifest+json", manifest_json);
}

void DashboardService::handleFaviconSvg() {
  sendWithCors(200, "image/svg+xml", favicon_svg);
}

void DashboardService::handleFaviconPng() {
  _server->sendHeader("Access-Control-Allow-Origin", "*");
  _server->send_P(200, "image/png", reinterpret_cast<const char*>(favicon_png), favicon_png_len);
}

void DashboardService::handleIcon192() {
  _server->sendHeader("Access-Control-Allow-Origin", "*");
  _server->send_P(200, "image/png", reinterpret_cast<const char*>(icon_192_png), icon_192_png_len);
}

void DashboardService::handleIcon512() {
  _server->sendHeader("Access-Control-Allow-Origin", "*");
  _server->send_P(200, "image/png", reinterpret_cast<const char*>(icon_512_png), icon_512_png_len);
}

void DashboardService::handleSwJs() {
  _server->sendHeader("Cache-Control", "no-cache");
  sendWithCors(200, "application/javascript", sw_js);
}

void DashboardService::handleWifiGet() {
  Preferences prefs;
  prefs.begin("irobot-wifi", true);
  String mode = prefs.getString("mode", "ap");
  String ssid = prefs.getString("ssid", "");
  String pass = prefs.getString("pass", "");
  prefs.end();

  DynamicJsonDocument doc(256);
  doc["mode"] = mode;
  doc["ssid"] = ssid;
  doc["configured"] = ssid.length() > 0;
  String json;
  serializeJson(doc, json);
  sendWithCors(200, "application/json", json);
}

void DashboardService::handleWifiPost() {
  String body = _server->arg("plain");
  DynamicJsonDocument doc(256);
  DeserializationError err = deserializeJson(doc, body);
  if (err) {
    sendWithCors(400, "application/json", "{\"error\":\"json invalido\"}");
    return;
  }

  String mode = doc["mode"] | "ap";
  String ssid = doc["ssid"] | "";
  String pass = doc["pass"] | "";
  if (mode != "ap" && mode != "sta") {
    sendWithCors(400, "application/json", "{\"error\":\"modo invalido\"}");
    return;
  }

  Preferences prefs;
  prefs.begin("irobot-wifi", false);
  prefs.putString("mode", mode);
  prefs.putString("ssid", ssid);
  prefs.putString("pass", pass);
  prefs.end();

  sendWithCors(200, "application/json", "{\"ok\":true,\"restart\":true}");
}

void DashboardService::handleSystemRestart() {
  sendWithCors(200, "application/json", "{\"ok\":true}");
  delay(200);
  ESP.restart();
}

void DashboardService::handleMotorStatus() {
  Serial.println(">>> /api/motor/status");
  String json = generateStatusJson();
  sendWithCors(200, "application/json", json);
}

void DashboardService::handleMotorMove() {
  Serial.println(">>> /api/motor/move");
  if (!_engine) {
    sendWithCors(400, "application/json", "{\"error\":\"engine invalido\"}");
    return;
  }

  // O WebServer ja bufferiza o corpo do POST em arg("plain"); ler do socket
  // aqui espera o timeout e devolve vazio, quebrando a tela.
  String body = _server->arg("plain");
  Serial.println("Body: " + body);
  DynamicJsonDocument doc(256);
  DeserializationError err = deserializeJson(doc, body);
  if (err) {
    sendWithCors(400, "application/json", "{\"error\":\"json invalido\"}");
    return;
  }

  bool ok = handleMoveRequest(doc);
  if (ok) {
    sendWithCors(200, "application/json", "{\"ok\":true}");
  } else {
    sendWithCors(400, "application/json", "{\"error\":\"comando invalido\"}");
  }
}

void DashboardService::handleMotorStop() {
  Serial.println(">>> /api/motor/stop");
  if (_engine) _engine->stop();
  sendWithCors(200, "application/json", "{\"ok\":true}");
}

void DashboardService::handleConfigGet() {
  Serial.println(">>> /api/config GET");
  String json = generateConfigJson();
  sendWithCors(200, "application/json", json);
}

void DashboardService::handleConfigPost() {
  Serial.println(">>> /api/config POST");
  String body = _server->arg("plain");
  Serial.println("Body: " + body);
  DynamicJsonDocument doc(512);
  DeserializationError err = deserializeJson(doc, body);
  if (err) {
    sendWithCors(400, "application/json", "{\"error\":\"json invalido\"}");
    return;
  }

  RobotConfig cfg = _configMgr->load();

  if (doc.containsKey("pwmMin"))   cfg.pwmMin   = doc["pwmMin"].as<uint8_t>();
  if (doc.containsKey("pwmMax"))   cfg.pwmMax   = doc["pwmMax"].as<uint8_t>();
  if (doc.containsKey("pwmDefault")) cfg.pwmDefault = doc["pwmDefault"].as<uint8_t>();
  if (doc.containsKey("pulseDurationMs")) cfg.pulseDurationMs = doc["pulseDurationMs"].as<unsigned long>();
  if (doc.containsKey("motorsEnabled")) cfg.motorsEnabled = doc["motorsEnabled"].as<bool>();
  if (doc.containsKey("autoSpeed")) cfg.autoSpeed = doc["autoSpeed"].as<uint8_t>();

  if (_configMgr->save(cfg)) {
    sendWithCors(200, "application/json", "{\"ok\":true}");
  } else {
    sendWithCors(500, "application/json", "{\"error\":\"falha ao salvar\"}");
  }
}

void DashboardService::handleSensors() {
  Serial.println(">>> /api/sensors");
  DynamicJsonDocument doc(768);
  EnvironmentReading r = _sensors ? _sensors->reading() : EnvironmentReading();
  doc["aht"] = r.ahtOk;
  doc["bmp"] = r.bmpOk;
  doc["tof"] = r.tofOk;
  if (!isnan(r.temperatureC)) doc["temperature"] = r.temperatureC;
  if (!isnan(r.humidityPct))  doc["humidity"] = r.humidityPct;
  if (!isnan(r.pressureHpa))  doc["pressure"] = r.pressureHpa;
  if (!isnan(r.bmpTemperatureC)) doc["bmpTemperature"] = r.bmpTemperatureC;
  doc["distanceMm"] = r.distanceMm;
  doc["auto"] = _autoNav ? _autoNav->enabled() : false;
  doc["autoState"] = _autoNav ? _autoNav->stateName() : "off";
  doc["stuck"] = _autoNav ? _autoNav->stuck() : false;
  doc["stuckSeq"] = _autoNav ? _autoNav->stuckSeq() : 0;
  doc["avoidCount"] = _autoNav ? _autoNav->avoidCount() : 0;
  doc["autoSpeed"] = _configMgr ? _configMgr->load().autoSpeed : AUTO_FORWARD_SPEED;
  doc["uptime"] = millis();

  JsonObject alerts = doc.createNestedObject("alerts");
  EnvAlerts a = _envAdvisor ? _envAdvisor->alerts() : EnvAlerts();
  alerts["rain"] = a.rainLikely;
  alerts["rainSensor"] = a.rainSensor;
  alerts["hot"] = a.hot;
  alerts["cold"] = a.cold;
  alerts["dry"] = a.dry;
  alerts["fire"] = a.fire;
  alerts["trendReady"] = a.trendReady;
  alerts["trend3h"] = a.trend3hHpa;
  alerts["trend30m"] = a.trend30mHpa;
  alerts["tempRise"] = a.tempRiseC;
  String json;
  serializeJson(doc, json);
  sendWithCors(200, "application/json", json);
}

void DashboardService::handleAutoGet() {
  Serial.println(">>> /api/auto GET");
  DynamicJsonDocument doc(192);
  doc["enabled"] = _autoNav ? _autoNav->enabled() : false;
  doc["state"] = _autoNav ? _autoNav->stateName() : "off";
  doc["tof"] = _sensors ? _sensors->tofPresent() : false;
  doc["stuck"] = _autoNav ? _autoNav->stuck() : false;
  doc["stuckSeq"] = _autoNav ? _autoNav->stuckSeq() : 0;
  doc["avoidCount"] = _autoNav ? _autoNav->avoidCount() : 0;
  doc["autoSpeed"] = _configMgr ? _configMgr->load().autoSpeed : AUTO_FORWARD_SPEED;
  String json;
  serializeJson(doc, json);
  sendWithCors(200, "application/json", json);
}

void DashboardService::handleAutoPost() {
  Serial.println(">>> /api/auto POST");
  if (!_autoNav) {
    sendWithCors(400, "application/json", "{\"error\":\"autonomo indisponivel\"}");
    return;
  }

  String body = _server->arg("plain");
  DynamicJsonDocument doc(128);
  DeserializationError err = deserializeJson(doc, body);
  if (err) {
    sendWithCors(400, "application/json", "{\"error\":\"json invalido\"}");
    return;
  }

  if (doc.containsKey("autoSpeed") && _configMgr) {
    RobotConfig cfg = _configMgr->load();
    cfg.autoSpeed = doc["autoSpeed"].as<uint8_t>();
    _configMgr->save(cfg);
  }

  bool on = _autoNav->enabled();
  if (doc.containsKey("enabled")) {
    on = doc["enabled"] | false;
    if (!_autoNav->setEnabled(on)) {
      sendWithCors(409, "application/json", "{\"error\":\"VL53L0X ausente\"}");
      return;
    }
  }

  DynamicJsonDocument out(192);
  out["ok"] = true;
  out["enabled"] = _autoNav->enabled();
  out["state"] = _autoNav->stateName();
  out["stuck"] = _autoNav->stuck();
  out["stuckSeq"] = _autoNav->stuckSeq();
  out["autoSpeed"] = _configMgr ? _configMgr->load().autoSpeed : AUTO_FORWARD_SPEED;
  String json;
  serializeJson(out, json);
  sendWithCors(200, "application/json", json);
}

void DashboardService::handleSystemInfo() {
  Serial.println(">>> /api/system/info");
  DynamicJsonDocument doc(256);
  doc["uptime"] = millis();
  doc["heap"] = ESP.getFreeHeap();
  doc["psram"] = ESP.getFreePsram();
  String json;
  serializeJson(doc, json);
  sendWithCors(200, "application/json", json);
}

void DashboardService::handleNotFound() {
  Serial.print(">>> 404 ");
  Serial.print(_server->method() == HTTP_POST ? "POST " : "GET ");
  Serial.println(_server->uri());
  sendWithCors(404, "text/plain", "Nao encontrado");
}

String DashboardService::generateStatusJson() {
  DynamicJsonDocument doc(256);
  doc["moving"] = _engine->isMoving();
  doc["direction"] = static_cast<int>(_engine->getCurrentDirection());
  doc["leftSpeed"] = _engine->getLeftSpeed();
  doc["rightSpeed"] = _engine->getRightSpeed();
  String json;
  serializeJson(doc, json);
  return json;
}

String DashboardService::generateConfigJson() {
  RobotConfig cfg = _configMgr->load();
  DynamicJsonDocument doc(512);
  doc["pwmMin"] = cfg.pwmMin;
  doc["pwmMax"] = cfg.pwmMax;
  doc["pwmDefault"] = cfg.pwmDefault;
  doc["autoSpeed"] = cfg.autoSpeed;
  doc["pulseDurationMs"] = cfg.pulseDurationMs;
  doc["motorsEnabled"] = cfg.motorsEnabled;
  doc["gpioLeftIn1"] = cfg.gpioLeftIn1;
  doc["gpioLeftIn2"] = cfg.gpioLeftIn2;
  doc["gpioLeftEna"] = cfg.gpioLeftEna;
  doc["gpioRightIn3"] = cfg.gpioRightIn3;
  doc["gpioRightIn4"] = cfg.gpioRightIn4;
  doc["gpioRightEnb"] = cfg.gpioRightEnb;
  String json;
  serializeJson(doc, json);
  return json;
}

bool DashboardService::handleMoveRequest(JsonDocument& doc) {
  if (!_engine) return false;

  const char* dirStr = doc["direction"] | "stop";
  uint8_t speed = doc["speed"] | _configMgr->load().pwmDefault;

  RobotDirection dir;
  if (strcmp(dirStr, "frente") == 0) dir = RobotDirection::FRENTE;
  else if (strcmp(dirStr, "tras") == 0) dir = RobotDirection::TRAS;
  else if (strcmp(dirStr, "esquerda") == 0) dir = RobotDirection::ESQUERDA;
  else if (strcmp(dirStr, "direita") == 0) dir = RobotDirection::DIREITA;
  else if (strcmp(dirStr, "stop") == 0) dir = RobotDirection::STOP;
  else return false;

  // Controle diferencial por velocidade individual (opcional)
  if (doc.containsKey("leftSpeed") && doc.containsKey("rightSpeed")) {
    int8_t left  = doc["leftSpeed"].as<int8_t>();
    int8_t right = doc["rightSpeed"].as<int8_t>();
    _engine->moveDifferential(left, right);
    return true;
  }

  _engine->move(dir, speed);
  return true;
}
