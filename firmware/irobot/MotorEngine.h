#ifndef MOTOR_ENGINE_H
#define MOTOR_ENGINE_H

#include "HardwareConfig.h"
#include "L298NDualDriver.h"

class MotorEngine {
public:
  MotorEngine();

  bool begin(L298NDualDriver* driver, const RobotConfig& config);

  // Comandos diferenciais do robô
  void move(RobotDirection dir, uint8_t speed = PWM_DEFAULT);
  void moveDifferential(int8_t leftSpeed, int8_t rightSpeed);
  void stop();

  // Controle individual por motor (compatível com API antiga)
  void setLeft(int8_t speed);
  void setRight(int8_t speed);

  // Estado atual
  RobotDirection getCurrentDirection() const { return _currentDir; }
  int8_t getLeftSpeed() const { return _driver ? _driver->getLeftSpeed() : 0; }
  int8_t getRightSpeed() const { return _driver ? _driver->getRightSpeed() : 0; }
  bool isMoving() const { return _currentDir != RobotDirection::STOP; }

private:
  L298NDualDriver* _driver = nullptr;
  RobotConfig _config;
  RobotDirection _currentDir = RobotDirection::STOP;
  uint8_t _currentSpeed = PWM_DEFAULT;
};

#endif // MOTOR_ENGINE_H
