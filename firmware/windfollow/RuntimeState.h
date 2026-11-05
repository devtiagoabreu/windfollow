#ifndef RUNTIME_STATE_H
#define RUNTIME_STATE_H

#include <Arduino.h>

// Estado compartilhado entre o loop de rastreamento, a camera e o dashboard.
struct TrackingStatus {
    bool motionDetected = false;
    int boxX = 0;
    int boxY = 0;
    int boxW = 0;
    int boxH = 0;
    int centerX = 0;
    int centerY = 0;
    int errorX = 0;
    int frameWidth = 320;
    int frameHeight = 240;
    const char* direction = "centro";
    bool tracking = false;
    bool motorMoving = false;
    const char* motorDirection = "parado";
    unsigned long lastMotionMs = 0;
};

struct CameraStatus {
    bool ready = false;
    bool available = false;
    bool enabled = true;
    int frameCount = 0;
    int width = 320;
    int height = 240;
    char sensor[16] = "none";
};

extern TrackingStatus g_trackingStatus;
extern CameraStatus g_cameraStatus;

#endif // RUNTIME_STATE_H
