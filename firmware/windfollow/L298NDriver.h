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
    // Canal LEDC dedicado ao ENA. A esp32-camera usa LEDC_CHANNEL_0/TIMER_0
    // para o XCLK; o canal 2 usa outro timer e nao conflita.
    static const int WF_ENA_PWM_CHANNEL = 2;

    void writePwm(int pwm);

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
