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

    // Diferença contra um modelo de fundo (EMA) + connected components (4-conectados).
    // Retorna o MAIOR blob acima de min_area, com centroide. Usa bg_alpha para
    // adaptar o fundo a mudanças lentas de iluminação.
    MotionRegion detect(uint8_t* currentFrame, int width, int height,
                        int sensitivity, int minArea, float bg_alpha);

    bool hasPrevious() const;

private:
    uint8_t* _bgFrame = nullptr;   // modelo de fundo (EMA)
    uint8_t* _visited = nullptr;   // visita para connected components
    uint8_t* _changed = nullptr;   // mascara de pixels alterados
    uint16_t* _queue = nullptr;    // fila para BFS (indices cabem em uint16_t)
    int _width = 0;
    int _height = 0;
    size_t _size = 0;
    bool _hasPrevious = false;
};

extern MotionDetector motionDetector;

#endif
