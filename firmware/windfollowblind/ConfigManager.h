#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include <Arduino.h>
#include <Preferences.h>
#include "HardwareConfig.h"

class ConfigManager {
public:
    bool begin();
    HardwareConfig getConfig() const;
    bool saveConfig(const HardwareConfig& config);
    bool resetConfig();
    bool validateGpio(int gpio) const;
    bool validateConfig(const HardwareConfig& config, String& error) const;

private:
    Preferences prefs;
    const char* namespaceName = "windflollowblind";
    HardwareConfig _config;

    bool loadDefaults();
    bool loadFromPrefs();
};

extern ConfigManager configManager;

#endif
