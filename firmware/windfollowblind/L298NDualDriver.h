#ifndef L298N_DUAL_DRIVER_H
#define L298N_DUAL_DRIVER_H

#include <Arduino.h>
#include "HardwareConfig.h"

class L298NDualDriver {
public:
    bool begin(const HardwareConfig& config);
    void update();
    void stop();
    void stopMotor1();
    void stopMotor2();
    void motor1Esquerda(int durationMs, int pwm);
    void motor1Direita(int durationMs, int pwm);
    void motor2Esquerda(int durationMs, int pwm);
    void motor2Direita(int durationMs, int pwm);
    void setSpeed1(int pwm);
    void setSpeed2(int pwm);
    bool isConfigured() const;
    bool isMoving() const;
    bool isMoving1() const;
    bool isMoving2() const;
    const char* directionName1() const;
    const char* directionName2() const;
    HardwareConfig getConfig() const;

private:
    // Canais LEDC dedicados (ESP32 core 3.x usa ledcAttachChannel)
    static const int WF_ENA_PWM_CHANNEL = 2;
    static const int WF_ENB_PWM_CHANNEL = 3;

    void writePwm1(int pwm);
    void writePwm2(int pwm);

    HardwareConfig _config;
    bool _initialized = false;

    // Motor 1
    int _currentPwm1 = 0;
    bool _moving1 = false;
    unsigned long _moveStart1 = 0;
    int _moveDuration1 = 0;
    const char* _direction1 = "parado";

    // Motor 2
    int _currentPwm2 = 0;
    bool _moving2 = false;
    unsigned long _moveStart2 = 0;
    int _moveDuration2 = 0;
    const char* _direction2 = "parado";
};

extern L298NDualDriver l298nDualDriver;

#endif
