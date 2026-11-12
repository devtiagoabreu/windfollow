#ifndef SMARTCAM_SERVICE_H
#define SMARTCAM_SERVICE_H

#include <Arduino.h>

class SmartCamService {
public:
    virtual bool init() { return true; }
    virtual void tick() {}
    virtual bool deinit() { return true; }
    virtual bool isRunning() const { return false; }
    virtual const char* name() const { return "SmartCamService"; }
    virtual ~SmartCamService() = default;
};

#endif
