#ifndef MOTION_DETECTOR_H
#define MOTION_DETECTOR_H

#include <Arduino.h>

struct MotionRegion {
    int x = 0;
    int y = 0;
    int width = 0;
    int height = 0;
    int area = 0;
    int centerX = 0;
    int centerY = 0;
    bool valid = false;
};

class MotionDetector {
public:
    bool begin(int width, int height);
    MotionRegion detect(uint8_t* currentFrame, uint8_t* previousFrame, int width, int height, int sensitivity, int minArea);
    bool hasPrevious() const;

private:
    uint8_t* _previousFrame = nullptr;
    int _prevWidth = 0;
    int _prevHeight = 0;
    bool _hasPrevious = false;
};

extern MotionDetector motionDetector;

#endif
