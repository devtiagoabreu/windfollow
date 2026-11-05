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
};

#endif
