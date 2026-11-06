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
    // Padrao do projeto para a T-SIMCAM V1.3 com o modem 4G desligado.
    // O GPIO48 (PCIE_PWR) e mantido em LOW no boot para liberar 45/46.
    int gpio_in1 = 45;
    int gpio_in2 = 46;
    int gpio_ena = 21;
    float motor_voltage = 12.0f;
    bool motor_enabled = true;
    int dead_zone = 30;
    int sensitivity = 30;
    int min_area = 100;
    int step_duration_ms = 150;
    int stabilization_ms = 300;
    bool auto_tracking = false;

    // Controle proporcional do rastreamento (só eixo X)
    float kp_x = 2.5f;          // ms de movimento por pixel de erro
    int move_min_ms = 40;       // duração mínima de um passo
    int move_max_ms = 400;      // duração máxima de um passo
    int settle_ms = 300;        // tempo de assento após mover (suprime detecção por ego-movimento)
    float ema_alpha_err = 0.7f; // suavização temporal do erro (remove jitter)
    int miss_frames = 8;        // quadros sem alvo antes de soltar o rastreamento
    float bg_alpha = 0.95f;     // adaptação do modelo de fundo (EMA)
    float pwm_kp = 1.2f;        // PWM por pixel de erro (desacelera perto do centro)
    int pwm_min = 90;           // PWM mínimo para vencer atrito/inercia do motor DC
    int pwm_max = 200;          // PWM máximo
    float dead_zone_hyst = 1.5f;// histerese: só sai da zona morta se erro > dead_zone*hyst
    int trackWindowW = 25;      // largura da janela de rastreamento (px, resolucao de detect 160)
    int trackWindowH = 25;      // altura (reservado)
    float refAlpha = 0.15f;     // deslize suave do centro de referencia quando parado na zona
};

#endif
