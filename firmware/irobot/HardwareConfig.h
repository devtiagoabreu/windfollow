#ifndef HARDWARE_CONFIG_H
#define HARDWARE_CONFIG_H

#include <Arduino.h>

// --- GPIOs do L298N (motores direito e esquerdo) ---
// Motor esquerdo (roda dianteira + traseira esquerda)
static const int8_t M_LEFT_IN1  = 26;   // Sentido 1
static const int8_t M_LEFT_IN2  = 25;   // Sentido 2
static const int8_t M_LEFT_ENA  = 27;   // PWM / habilitação (era botão 1)

// Motor direito (roda dianteira + traseira direita)
static const int8_t M_RIGHT_IN3 = 32;   // Sentido 1
static const int8_t M_RIGHT_IN4 = 33;   // Sentido 2
static const int8_t M_RIGHT_ENB = 14;   // PWM / habilitação (era botão 2)

// --- Configuração PWM ---
static const int   PWM_FREQ    = 1000;  // Hz
static const int   PWM_RES     = 8;     // bits (0-255)
static const int   PWM_CHANNEL_LEFT  = 0;
static const int   PWM_CHANNEL_RIGHT = 1;

// --- Limites de velocidade (0-255) ---
static const uint8_t PWM_MIN    = 60;   // abaixo disso o motor não gira
static const uint8_t PWM_MAX    = 255;
static const uint8_t PWM_DEFAULT = 150;

// --- Duração de pulso para movimentos temporizados (ms) ---
static const unsigned long PULSE_DURATION_DEFAULT = 500;

// --- Estrutura de configuração salva em NVS ---
struct RobotConfig {
  int8_t  gpioLeftIn1  = M_LEFT_IN1;
  int8_t  gpioLeftIn2  = M_LEFT_IN2;
  int8_t  gpioLeftEna  = M_LEFT_ENA;
  int8_t  gpioRightIn3 = M_RIGHT_IN3;
  int8_t  gpioRightIn4 = M_RIGHT_IN4;
  int8_t  gpioRightEnb = M_RIGHT_ENB;
  uint8_t pwmMin       = PWM_MIN;
  uint8_t pwmMax       = PWM_MAX;
  uint8_t pwmDefault   = PWM_DEFAULT;
  unsigned long pulseDurationMs = PULSE_DURATION_DEFAULT;
  bool    motorsEnabled = true;
};

// --- Comandos de movimento diferencial ---
enum class RobotDirection {
  STOP,
  FRENTE,
  TRAS,
  ESQUERDA,
  DIREITA
};

#endif // HARDWARE_CONFIG_H
