#include "utils/PerlinNoise.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace HillExplorer {

PerlinNoise::PerlinNoise(int seed) {
    p.resize(512);
    for (int i = 0; i < 256; ++i) {
        p[i] = i;
    }

    std::mt19937 engine(seed);
    std::shuffle(p.begin(), p.begin() + 256, engine);

    for (int i = 0; i < 256; ++i) {
        p[i + 256] = p[i];
    }
}

float PerlinNoise::Noise(float x, float y) const {
    int X = static_cast<int>(std::floor(x)) & 255;
    int Y = static_cast<int>(std::floor(y)) & 255;

    x -= std::floor(x);
    y -= std::floor(y);

    float u = Fade(x);
    float v = Fade(y);

    int A = p[X] + Y;
    int B = p[X + 1] + Y;

    return Lerp(v,
                Lerp(u, Grad(p[A], x, y), Grad(p[B], x - 1, y)),
                Lerp(u, Grad(p[A + 1], x, y - 1), Grad(p[B + 1], x - 1, y - 1)));
}

float PerlinNoise::OctaveNoise(float x, float y, int octaves, float persistence) const {
    float total = 0.0f;
    float frequency = 1.0f;
    float amplitude = 1.0f;
    float maxValue = 0.0f;

    for (int i = 0; i < octaves; ++i) {
        total += Noise(x * frequency, y * frequency) * amplitude;
        maxValue += amplitude;
        amplitude *= persistence;
        frequency *= 2.0f;
    }

    return total / maxValue;
}

float PerlinNoise::Fade(float t) {
    return t * t * t * (t * (t * 6 - 15) + 10);
}

float PerlinNoise::Lerp(float t, float a, float b) {
    return a + t * (b - a);
}

float PerlinNoise::Grad(int hash, float x, float y) {
    int h = hash & 15;
    float u = h < 8 ? x : y;
    float v = h < 4 ? y : (h == 12 || h == 14 ? x : 0);
    return ((h & 1) ? -u : u) + ((h & 2) ? -v : v);
}

} // namespace HillExplorer