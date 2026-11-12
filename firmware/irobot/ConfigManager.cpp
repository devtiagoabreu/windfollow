#include "ConfigManager.h"

const char* ConfigManager::NVS_NAMESPACE = "irobot";
const char* ConfigManager::NVS_KEY       = "robot_cfg";

ConfigManager::ConfigManager() {}

bool ConfigManager::begin() {
  return _prefs.begin(NVS_NAMESPACE, false) == true;
}

RobotConfig ConfigManager::load() {
  RobotConfig cfg;
  if (!_prefs.begin(NVS_NAMESPACE, true)) {
    return cfg; // defaults
  }

  size_t len = _prefs.getBytesLength(NVS_KEY);
  if (len == sizeof(RobotConfig)) {
    _prefs.getBytes(NVS_KEY, &cfg, sizeof(RobotConfig));
  } else {
    // dados inválidos ou ausentes -> defaults
    cfg = RobotConfig();
  }
  _prefs.end();
  return cfg;
}

bool ConfigManager::save(const RobotConfig& config) {
  if (!_prefs.begin(NVS_NAMESPACE, false)) {
    return false;
  }
  bool ok = _prefs.putBytes(NVS_KEY, &config, sizeof(RobotConfig));
  _prefs.end();
  return ok;
}

void ConfigManager::resetToDefaults() {
  if (!_prefs.begin(NVS_NAMESPACE, false)) {
    return;
  }
  _prefs.remove(NVS_KEY);
  _prefs.end();
}
