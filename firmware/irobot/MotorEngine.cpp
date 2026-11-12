#include "MotorEngine.h"

MotorEngine::MotorEngine() {}

bool MotorEngine::begin(L298NDualDriver* driver, const RobotConfig& config) {
  _driver = driver;
  _config = config;
  return _driver != nullptr;
}

void MotorEngine::move(RobotDirection dir, uint8_t speed) {
  if (!_driver) return;

  _currentDir = dir;
  _currentSpeed = speed;

  int8_t left  = 0;
  int8_t right = 0;

  switch (dir) {
    case RobotDirection::FRENTE:
      left  = speed;
      right = speed;
      break;
    case RobotDirection::TRAS:
      left  = -static_cast<int8_t>(speed);
      right = -static_cast<int8_t>(speed);
      break;
    case RobotDirection::ESQUERDA:
      // Gira para a esquerda: direito frente, esquerdo parado
      left  = 0;
      right = speed;
      break;
    case RobotDirection::DIREITA:
      // Gira para a direita: esquerdo frente, direito parado
      left  = speed;
      right = 0;
      break;
    case RobotDirection::STOP:
    default:
      left  = 0;
      right = 0;
      break;
  }

  _driver->setSpeeds(left, right);
}

void MotorEngine::moveDifferential(int8_t leftSpeed, int8_t rightSpeed) {
  if (!_driver) return;

  _currentDir = RobotDirection::STOP; // controle manual, não mapeia para direção simples
  _currentSpeed = PWM_DEFAULT;

  _driver->setSpeeds(leftSpeed, rightSpeed);
}

void MotorEngine::stop() {
  move(RobotDirection::STOP, 0);
}

void MotorEngine::setLeft(int8_t speed) {
  if (!_driver) return;
  _driver->setLeftMotor(speed);
}

void MotorEngine::setRight(int8_t speed) {
  if (!_driver) return;
  _driver->setRightMotor(speed);
}
