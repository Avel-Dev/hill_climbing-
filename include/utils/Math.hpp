#pragma once
#include <cmath>

namespace HillExplorer {

inline float Lerp(float a, float b, float t) {
    return a + t * (b - a);
}

inline float Clamp(float value, float min, float max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

inline float SmoothStep(float edge0, float edge1, float x) {
    float t = Clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
    return t * t * (3.0f - 2.0f * t);
}

} // namespace HillExplorer