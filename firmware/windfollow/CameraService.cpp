#include "CameraService.h"
#include "RuntimeState.h"
#include <Arduino.h>

#if __has_include(<esp_camera.h>)
#include "esp_camera.h"
#include "img_converters.h"
#define WF_HAS_CAMERA 1
#else
#define WF_HAS_CAMERA 0
#endif

CameraService cameraService;

#if WF_HAS_CAMERA
// Pinagem oficial da T-SIMCAM V1.3 (IR-CUT), extraida do exemplo de fabrica
// LilyGo-Camera-Series/examples/t_sim_cam_factory/config.h.
// NAO use a tabela generica da wiki: ela aponta para GPIOs do flash/PSRAM
// (26-37) e provoca panic (LoadProhibited) no esp_camera_init.
#define WF_CAM_PIN_PWDN   -1
#define WF_CAM_PIN_RESET  -1
#define WF_CAM_PIN_XCLK   14
#define WF_CAM_PIN_SIOD   4
#define WF_CAM_PIN_SIOC   5
#define WF_CAM_PIN_D7     15
#define WF_CAM_PIN_D6     16
#define WF_CAM_PIN_D5     17
#define WF_CAM_PIN_D4     12
#define WF_CAM_PIN_D3     10
#define WF_CAM_PIN_D2     8
#define WF_CAM_PIN_D1     9
#define WF_CAM_PIN_D0     11
#define WF_CAM_PIN_VSYNC  6
#define WF_CAM_PIN_HREF   7
#define WF_CAM_PIN_PCLK   13
#endif

static void* wfAlloc(size_t size) {
    if (size == 0) return nullptr;
    void* p = nullptr;
    if (psramFound()) {
        p = ps_malloc(size);
    }
    if (!p) {
        p = malloc(size);
    }
    return p;
}

static const char B64_TABLE[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static size_t base64Encode(const uint8_t* src, size_t len, char* out, size_t outCap) {
    size_t o = 0;
    for (size_t i = 0; i < len; i += 3) {
        if (o + 4 > outCap) return 0;
        uint32_t v = (uint32_t)src[i] << 16;
        if (i + 1 < len) v |= (uint32_t)src[i + 1] << 8;
        if (i + 2 < len) v |= (uint32_t)src[i + 2];
        out[o++] = B64_TABLE[(v >> 18) & 0x3F];
        out[o++] = B64_TABLE[(v >> 12) & 0x3F];
        out[o++] = (i + 1 < len) ? B64_TABLE[(v >> 6) & 0x3F] : '=';
        out[o++] = (i + 2 < len) ? B64_TABLE[v & 0x3F] : '=';
    }
    return o;
}

bool CameraService::begin() {
#if WF_HAS_CAMERA
    if (_ready) return true;
    if (_started) return false;
    _started = true;

    Serial.println(F("[camera] inicializando (T-SIMCAM V1.3)..."));

    camera_config_t config = {};
    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer = LEDC_TIMER_0;
    config.pin_d0 = WF_CAM_PIN_D0;
    config.pin_d1 = WF_CAM_PIN_D1;
    config.pin_d2 = WF_CAM_PIN_D2;
    config.pin_d3 = WF_CAM_PIN_D3;
    config.pin_d4 = WF_CAM_PIN_D4;
    config.pin_d5 = WF_CAM_PIN_D5;
    config.pin_d6 = WF_CAM_PIN_D6;
    config.pin_d7 = WF_CAM_PIN_D7;
    config.pin_xclk = WF_CAM_PIN_XCLK;
    config.pin_pclk = WF_CAM_PIN_PCLK;
    config.pin_vsync = WF_CAM_PIN_VSYNC;
    config.pin_href = WF_CAM_PIN_HREF;
#if defined(ESP_ARDUINO_VERSION_MAJOR) && ESP_ARDUINO_VERSION_MAJOR >= 3
    config.pin_sccb_sda = WF_CAM_PIN_SIOD;
    config.pin_sccb_scl = WF_CAM_PIN_SIOC;
#else
    config.pin_sscb_sda = WF_CAM_PIN_SIOD;
    config.pin_sscb_scl = WF_CAM_PIN_SIOC;
#endif
    config.pin_pwdn = WF_CAM_PIN_PWDN;
    config.pin_reset = WF_CAM_PIN_RESET;
    config.xclk_freq_hz = 20000000;
    config.pixel_format = PIXFORMAT_JPEG;
    config.frame_size = FRAMESIZE_QVGA;
    config.jpeg_quality = 12;
    config.fb_count = psramFound() ? 2 : 1;
    config.fb_location = psramFound() ? CAMERA_FB_IN_PSRAM : CAMERA_FB_IN_DRAM;
    config.grab_mode = CAMERA_GRAB_LATEST;

    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK) {
        Serial.printf("[camera] init falhou: 0x%x\n", err);
        g_cameraStatus.ready = false;
        g_cameraStatus.available = true;
        return false;
    }

    sensor_t* sensor = esp_camera_sensor_get();
    if (sensor) {
        int pid = sensor->id.PID;
        if (pid == 0x26 || pid == 0x2640) {
            strncpy(_sensor, "OV2640", sizeof(_sensor) - 1);
        } else if (pid == 0x56 || pid == 0x5640) {
            strncpy(_sensor, "OV5640", sizeof(_sensor) - 1);
        } else {
            snprintf(_sensor, sizeof(_sensor), "PID:0x%02X", pid);
        }
    }

    _jpegCap = 96 * 1024;
    _jpegBuf = (uint8_t*)wfAlloc(_jpegCap);

    _rgbCap = 320 * 240 * 3;
    _rgbBuf = (uint8_t*)wfAlloc(_rgbCap);

    _b64Cap = 140 * 1024;
    _b64Buf = (char*)wfAlloc(_b64Cap);

    _width = 320;
    _height = 240;
    _ready = (_jpegBuf != nullptr && _b64Buf != nullptr);

    g_cameraStatus.ready = _ready;
    g_cameraStatus.available = true;
    g_cameraStatus.enabled = _enabled;
    g_cameraStatus.width = _width;
    g_cameraStatus.height = _height;
    strncpy(g_cameraStatus.sensor, _sensor, sizeof(g_cameraStatus.sensor) - 1);

    Serial.printf("[camera] %s pronta (%dx%d)\n", _sensor, _width, _height);
    return _ready;
#else
    Serial.println("[camera] biblioteca esp32-camera ausente; camera desativada");
    g_cameraStatus.ready = false;
    g_cameraStatus.available = false;
    g_cameraStatus.enabled = false;
    strncpy(g_cameraStatus.sensor, "none", sizeof(g_cameraStatus.sensor) - 1);
    _ready = false;
    _started = true;
    return false;
#endif
}

