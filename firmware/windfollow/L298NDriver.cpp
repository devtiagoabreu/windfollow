#include "L298NDriver.h"

void L298NDriver::writePwm(int pwm) {
    if (!_initialized && pwm != 0) return;
#if defined(ESP_ARDUINO_VERSION_MAJOR) && ESP_ARDUINO_VERSION_MAJOR >= 3
    ledcWrite(_config.gpio_ena, pwm);
#else
    ledcWrite(WF_ENA_PWM_CHANNEL, pwm);
#endif
}

bool L298NDriver::begin(const HardwareConfig& config) {
    _config = config;
    _moving = false;
    _moveDuration = 0;
    _currentPwm = 0;
    _direction = "parado";

    if (_config.gpio_in1 < 0 || _config.gpio_in2 < 0 || _config.gpio_ena < 0) {
        _initialized = false;
        return false;
    }
    pinMode(_config.gpio_in1, OUTPUT);
    pinMode(_config.gpio_in2, OUTPUT);
    pinMode(_config.gpio_ena, OUTPUT);
    digitalWrite(_config.gpio_in1, LOW);
    digitalWrite(_config.gpio_in2, LOW);
#if defined(ESP_ARDUINO_VERSION_MAJOR) && ESP_ARDUINO_VERSION_MAJOR >= 3
    ledcAttachChannel(_config.gpio_ena, 5000, 8, WF_ENA_PWM_CHANNEL);
#else
    ledcSetup(WF_ENA_PWM_CHANNEL, 5000, 8);
    ledcAttachPin(_config.gpio_ena, WF_ENA_PWM_CHANNEL);
#endif
    writePwm(0);
    _initialized = true;
    return true;
}

void L298NDriver::update() {
    if (!_initialized) return;
    if (_moving && _moveDuration > 0 &&
        (millis() - _moveStart) >= (unsigned long)_moveDuration) {
        stop();
    }
}

void L298NDriver::stop() {
    if (!_initialized) return;
    digitalWrite(_config.gpio_in1, LOW);
    digitalWrite(_config.gpio_in2, LOW);
    writePwm(0);
    _currentPwm = 0;
    _moving = false;
    _moveDuration = 0;
    _direction = "parado";
}

void L298NDriver::esquerda(int durationMs, int pwm) {
    if (!_initialized) return;
    digitalWrite(_config.gpio_in1, HIGH);
    digitalWrite(_config.gpio_in2, LOW);
    writePwm(pwm);
    _currentPwm = pwm;
    _moving = true;
    _direction = "esquerda";
    _moveStart = millis();
    _moveDuration = durationMs;
}

void L298NDriver::direita(int durationMs, int pwm) {
    if (!_initialized) return;
    digitalWrite(_config.gpio_in1, LOW);
    digitalWrite(_config.gpio_in2, HIGH);
    writePwm(pwm);
    _currentPwm = pwm;
    _moving = true;
    _direction = "direita";
    _moveStart = millis();
    _moveDuration = durationMs;
}

void L298NDriver::setSpeed(int pwm) {
    _currentPwm = pwm;
    if (!_initialized) return;
    writePwm(pwm);
}

bool L298NDriver::isConfigured() const {
    return _initialized;
}

bool L298NDriver::isMoving() const {
    return _moving;
}

const char* L298NDriver::directionName() const {
    return _direction;
}

HardwareConfig L298NDriver::getConfig() const {
    return _config;
}
