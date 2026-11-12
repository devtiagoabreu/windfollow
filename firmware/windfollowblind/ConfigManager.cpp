#include "ConfigManager.h"

bool ConfigManager::begin() {
    if (!prefs.begin(namespaceName, false)) {
        return false;
    }
    loadFromPrefs();

    // Migracao (uma unica vez): placas nunca configuradas usam os defaults
    // definidos em HardwareConfig.h (conforme docs/documentacao_hardware_esp32_l298n.md).
    if (!prefs.getBool("cfg_migrated_v2", false)) {
        if (_config.gpio_in1 < 0 && _config.gpio_in2 < 0 &&
            _config.gpio_in3 < 0 && _config.gpio_in4 < 0) {
            HardwareConfig defaults;
            _config = defaults;
            prefs.putInt("gpio_in1", _config.gpio_in1);
            prefs.putInt("gpio_in2", _config.gpio_in2);
            prefs.putInt("gpio_ena", _config.gpio_ena);
            prefs.putInt("gpio_in3", _config.gpio_in3);
            prefs.putInt("gpio_in4", _config.gpio_in4);
            prefs.putInt("gpio_enb", _config.gpio_enb);
            prefs.putInt("gpio_btn1", _config.gpio_btn1);
            prefs.putInt("gpio_btn2", _config.gpio_btn2);
            prefs.putInt("step_duration_ms", _config.step_duration_ms);
            prefs.putBool("motor1_enabled", _config.motor1_enabled);
            prefs.putBool("motor2_enabled", _config.motor2_enabled);
            Serial.println(F("[config] padrao aplicado: WindFollowBlind"));
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
    _config.gpio_in3 = prefs.getInt("gpio_in3", -1);
    _config.gpio_in4 = prefs.getInt("gpio_in4", -1);
    _config.gpio_enb = prefs.getInt("gpio_enb", -1);
    _config.gpio_btn1 = prefs.getInt("gpio_btn1", -1);
    _config.gpio_btn2 = prefs.getInt("gpio_btn2", -1);
    _config.step_duration_ms = prefs.getInt("step_duration_ms", 150);
    _config.motor1_enabled = prefs.getBool("motor1_enabled", true);
    _config.motor2_enabled = prefs.getBool("motor2_enabled", true);
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
    prefs.putInt("gpio_in3", config.gpio_in3);
    prefs.putInt("gpio_in4", config.gpio_in4);
    prefs.putInt("gpio_enb", config.gpio_enb);
    prefs.putInt("gpio_btn1", config.gpio_btn1);
    prefs.putInt("gpio_btn2", config.gpio_btn2);
    prefs.putInt("step_duration_ms", config.step_duration_ms);
    prefs.putBool("motor1_enabled", config.motor1_enabled);
    prefs.putBool("motor2_enabled", config.motor2_enabled);
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
    // GPIOs livres para uso geral no ESP32 DevKit (nao sao strapping pins
    // críticos nem usados por periféricos internos do T-SIMCAM, pois este
    // projeto nao usa a placa T-SIMCAM).
    int occupied[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
                      15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27,
                      28, 29, 30, 31, 32, 33, 34, 35, 36, 39};
    for (int i = 0; i < sizeof(occupied) / sizeof(occupied[0]); i++) {
        if (gpio == occupied[i]) return false;
    }
    return true;
}

bool ConfigManager::validateConfig(const HardwareConfig& config, String& error) const {
    if (config.gpio_in1 >= 0 && !validateGpio(config.gpio_in1)) {
        error = "GPIO IN1 invalido ou reservado";
        return false;
    }
    if (config.gpio_in2 >= 0 && !validateGpio(config.gpio_in2)) {
        error = "GPIO IN2 invalido ou reservado";
        return false;
    }
    if (config.gpio_in3 >= 0 && !validateGpio(config.gpio_in3)) {
        error = "GPIO IN3 invalido ou reservado";
        return false;
    }
    if (config.gpio_in4 >= 0 && !validateGpio(config.gpio_in4)) {
        error = "GPIO IN4 invalido ou reservado";
        return false;
    }
    if (config.gpio_btn1 >= 0 && !validateGpio(config.gpio_btn1)) {
        error = "GPIO Botao 1 invalido ou reservado";
        return false;
    }
    if (config.gpio_btn2 >= 0 && !validateGpio(config.gpio_btn2)) {
        error = "GPIO Botao 2 invalido ou reservado";
        return false;
    }
    // Pinos dos dois motores devem ser distintos entre si
    int pins[] = {config.gpio_in1, config.gpio_in2, config.gpio_in3,
                  config.gpio_in4, config.gpio_btn1, config.gpio_btn2};
    for (int i = 0; i < 6; i++) {
        if (pins[i] < 0) continue;
        for (int j = i + 1; j < 6; j++) {
            if (pins[j] >= 0 && pins[i] == pins[j]) {
                error = "GPIOs repetidos entre motores e botoes";
                return false;
            }
        }
    }
    return true;
}
