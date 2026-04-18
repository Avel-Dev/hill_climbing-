#pragma once
#include "common/Types.hpp"
#include "utils/PerlinNoise.hpp"
#include <memory>
#include <vector>
#include <tuple>

namespace HillExplorer {

class IHeightFunction {
public:
    virtual ~IHeightFunction() = default;
    virtual float GetHeight(int x, int z) const = 0;
    virtual float GetMaxHeight() const = 0;
    virtual float GetMinHeight() const = 0;
    virtual std::string GetName() const = 0;
};

class MathematicalHeight : public IHeightFunction {
public:
    explicit MathematicalHeight(int seed = 0);
    float GetHeight(int x, int z) const override;
    float GetMaxHeight() const override { return maxHeight; }
    float GetMinHeight() const override { return minHeight; }
    std::string GetName() const override { return "Mathematical"; }

private:
    int seed;
    float maxHeight = 50.0f;
    float minHeight = 0.0f;
    std::vector<std::tuple<float, float, float, float>> peaks;
};

class PerlinHeight : public IHeightFunction {
public:
    PerlinHeight(int seed = 0, int octaves = 4, float persistence = 0.5f);
    float GetHeight(int x, int z) const override;
    float GetMaxHeight() const override;
    float GetMinHeight() const override;
    std::string GetName() const override { return "Perlin"; }

private:
    PerlinNoise noise;
    int octaves;
    float persistence;
    float scale;
    mutable float cachedMin = 0.0f;
    mutable float cachedMax = 30.0f;
    mutable bool cached = false;

    void ComputeBounds(int sampleSize = 200) const;
};

} // namespace HillExplorer