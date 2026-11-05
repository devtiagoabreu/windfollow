#ifndef CAMERA_SERVICE_H
#define CAMERA_SERVICE_H

#include <Arduino.h>

// Encapsula a esp32-camera. Se a biblioteca nao estiver instalada, o firmware
// continua compilando e a interface informa que a camera esta indisponivel.
class CameraService {
public:
    bool begin();
    bool ensureStarted();
    bool isReady() const;
    bool isAvailable() const;
    bool libraryAvailable() const;
    bool isEnabled() const;
    void setEnabled(bool enabled);

    const char* sensorName() const;
    int width() const;
    int height() const;
    int frameCount() const;

    // JPEG codificado em base64 (string valida ate a proxima chamada).
    const char* captureBase64(size_t& length);

    // Quadro em tons de cinza reduzido para deteccao de movimento.
    bool grabGray(uint8_t* out, int outW, int outH, int& srcW, int& srcH);

private:
    const uint8_t* captureJpeg(size_t& length);

    bool _ready = false;
    bool _started = false;
    bool _enabled = true;
    int _frameCount = 0;
    int _width = 0;
    int _height = 0;
    char _sensor[16] = "none";

    uint8_t* _jpegBuf = nullptr;
    size_t _jpegCap = 0;
    uint8_t* _rgbBuf = nullptr;
    size_t _rgbCap = 0;
    char* _b64Buf = nullptr;
    size_t _b64Cap = 0;
};

extern CameraService cameraService;

#endif // CAMERA_SERVICE_H
