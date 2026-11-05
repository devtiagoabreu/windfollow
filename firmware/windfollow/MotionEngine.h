#ifndef MOTION_ENGINE_H
#define MOTION_ENGINE_H

#include <Arduino.h>
#include "L298NDriver.h"

class MotionEngine {
public:
    bool begin();
    void update();
    void stop();
    void esquerda(int durationMs = 0, int pwm = 200);
    void direita(int durationMs = 0, int pwm = 200);
    bool isMoving() const;
    bool isConfigured() const;

private:
    unsigned long _moveStartTime = 0;
    int _moveDurationMs = 0;
    bool _isMoving = false;
};

extern MotionEngine motionEngine;

#endif
