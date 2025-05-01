#include "MathUtils.h"
#include <algorithm>

float MathUtils::clamp(float value, float min, float max) {
    return std::max(min, std::min(max, value));
}

float MathUtils::map(float value, float inMin, float inMax, float outMin, float outMax) {
    return outMin + (value - inMin) * (outMax - outMin) / (inMax - inMin);
}