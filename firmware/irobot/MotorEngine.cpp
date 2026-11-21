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

  int16_t s = speed;
  int16_t left  = 0;
  int16_t right = 0;

  switch (dir) {
    case RobotDirection::FRENTE:
      left  = s;
      right = s;
      break;
    case RobotDirection::TRAS:
      left  = -s;
      right = -s;
      break;
    case RobotDirection::ESQUERDA:
      // Gira para a esquerda: direito frente, esquerdo parado
      left  = 0;
      right = s;
      break;
    case RobotDirection::DIREITA:
      // Gira para a direita: esquerdo frente, direito parado
      left  = s;
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

void MotorEngine::moveDifferential(int16_t leftSpeed, int16_t rightSpeed) {
  if (!_driver) return;

  // Mantem o estado coerente para a telemetria, inclusive no pivo
  // (uma roda em re e outra em frente).
  if (leftSpeed == 0 && rightSpeed == 0) {
    _currentDir = RobotDirection::STOP;
  } else if (leftSpeed >= 0 && rightSpeed >= 0) {
    _currentDir = RobotDirection::FRENTE;
  } else if (leftSpeed <= 0 && rightSpeed <= 0) {
    _currentDir = RobotDirection::TRAS;
  } else if (leftSpeed < 0 && rightSpeed > 0) {
    _currentDir = RobotDirection::ESQUERDA;
  } else {
    _currentDir = RobotDirection::DIREITA;
  }
  int16_t al = leftSpeed >= 0 ? leftSpeed : -leftSpeed;
  int16_t ar = rightSpeed >= 0 ? rightSpeed : -rightSpeed;
  _currentSpeed = (uint8_t)(al > ar ? al : ar);

  _driver->setSpeeds(leftSpeed, rightSpeed);
}

void MotorEngine::stop() {
  move(RobotDirection::STOP, 0);
}

void MotorEngine::setLeft(int16_t speed) {
  if (!_driver) return;
  _driver->setLeftMotor(speed);
}

void MotorEngine::setRight(int16_t speed) {
  if (!_driver) return;
  _driver->setRightMotor(speed);
}
