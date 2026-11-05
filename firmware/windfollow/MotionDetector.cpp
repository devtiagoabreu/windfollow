#include "MotionDetector.h"

MotionDetector motionDetector;

bool MotionDetector::begin(int width, int height) {
    _prevWidth = width;
    _prevHeight = height;
    size_t size = (size_t)width * (size_t)height;
    _previousFrame = (uint8_t*)malloc(size);
    if (_previousFrame) {
        memset(_previousFrame, 0, size);
        _hasPrevious = true;
    }
    return _previousFrame != nullptr;
}

MotionRegion MotionDetector::detect(uint8_t* currentFrame, uint8_t* previousFrame, int width, int height, int sensitivity, int minArea) {
    MotionRegion result;
    result.valid = false;

    if (!currentFrame || !previousFrame || width <= 0 || height <= 0) {
        return result;
    }

    size_t size = (size_t)width * (size_t)height;
    int maxArea = 0;
    int bestX = 0, bestY = 0, bestW = 0, bestH = 0;

    // Simple threshold difference + bounding box of changed pixels
    int minX = width, minY = height, maxX = 0, maxY = 0;
    int changedCount = 0;

    for (size_t i = 0; i < size; i++) {
        int diff = abs((int)currentFrame[i] - (int)previousFrame[i]);
        if (diff > sensitivity) {
            changedCount++;
            int px = (int)(i % width);
            int py = (int)(i / width);
            if (px < minX) minX = px;
            if (px > maxX) maxX = px;
            if (py < minY) minY = py;
            if (py > maxY) maxY = py;
        }
    }

    if (changedCount < minArea) {
        return result;
    }

    int w = maxX - minX + 1;
    int h = maxY - minY + 1;
    int area = w * h;

    result.x = minX;
    result.y = minY;
    result.width = w;
    result.height = h;
    result.area = changedCount;
    result.centerX = minX + w / 2;
    result.centerY = minY + h / 2;
    result.valid = true;

    return result;
}

bool MotionDetector::hasPrevious() const {
    return _hasPrevious;
}
