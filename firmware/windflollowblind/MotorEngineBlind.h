#ifndef MOTOR_ENGINE_BLIND_H
#define MOTOR_ENGINE_BLIND_H

#include <Arduino.h>
#include "L298NDualDriver.h"
#include "HardwareConfig.h"

class MotorEngineBlind {
public:
    bool begin();
    void update();
    void stop();
    void stopMotor1();
    void stopMotor2();
    void motor1Esquerda(int durationMs = 0, int pwm = 200);
    void motor1Direita(int durationMs = 0, int pwm = 200);
    void motor2Esquerda(int durationMs = 0, int pwm = 200);
    void motor2Direita(int durationMs = 0, int pwm = 200);
    bool isMoving() const;
    bool isMoving1() const;
    bool isMoving2() const;
    bool isConfigured() const;
    const char* directionName1() const;
    const char* directionName2() const;
    int stepDurationMs() const { return _stepDurationMs; }
    void setStepDurationMs(int ms) { _stepDurationMs = ms; }

private:
    unsigned long _moveStartTime1 = 0;
    unsigned long _moveStartTime2 = 0;
    int _moveDurationMs1 = 0;
    int _moveDurationMs2 = 0;
    bool _isMoving1 = false;
    bool _isMoving2 = false;
    int _stepDurationMs = 150;
};

extern MotorEngineBlind motorEngineBlind;

#endif
