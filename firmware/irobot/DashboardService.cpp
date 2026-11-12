#include "DashboardService.h"
#include "WebAssets.h"
#include <ESPmDNS.h>
#include <WiFi.h>

DashboardService::DashboardService() {}

bool DashboardService::begin(MotorEngine* engine, L298NDualDriver* driver, ConfigManager* configMgr) {
  _engine = engine;
  _driver = driver;
  _configMgr = configMgr;

  _server = new WebServer(80);
  setupRoutes();
  _server->begin();

  // mDNS
  if (!MDNS.begin("irobot")) {
    Serial.println("Erro ao iniciar mDNS");
  } else {
    MDNS.addService("http", "tcp", 80);
    Serial.println("mDNS ativo: http://irobot.local");
  }

  return true;
}

void DashboardService::loop() {
  _server->handleClient();
}

void DashboardService::setupRoutes() {
  _server->on("/", HTTP_GET, std::bind(&DashboardService::handleRoot, this));
  _server->on("/style.css", HTTP_GET, std::bind(&DashboardService::handleStyleCss, this));
  _server->on("/app.js", HTTP_GET, std::bind(&DashboardService::handleAppJs, this));
  _server->on("/favicon.ico", HTTP_GET, std::bind(&DashboardService::handleFavicon, this));
  _server->on("/api/motor/status", HTTP_GET, std::bind(&DashboardService::handleMotorStatus, this));
  _server->on("/api/motor/status", HTTP_OPTIONS, std::bind(&DashboardService::handleCorsPreflight, this));
  _server->on("/api/motor/move", HTTP_POST, std::bind(&DashboardService::handleMotorMove, this));
  _server->on("/api/motor/move", HTTP_OPTIONS, std::bind(&DashboardService::handleCorsPreflight, this));
  _server->on("/api/motor/stop", HTTP_POST, std::bind(&DashboardService::handleMotorStop, this));
  _server->on("/api/motor/stop", HTTP_OPTIONS, std::bind(&DashboardService::handleCorsPreflight, this));
  _server->on("/api/config", HTTP_GET, std::bind(&DashboardService::handleConfigGet, this));
  _server->on("/api/config", HTTP_POST, std::bind(&DashboardService::handleConfigPost, this));
  _server->on("/api/config", HTTP_OPTIONS, std::bind(&DashboardService::handleCorsPreflight, this));
  _server->on("/api/system/info", HTTP_GET, std::bind(&DashboardService::handleSystemInfo, this));
  _server->on("/api/system/info", HTTP_OPTIONS, std::bind(&DashboardService::handleCorsPreflight, this));
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
  sendWithCors(200, "text/html", index_html);
}

void DashboardService::handleStyleCss() {
  sendWithCors(200, "text/css", style_css);
}

void DashboardService::handleAppJs() {
  sendWithCors(200, "application/javascript", app_js);
}

void DashboardService::handleFavicon() {
  sendWithCors(204, "image/x-icon", "");
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

  if (_configMgr->save(cfg)) {
    sendWithCors(200, "application/json", "{\"ok\":true}");
  } else {
    sendWithCors(500, "application/json", "{\"error\":\"falha ao salvar\"}");
  }
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
