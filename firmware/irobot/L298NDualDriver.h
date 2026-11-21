#ifndef L298N_DUAL_DRIVER_H
#define L298N_DUAL_DRIVER_H

#include "HardwareConfig.h"

class L298NDualDriver {
public:
  L298NDualDriver();

  bool begin(const RobotConfig& config);
  void setLeftMotor(int16_t speed);   // -255..255 (negativo = ré)
  void setRightMotor(int16_t speed);  // -255..255 (negativo = ré)
  void stopAll();
  void setSpeeds(int16_t left, int16_t right);

  bool isInitialized() const { return _initialized; }
  int16_t getLeftSpeed() const { return _leftSpeed; }
  int16_t getRightSpeed() const { return _rightSpeed; }

private:
  RobotConfig _config;
  bool _initialized = false;
  int16_t _leftSpeed = 0;
  int16_t _rightSpeed = 0;

  void applyMotor(int8_t in1, int8_t in2, int8_t pwmPin, int16_t speed);
  uint8_t clampSpeed(int16_t speed) const;
};

#endif // L298N_DUAL_DRIVER_H
