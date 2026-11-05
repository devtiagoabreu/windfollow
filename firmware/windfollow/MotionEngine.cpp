#include "MotionEngine.h"

MotionEngine motionEngine;

bool MotionEngine::begin() {
    _isMoving = false;
    return true;
}

void MotionEngine::update() {
    if (_isMoving && _moveDurationMs > 0) {
        if (millis() - _moveStartTime >= (unsigned long)_moveDurationMs) {
            stop();
        }
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
