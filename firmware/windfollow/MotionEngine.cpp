#include "MotionEngine.h"

bool MotionEngine::begin() {
    _isMoving = false;
    _moveDurationMs = 0;
    return true;
}

void MotionEngine::update() {
    l298nDriver.update();
    _isMoving = l298nDriver.isMoving();
    if (!_isMoving) {
        _moveDurationMs = 0;
    }
}

void MotionEngine::stop() {
    l298nDriver.stop();
    _isMoving = false;
    _moveDurationMs = 0;
}

void MotionEngine::esquerda(int durationMs, int pwm) {
    l298nDriver.esquerda(durationMs, pwm);
    _isMoving = true;
    _moveDurationMs = durationMs;
    _moveStartTime = millis();
}

void MotionEngine::direita(int durationMs, int pwm) {
    l298nDriver.direita(durationMs, pwm);
    _isMoving = true;
    _moveDurationMs = durationMs;
    _moveStartTime = millis();
}

bool MotionEngine::isMoving() const {
    return _isMoving;
}

bool MotionEngine::isConfigured() const {
    return l298nDriver.isConfigured();
}
