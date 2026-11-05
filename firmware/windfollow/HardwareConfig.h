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
    int gpio_in1 = -1;
    int gpio_in2 = -1;
    int gpio_ena = -1;
    float motor_voltage = 0.0f;
    bool motor_enabled = false;
    int dead_zone = 30;
    int sensitivity = 30;
    int min_area = 100;
    int step_duration_ms = 150;
    int stabilization_ms = 300;
    bool auto_tracking = false;
};

#endif
