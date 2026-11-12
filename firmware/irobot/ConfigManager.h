#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include "HardwareConfig.h"
#include <Preferences.h>

class ConfigManager {
public:
  ConfigManager();

  bool begin();
  RobotConfig load();
  bool save(const RobotConfig& config);
  void resetToDefaults();

private:
  Preferences _prefs;
  static const char* NVS_NAMESPACE;
  static const char* NVS_KEY;
};

#endif // CONFIG_MANAGER_H