bool CameraService::ensureStarted() {
    if (_ready) return true;
    if (_started) return false;
    return begin();
}

bool CameraService::libraryAvailable() const {
#if WF_HAS_CAMERA
    return true;
#else
    return false;
#endif
}

bool CameraService::isReady() const { return _ready; }
bool CameraService::isAvailable() const { return libraryAvailable(); }
bool CameraService::isEnabled() const { return _enabled; }

void CameraService::setEnabled(bool enabled) {
    _enabled = enabled;
    g_cameraStatus.enabled = enabled;
}

const char* CameraService::sensorName() const { return _sensor; }
int CameraService::width() const { return _width; }
int CameraService::height() const { return _height; }
int CameraService::frameCount() const { return _frameCount; }

const uint8_t* CameraService::captureJpeg(size_t& length) {
    length = 0;
#if WF_HAS_CAMERA
    if (!_ready || !_jpegBuf) return nullptr;

    camera_fb_t* fb = esp_camera_fb_get();
    if (!fb) return nullptr;

    if (fb->format != PIXFORMAT_JPEG || fb->len == 0 || fb->len > _jpegCap) {
        esp_camera_fb_return(fb);
        return nullptr;
    }

    memcpy(_jpegBuf, fb->buf, fb->len);
    length = fb->len;
    _width = fb->width;
    _height = fb->height;
    _frameCount++;
    esp_camera_fb_return(fb);
    return _jpegBuf;
#else
    return nullptr;
#endif
}

const char* CameraService::captureBase64(size_t& length) {
    length = 0;
    size_t jpegLen = 0;
    const uint8_t* jpeg = captureJpeg(jpegLen);
    if (!jpeg || jpegLen == 0 || !_b64Buf || _b64Cap < 8) return nullptr;

    size_t n = base64Encode(jpeg, jpegLen, _b64Buf, _b64Cap - 1);
    if (n == 0) return nullptr;
    _b64Buf[n] = '\0';
    length = n;
    return _b64Buf;
}

bool CameraService::grabGray(uint8_t* out, int outW, int outH, int& srcW, int& srcH) {
#if WF_HAS_CAMERA
    srcW = 0;
    srcH = 0;
    if (!_ready || !out || outW <= 0 || outH <= 0) return false;

    camera_fb_t* fb = esp_camera_fb_get();
    if (!fb) return false;

    srcW = fb->width;
    srcH = fb->height;
    bool ok = false;

    if (fb->format == PIXFORMAT_JPEG && _rgbBuf &&
        (size_t)srcW * (size_t)srcH * 3 <= _rgbCap) {
        if (fmt2rgb888(fb->buf, fb->len, PIXFORMAT_JPEG, _rgbBuf)) {
            for (int y = 0; y < outH; y++) {
                int sy = (int)((int64_t)y * srcH / outH);
                for (int x = 0; x < outW; x++) {
                    int sx = (int)((int64_t)x * srcW / outW);
                    size_t idx = ((size_t)sy * (size_t)srcW + (size_t)sx) * 3;
                    uint8_t r = _rgbBuf[idx];
                    uint8_t g = _rgbBuf[idx + 1];
                    uint8_t b = _rgbBuf[idx + 2];
                    out[(size_t)y * (size_t)outW + (size_t)x] =
                        (uint8_t)(((uint16_t)r * 77 + (uint16_t)g * 150 + (uint16_t)b * 29) >> 8);
                }
            }
            ok = true;
        }
    }

    _width = srcW;
    _height = srcH;
    _frameCount++;
    esp_camera_fb_return(fb);
    return ok;
#else
    srcW = 0;
    srcH = 0;
    return false;
#endif
}
