#include "MotorEngineBlind.h"

bool MotorEngineBlind::begin() {
    _isMoving1 = false;
    _isMoving2 = false;
    _moveDurationMs1 = 0;
    _moveDurationMs2 = 0;
    return true;
}

void MotorEngineBlind::update() {
    l298nDualDriver.update();
    _isMoving1 = l298nDualDriver.isMoving1();
    _isMoving2 = l298nDualDriver.isMoving2();
    if (!_isMoving1) _moveDurationMs1 = 0;
    if (!_isMoving2) _moveDurationMs2 = 0;
}

void MotorEngineBlind::stop() {
    l298nDualDriver.stop();
    _isMoving1 = false;
    _isMoving2 = false;
    _moveDurationMs1 = 0;
    _moveDurationMs2 = 0;
}

void MotorEngineBlind::stopMotor1() {
    l298nDualDriver.stopMotor1();
    _isMoving1 = false;
    _moveDurationMs1 = 0;
}

void MotorEngineBlind::stopMotor2() {
    l298nDualDriver.stopMotor2();
    _isMoving2 = false;
    _moveDurationMs2 = 0;
}

void MotorEngineBlind::motor1Esquerda(int durationMs, int pwm) {
    if (durationMs <= 0) durationMs = _stepDurationMs;
    l298nDualDriver.motor1Esquerda(durationMs, pwm);
    _isMoving1 = true;
    _moveDurationMs1 = durationMs;
    _moveStartTime1 = millis();
}

void MotorEngineBlind::motor1Direita(int durationMs, int pwm) {
    if (durationMs <= 0) durationMs = _stepDurationMs;
    l298nDualDriver.motor1Direita(durationMs, pwm);
    _isMoving1 = true;
    _moveDurationMs1 = durationMs;
    _moveStartTime1 = millis();
}

void MotorEngineBlind::motor2Esquerda(int durationMs, int pwm) {
    if (durationMs <= 0) durationMs = _stepDurationMs;
    l298nDualDriver.motor2Esquerda(durationMs, pwm);
    _isMoving2 = true;
    _moveDurationMs2 = durationMs;
    _moveStartTime2 = millis();
}

void MotorEngineBlind::motor2Direita(int durationMs, int pwm) {
    if (durationMs <= 0) durationMs = _stepDurationMs;
    l298nDualDriver.motor2Direita(durationMs, pwm);
    _isMoving2 = true;
    _moveDurationMs2 = durationMs;
    _moveStartTime2 = millis();
}

bool MotorEngineBlind::isMoving() const {
    return _isMoving1 || _isMoving2;
}

bool MotorEngineBlind::isMoving1() const {
    return _isMoving1;
}

bool MotorEngineBlind::isMoving2() const {
    return _isMoving2;
}

bool MotorEngineBlind::isConfigured() const {
    return l298nDualDriver.isConfigured();
}

const char* MotorEngineBlind::directionName1() const {
    return l298nDualDriver.directionName1();
}

const char* MotorEngineBlind::directionName2() const {
    return l298nDualDriver.directionName2();
}
