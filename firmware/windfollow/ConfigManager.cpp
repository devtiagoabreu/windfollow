#include "ConfigManager.h"

bool ConfigManager::begin() {
    if (!prefs.begin(namespaceName, false)) {
        return false;
    }
    loadFromPrefs();

    // Migracao (uma unica vez): placas que nunca foram configuradas (GPIOs -1)
    // passam a usar a ligacao padrao do projeto, com o modem 4G desligado:
    // IN1=45 IN2=46 ENA=21.
    if (!prefs.getBool("cfg_migrated_v2", false)) {
        if (_config.gpio_in1 < 0 && _config.gpio_in2 < 0 && _config.gpio_ena < 0) {
            HardwareConfig defaults;
            _config.gpio_in1 = defaults.gpio_in1;
            _config.gpio_in2 = defaults.gpio_in2;
            _config.gpio_ena = defaults.gpio_ena;
            _config.motor_enabled = defaults.motor_enabled;
            if (_config.motor_voltage <= 0.0f) {
                _config.motor_voltage = defaults.motor_voltage;
            }
            prefs.putInt("gpio_in1", _config.gpio_in1);
            prefs.putInt("gpio_in2", _config.gpio_in2);
            prefs.putInt("gpio_ena", _config.gpio_ena);
            prefs.putBool("motor_enabled", _config.motor_enabled);
            prefs.putFloat("motor_voltage", _config.motor_voltage);
            Serial.println(F("[config] padrao aplicado: IN1=45 IN2=46 ENA=21 V=12.0"));
        }
        prefs.putBool("cfg_migrated_v2", true);
    }

    prefs.end();
    return true;
}

bool ConfigManager::loadFromPrefs() {
    _config.gpio_in1 = prefs.getInt("gpio_in1", -1);
    _config.gpio_in2 = prefs.getInt("gpio_in2", -1);
    _config.gpio_ena = prefs.getInt("gpio_ena", -1);
    _config.motor_voltage = prefs.getFloat("motor_voltage", 0.0f);
    _config.motor_enabled = prefs.getBool("motor_enabled", false);
    _config.dead_zone = prefs.getInt("dead_zone", 30);
    _config.sensitivity = prefs.getInt("sensitivity", 30);
    _config.min_area = prefs.getInt("min_area", 100);
    _config.step_duration_ms = prefs.getInt("step_duration_ms", 150);
    _config.stabilization_ms = prefs.getInt("stabilization_ms", 300);
    _config.auto_tracking = prefs.getBool("auto_tracking", false);
    return true;
}

bool ConfigManager::loadDefaults() {
    _config = HardwareConfig();
    return saveConfig(_config);
}

HardwareConfig ConfigManager::getConfig() const {
    return _config;
}

bool ConfigManager::saveConfig(const HardwareConfig& config) {
    if (!prefs.begin(namespaceName, false)) {
        return false;
    }
    prefs.putInt("gpio_in1", config.gpio_in1);
    prefs.putInt("gpio_in2", config.gpio_in2);
    prefs.putInt("gpio_ena", config.gpio_ena);
    prefs.putFloat("motor_voltage", config.motor_voltage);
    prefs.putBool("motor_enabled", config.motor_enabled);
    prefs.putInt("dead_zone", config.dead_zone);
    prefs.putInt("sensitivity", config.sensitivity);
    prefs.putInt("min_area", config.min_area);
    prefs.putInt("step_duration_ms", config.step_duration_ms);
    prefs.putInt("stabilization_ms", config.stabilization_ms);
    prefs.putBool("auto_tracking", config.auto_tracking);
    _config = config;
    prefs.end();
    return true;
}

bool ConfigManager::resetConfig() {
    HardwareConfig defaults;
    return saveConfig(defaults);
}

bool ConfigManager::validateGpio(int gpio) const {
    if (gpio < 0 || gpio > 48) return false;
    // Bloqueados: power/button/mic, camera+IR (0-20), flash/PSRAM (26-37),
    // SD/mic/UART do modem (38-44), SD CS (47) e PCIE_PWR (48).
    // Liberados: 21-25, 45 e 46 (modem 4G desligado).
    int occupied[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
                      16, 17, 18, 19, 20, 26, 27, 28, 29, 30, 31, 32, 33, 34,
                      35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 47, 48};
    for (int i = 0; i < sizeof(occupied) / sizeof(occupied[0]); i++) {
        if (gpio == occupied[i]) return false;
    }
    return true;
}

bool ConfigManager::validateConfig(const HardwareConfig& config, String& error) const {
    if (config.gpio_in1 >= 0 && !validateGpio(config.gpio_in1)) {
        error = "GPIO IN1 invalido ou ocupado pela camera/modem";
        return false;
    }
    if (config.gpio_in2 >= 0 && !validateGpio(config.gpio_in2)) {
        error = "GPIO IN2 invalido ou ocupado pela camera/modem";
        return false;
    }
    if (config.gpio_ena >= 0 && !validateGpio(config.gpio_ena)) {
        error = "GPIO ENA invalido ou ocupado pela camera/modem";
        return false;
    }
    if (config.gpio_in1 >= 0 && config.gpio_in2 >= 0 && config.gpio_in1 == config.gpio_in2) {
        error = "IN1 e IN2 devem ser pinos diferentes";
        return false;
    }
    if (config.gpio_in1 >= 0 && config.gpio_ena >= 0 && config.gpio_in1 == config.gpio_ena) {
        error = "IN1 e ENA devem ser pinos diferentes";
        return false;
    }
    if (config.gpio_in2 >= 0 && config.gpio_ena >= 0 && config.gpio_in2 == config.gpio_ena) {
        error = "IN2 e ENA devem ser pinos diferentes";
        return false;
    }
    if (config.motor_voltage <= 0.0f) {
        error = "Tensão do motor deve ser maior que 0";
        return false;
    }
    return true;
}
