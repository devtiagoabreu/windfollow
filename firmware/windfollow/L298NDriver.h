#ifndef L298N_DRIVER_H
#define L298N_DRIVER_H

#include <Arduino.h>
#include "HardwareConfig.h"

class L298NDriver {
public:
    bool begin(const HardwareConfig& config);
    void update();
    void stop();
    void esquerda(int durationMs, int pwm);
    void direita(int durationMs, int pwm);
    void setSpeed(int pwm);
    bool isConfigured() const;
    bool isMoving() const;
    const char* directionName() const;
    HardwareConfig getConfig() const;

private:
    HardwareConfig _config;
    bool _initialized = false;
    int _currentPwm = 0;
    bool _moving = false;
    unsigned long _moveStart = 0;
    int _moveDuration = 0;
    const char* _direction = "parado";
};

extern L298NDriver l298nDriver;

#endif // L298N_DRIVER_H
