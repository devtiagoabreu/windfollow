#ifndef HARDWARE_CONFIG_H
#define HARDWARE_CONFIG_H

#include <Arduino.h>

enum class SystemState {
    Boot,
    Init,
    Ready,
    Error
};

struct HardwareConfig {
    // Padrao do projeto WindFollowBlind (ESP32 + L298N + 2 motores DC + 2 botoes NA).
    // Conforme docs/documentacao_hardware_esp32_l298n.md:
    //   Motor 1: IN1=GPIO26, IN2=GPIO25, ENA (jumper habilitado)
    //   Motor 2: IN3=GPIO32, IN4=GPIO33, ENB (jumper habilitado)
    //   Botao 1: GPIO27 <-> GND  (avanca sequencia do motor 1)
    //   Botao 2: GPIO14 <-> GND  (avanca sequencia do motor 2)
    int gpio_in1 = 26;
    int gpio_in2 = 25;
    int gpio_ena = -1;   // -1 = jumper ENA instalado (sem PWM)
    int gpio_in3 = 32;
    int gpio_in4 = 33;
    int gpio_enb = -1;   // -1 = jumper ENB instalado (sem PWM)
    int gpio_btn1 = 27;
    int gpio_btn2 = 14;

    // Controle de movimento (ajustavel via painel web)
    int step_duration_ms = 150;   // duracao do acionamento do motor a cada comando
    bool motor1_enabled = true;
    bool motor2_enabled = true;
};

#endif
