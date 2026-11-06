#include "L298NDualDriver.h"

void L298NDualDriver::writePwm1(int pwm) {
    if (!_initialized && pwm != 0) return;
#if defined(ESP_ARDUINO_VERSION_MAJOR) && ESP_ARDUINO_VERSION_MAJOR >= 3
    ledcWrite(_config.gpio_ena, pwm);
#else
    ledcWrite(WF_ENA_PWM_CHANNEL, pwm);
#endif
}

void L298NDualDriver::writePwm2(int pwm) {
    if (!_initialized && pwm != 0) return;
#if defined(ESP_ARDUINO_VERSION_MAJOR) && ESP_ARDUINO_VERSION_MAJOR >= 3
    ledcWrite(_config.gpio_enb, pwm);
#else
    ledcWrite(WF_ENB_PWM_CHANNEL, pwm);
#endif
}

bool L298NDualDriver::begin(const HardwareConfig& config) {
    _config = config;
    _moving1 = false;
    _moving2 = false;
    _moveDuration1 = 0;
    _moveDuration2 = 0;
    _currentPwm1 = 0;
    _currentPwm2 = 0;
    _direction1 = "parado";
    _direction2 = "parado";

    bool ok1 = (_config.gpio_in1 >= 0 && _config.gpio_in2 >= 0);
    bool ok2 = (_config.gpio_in3 >= 0 && _config.gpio_in4 >= 0);
    if (!ok1 && !ok2) {
        _initialized = false;
        return false;
    }

    if (ok1) {
        pinMode(_config.gpio_in1, OUTPUT);
        pinMode(_config.gpio_in2, OUTPUT);
        digitalWrite(_config.gpio_in1, LOW);
        digitalWrite(_config.gpio_in2, LOW);
        if (_config.gpio_ena >= 0) {
            pinMode(_config.gpio_ena, OUTPUT);
#if defined(ESP_ARDUINO_VERSION_MAJOR) && ESP_ARDUINO_VERSION_MAJOR >= 3
            ledcAttachChannel(_config.gpio_ena, 5000, 8, WF_ENA_PWM_CHANNEL);
#else
            ledcSetup(WF_ENA_PWM_CHANNEL, 5000, 8);
            ledcAttachPin(_config.gpio_ena, WF_ENA_PWM_CHANNEL);
#endif
            writePwm1(0);
        }
    }

    if (ok2) {
        pinMode(_config.gpio_in3, OUTPUT);
        pinMode(_config.gpio_in4, OUTPUT);
        digitalWrite(_config.gpio_in3, LOW);
        digitalWrite(_config.gpio_in4, LOW);
        if (_config.gpio_enb >= 0) {
            pinMode(_config.gpio_enb, OUTPUT);
#if defined(ESP_ARDUINO_VERSION_MAJOR) && ESP_ARDUINO_VERSION_MAJOR >= 3
            ledcAttachChannel(_config.gpio_enb, 5000, 8, WF_ENB_PWM_CHANNEL);
#else
            ledcSetup(WF_ENB_PWM_CHANNEL, 5000, 8);
            ledcAttachPin(_config.gpio_enb, WF_ENB_PWM_CHANNEL);
#endif
            writePwm2(0);
        }
    }

    _initialized = true;
    return true;
}

void L298NDualDriver::update() {
    if (!_initialized) return;
    // Sem auto-stop por tempo: o motor fica girando ate stop() ser chamado
    // explicitamente (botao Parar ou comando oposto).
}

void L298NDualDriver::stop() {
    stopMotor1();
    stopMotor2();
}

void L298NDualDriver::stopMotor1() {
    if (!_initialized) return;
    digitalWrite(_config.gpio_in1, LOW);
    digitalWrite(_config.gpio_in2, LOW);
    writePwm1(0);
    _currentPwm1 = 0;
    _moving1 = false;
    _moveDuration1 = 0;
    _direction1 = "parado";
}

void L298NDualDriver::stopMotor2() {
    if (!_initialized) return;
    digitalWrite(_config.gpio_in3, LOW);
    digitalWrite(_config.gpio_in4, LOW);
    writePwm2(0);
    _currentPwm2 = 0;
    _moving2 = false;
    _moveDuration2 = 0;
    _direction2 = "parado";
}

void L298NDualDriver::motor1Esquerda(int durationMs, int pwm) {
    if (!_initialized || !(_config.gpio_in1 >= 0 && _config.gpio_in2 >= 0)) return;
    digitalWrite(_config.gpio_in1, HIGH);
    digitalWrite(_config.gpio_in2, LOW);
    writePwm1(pwm);
    _currentPwm1 = pwm;
    _moving1 = true;
    _direction1 = "esquerda";
    // Movimento contínuo: nao para automaticamente. So para com stopMotor1().
    (void)durationMs;
}

void L298NDualDriver::motor1Direita(int durationMs, int pwm) {
    if (!_initialized || !(_config.gpio_in1 >= 0 && _config.gpio_in2 >= 0)) return;
    digitalWrite(_config.gpio_in1, LOW);
    digitalWrite(_config.gpio_in2, HIGH);
    writePwm1(pwm);
    _currentPwm1 = pwm;
    _moving1 = true;
    _direction1 = "direita";
    // Movimento contínuo: nao para automaticamente. So para com stopMotor1().
    (void)durationMs;
}

void L298NDualDriver::motor2Esquerda(int durationMs, int pwm) {
    if (!_initialized || !(_config.gpio_in3 >= 0 && _config.gpio_in4 >= 0)) return;
    digitalWrite(_config.gpio_in3, HIGH);
    digitalWrite(_config.gpio_in4, LOW);
    writePwm2(pwm);
    _currentPwm2 = pwm;
    _moving2 = true;
    _direction2 = "esquerda";
    // Movimento contínuo: nao para automaticamente. So para com stopMotor2().
    (void)durationMs;
}

void L298NDualDriver::motor2Direita(int durationMs, int pwm) {
    if (!_initialized || !(_config.gpio_in3 >= 0 && _config.gpio_in4 >= 0)) return;
    digitalWrite(_config.gpio_in3, LOW);
    digitalWrite(_config.gpio_in4, HIGH);
    writePwm2(pwm);
    _currentPwm2 = pwm;
    _moving2 = true;
    _direction2 = "direita";
    // Movimento contínuo: nao para automaticamente. So para com stopMotor2().
    (void)durationMs;
}

void L298NDualDriver::setSpeed1(int pwm) {
    _currentPwm1 = pwm;
    if (!_initialized) return;
    writePwm1(pwm);
}

void L298NDualDriver::setSpeed2(int pwm) {
    _currentPwm2 = pwm;
    if (!_initialized) return;
    writePwm2(pwm);
}

bool L298NDualDriver::isConfigured() const {
    return _initialized;
}

bool L298NDualDriver::isMoving() const {
    return _moving1 || _moving2;
}

bool L298NDualDriver::isMoving1() const {
    return _moving1;
}

bool L298NDualDriver::isMoving2() const {
    return _moving2;
}

const char* L298NDualDriver::directionName1() const {
    return _direction1;
}

const char* L298NDualDriver::directionName2() const {
    return _direction2;
}

HardwareConfig L298NDualDriver::getConfig() const {
    return _config;
}
