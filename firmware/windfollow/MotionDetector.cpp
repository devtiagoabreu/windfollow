#include "MotionDetector.h"

bool MotionDetector::begin(int width, int height) {
    _width = width;
    _height = height;
    _size = (size_t)width * (size_t)height;

    free(_bgFrame);
    free(_visited);
    free(_changed);
    free(_queue);

    _bgFrame = (uint8_t*)malloc(_size);
    _visited = (uint8_t*)malloc(_size);
    _changed = (uint8_t*)malloc(_size);
    _queue = (uint16_t*)malloc(_size * sizeof(uint16_t));

    if (_bgFrame && _visited && _changed && _queue) {
        memset(_bgFrame, 0, _size);
        memset(_visited, 0, _size);
        memset(_changed, 0, _size);
        _hasPrevious = false;
        return true;
    }
    _hasPrevious = false;
    return false;
}

MotionRegion MotionDetector::detect(uint8_t* currentFrame, int width, int height,
                                    int sensitivity, int minArea, float bg_alpha) {
    MotionRegion result;
    result.valid = false;

    if (!currentFrame || width <= 0 || height <= 0) return result;

    size_t size = (size_t)width * (size_t)height;
    if (!_bgFrame || _size != size) return result;

    // Primeiro frame: apenas inicializa o modelo de fundo
    if (!_hasPrevious) {
        memcpy(_bgFrame, currentFrame, size);
        _hasPrevious = true;
        return result;
    }

    const uint8_t* bg = _bgFrame;
    uint8_t* changed = _changed;
    uint8_t* visited = _visited;
    for (size_t i = 0; i < size; i++) visited[i] = 0;

    // Diferença contra o modelo de fundo (EMA), não contra o quadro anterior.
    // Ignora mudanças lentas de iluminação (nuvens, lâmpadas).
    for (size_t i = 0; i < size; i++) {
        int d = (int)currentFrame[i] - (int)bg[i];
        int diff = d < 0 ? -d : d;
        changed[i] = (diff > sensitivity) ? 1 : 0;
    }

    // Connected components (4-conectados): fica com o MAIOR blob, ignorando
    // ruído disperso e mudanças pontuais de fundo.
    int bestArea = 0;
    int bestMinX = width, bestMinY = height, bestMaxX = 0, bestMaxY = 0;
    uint16_t* queue = _queue;

    for (int seed = 0; seed < (int)size; seed++) {
        if (!changed[seed] || visited[seed]) continue;

        int area = 0, minX = width, minY = height, maxX = 0, maxY = 0;
        int qhead = 0, qtail = 0;
        queue[qtail++] = (uint16_t)seed;
        visited[seed] = 1;

        while (qhead < qtail) {
            int ci = queue[qhead++];
            int cx = ci % width;
            int cy = ci / width;
            area++;
            if (cx < minX) minX = cx;
            if (cx > maxX) maxX = cx;
            if (cy < minY) minY = cy;
            if (cy > maxY) maxY = cy;

            // 4 vizinhos (sem wrap-around nas bordas)
            if (cx > 0) {
                int ni = ci - 1;
                if (changed[ni] && !visited[ni]) { visited[ni] = 1; queue[qtail++] = (uint16_t)ni; }
            }
            if (cx < width - 1) {
                int ni = ci + 1;
                if (changed[ni] && !visited[ni]) { visited[ni] = 1; queue[qtail++] = (uint16_t)ni; }
            }
            if (cy > 0) {
                int ni = ci - width;
                if (changed[ni] && !visited[ni]) { visited[ni] = 1; queue[qtail++] = (uint16_t)ni; }
            }
            if (cy < height - 1) {
                int ni = ci + width;
                if (changed[ni] && !visited[ni]) { visited[ni] = 1; queue[qtail++] = (uint16_t)ni; }
            }
        }

        if (area > bestArea) {
            bestArea = area;
            bestMinX = minX; bestMinY = minY; bestMaxX = maxX; bestMaxY = maxY;
        }
    }

    if (bestArea < minArea) {
        // Ainda atualiza o fundo para se adaptar à iluminação
        for (size_t i = 0; i < size; i++) {
            _bgFrame[i] = (uint8_t)((bg_alpha * (float)_bgFrame[i] +
                                     (1.0f - bg_alpha) * (float)currentFrame[i]) + 0.5f);
        }
        return result;
    }

    int boxW = bestMaxX - bestMinX + 1;
    int boxH = bestMaxY - bestMinY + 1;

    // Adapta o fundo à iluminação lenta (EMA)
    for (size_t i = 0; i < size; i++) {
        _bgFrame[i] = (uint8_t)((bg_alpha * (float)_bgFrame[i] +
                                 (1.0f - bg_alpha) * (float)currentFrame[i]) + 0.5f);
    }

    result.x = bestMinX;
    result.y = bestMinY;
    result.width = boxW;
    result.height = boxH;
    result.area = bestArea;
    result.centerX = bestMinX + boxW / 2;
    result.centerY = bestMinY + boxH / 2;
    result.valid = true;

    return result;
}

bool MotionDetector::hasPrevious() const {
    return _hasPrevious;
}
