#pragma once
#include <vector>

namespace HillExplorer {

class PerlinNoise {
public:
    explicit PerlinNoise(int seed = 0);

    float Noise(float x, float y) const;
    float OctaveNoise(float x, float y, int octaves, float persistence) const;

private:
    std::vector<int> p;

    static float Fade(float t);
    static float Lerp(float t, float a, float b);
    static float Grad(int hash, float x, float y);
};

} // namespace HillExplorer