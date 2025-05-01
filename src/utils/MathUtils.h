#pragma once

class MathUtils {
public:
    static float clamp(float value, float min, float max);
    static float map(float value, float inMin, float inMax, float outMin, float outMax);
};