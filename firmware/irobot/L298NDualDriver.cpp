#include "L298NDualDriver.h"
#include <esp32-hal-ledc.h>

L298NDualDriver::L298NDualDriver() {}

bool L298NDualDriver::begin(const RobotConfig& config) {
  _config = config;

  // Configura pinos de direção como saída
  pinMode(_config.gpioLeftIn1,  OUTPUT);
  pinMode(_config.gpioLeftIn2,  OUTPUT);
  pinMode(_config.gpioRightIn3, OUTPUT);
  pinMode(_config.gpioRightIn4, OUTPUT);

  // Configura PWM nos pinos ENA/ENB usando a nova API LEDC (ESP32 core 3.x)
  // ledcAttach(pin, freq, resolution) associa o pino a um canal automaticamente
  if (!ledcAttach(_config.gpioLeftEna,  PWM_FREQ, PWM_RES)) {
    Serial.println("Falha ao attach ENA");
    return false;
  }
  if (!ledcAttach(_config.gpioRightEnb, PWM_FREQ, PWM_RES)) {
    Serial.println("Falha ao attach ENB");
    return false;
  }

  // Inicializa parados
  digitalWrite(_config.gpioLeftIn1,  LOW);
  digitalWrite(_config.gpioLeftIn2,  LOW);
  digitalWrite(_config.gpioRightIn3, LOW);
  digitalWrite(_config.gpioRightIn4, LOW);
  ledcWrite(_config.gpioLeftEna,  0);
  ledcWrite(_config.gpioRightEnb, 0);

  _initialized = true;
  return true;
}

void L298NDualDriver::applyMotor(int8_t in1, int8_t in2, int8_t pwmPin, int8_t speed) {
  uint8_t pwm = clampSpeed(speed);

  if (speed > 0) {
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
  } else if (speed < 0) {
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
  } else {
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
  }

  ledcWrite(pwmPin, pwm);
}

uint8_t L298NDualDriver::clampSpeed(int8_t speed) const {
  if (speed >  _config.pwmMax) return _config.pwmMax;
  if (speed < -_config.pwmMax) return _config.pwmMax;
  uint8_t absSpeed = abs(speed);
  if (absSpeed < _config.pwmMin && absSpeed > 0) return _config.pwmMin;
  return absSpeed;
}

void L298NDualDriver::setLeftMotor(int8_t speed) {
  if (!_initialized) return;
  _leftSpeed = speed;
  applyMotor(_config.gpioLeftIn1, _config.gpioLeftIn2, _config.gpioLeftEna, speed);
}

void L298NDualDriver::setRightMotor(int8_t speed) {
  if (!_initialized) return;
  _rightSpeed = speed;
  applyMotor(_config.gpioRightIn3, _config.gpioRightIn4, _config.gpioRightEnb, speed);
}

void L298NDualDriver::setSpeeds(int8_t left, int8_t right) {
  setLeftMotor(left);
  setRightMotor(right);
}

void L298NDualDriver::stopAll() {
  setLeftMotor(0);
  setRightMotor(0);
}
