#include "L298NDriver.h"

L298NDriver l298nDriver;

bool L298NDriver::begin(const HardwareConfig& config) {
    _config = config;
    if (_config.gpio_in1 < 0 || _config.gpio_in2 < 0 || _config.gpio_ena < 0) {
        _initialized = false;
        return false;
    }
    pinMode(_config.gpio_in1, OUTPUT);
    pinMode(_config.gpio_in2, OUTPUT);
    pinMode(_config.gpio_ena, OUTPUT);
    digitalWrite(_config.gpio_in1, LOW);
    digitalWrite(_config.gpio_in2, LOW);
    analogWrite(_config.gpio_ena, 0);
    _initialized = true;
    return true;
}

void L298NDriver::stop() {
    if (!_initialized) return;
    digitalWrite(_config.gpio_in1, LOW);
    digitalWrite(_config.gpio_in2, LOW);
    analogWrite(_config.gpio_ena, 0);
    _currentPwm = 0;
}

void L298NDriver::esquerda(int durationMs, int pwm) {
    if (!_initialized) return;
    digitalWrite(_config.gpio_in1, HIGH);
    digitalWrite(_config.gpio_in2, LOW);
    analogWrite(_config.gpio_ena, pwm);
    _currentPwm = pwm;
    delay(durationMs);
    stop();
}

void L298NDriver::direita(int durationMs, int pwm) {
    if (!_initialized) return;
    digitalWrite(_config.gpio_in1, LOW);
    digitalWrite(_config.gpio_in2, HIGH);
    analogWrite(_config.gpio_ena, pwm);
    _currentPwm = pwm;
    delay(durationMs);
    stop();
}

void L298NDriver::setSpeed(int pwm) {
    _currentPwm = pwm;
    if (!_initialized) return;
    analogWrite(_config.gpio_ena, pwm);
}

bool L298NDriver::isConfigured() const {
    return _initialized;
}

HardwareConfig L298NDriver::getConfig() const {
    return _config;
}
