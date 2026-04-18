#include "terrain/HeightFunction.hpp"
#include "utils/Math.hpp"
#include <cmath>
#include <random>

namespace HillExplorer {

MathematicalHeight::MathematicalHeight(int seed) : seed(seed) {
    std::mt19937 rng(seed);
    std::uniform_real_distribution<float> heightDist(20.0f, 50.0f);
    std::uniform_real_distribution<float> spreadDist(5.0f, 20.0f);
    std::uniform_int_distribution<int> posDist(-100, 100);

    int numPeaks = 3 + (rng() % 5);
    for (int i = 0; i < numPeaks; ++i) {
        float px = static_cast<float>(posDist(rng));
        float pz = static_cast<float>(posDist(rng));
        float h = heightDist(rng);
        float s = spreadDist(rng);
        peaks.emplace_back(px, pz, h, s);
    }
}

float MathematicalHeight::GetHeight(int x, int z) const {
    float height = 0.0f;
    float baseNoise = std::sin(x * 0.1f + seed) * std::cos(z * 0.1f + seed) * 2.0f;

    for (const auto& peak : peaks) {
        float px, pz, ph, ps;
        std::tie(px, pz, ph, ps) = peak;
        float dx = x - px;
        float dz = z - pz;
        float dist = std::sqrt(dx * dx + dz * dz);
        height += ph * std::exp(-dist * dist / (2 * ps * ps));
    }

    return height + baseNoise + 5.0f;
}

PerlinHeight::PerlinHeight(int seed, int octaves, float persistence)
    : noise(seed), octaves(octaves), persistence(persistence), scale(0.03f) {}

float PerlinHeight::GetHeight(int x, int z) const {
    float n = noise.OctaveNoise(x * scale, z * scale, octaves, persistence);
    return (n + 1.0f) * 0.5f * 40.0f + 2.0f;
}

float PerlinHeight::GetMaxHeight() const {
    if (!cached) ComputeBounds();
    return cachedMax;
}

float PerlinHeight::GetMinHeight() const {
    if (!cached) ComputeBounds();
    return cachedMin;
}

void PerlinHeight::ComputeBounds(int sampleSize) const {
    cachedMin = std::numeric_limits<float>::max();
    cachedMax = std::numeric_limits<float>::lowest();

    for (int i = 0; i < sampleSize; ++i) {
        for (int j = 0; j < sampleSize; ++j) {
            float h = GetHeight(i, j);
            cachedMin = std::min(cachedMin, h);
            cachedMax = std::max(cachedMax, h);
        }
    }
    cached = true;
}

} // namespace HillExplorer