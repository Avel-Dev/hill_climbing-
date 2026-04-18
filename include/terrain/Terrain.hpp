#pragma once
#include "common/Types.hpp"
#include "terrain/HeightFunction.hpp"
#include <vector>
#include <memory>

namespace HillExplorer {

class Terrain {
public:
    Terrain(int size, float scale, std::unique_ptr<IHeightFunction> heightFunc);

    float GetHeight(int x, int z) const;
    float GetHeightInterpolated(float x, float z) const;
    std::vector<Position2D> GetNeighbors(int x, int z, int stepSize = 1) const;

    Vector3 GridToWorld(int x, int z) const;
    Position2D WorldToGrid(float x, float z) const;

    int GetSize() const { return size; }
    float GetScale() const { return scale; }
    float GetMaxHeight() const;
    float GetMinHeight() const;
    const IHeightFunction* GetHeightFunction() const { return heightFunc.get(); }
    const std::vector<float>& GetHeightCache() const { return heightCache; }

private:
    int size;
    float scale;
    std::unique_ptr<IHeightFunction> heightFunc;
    std::vector<float> heightCache;

    bool IsInBounds(int x, int z) const;
    int GetIndex(int x, int z) const;
};

} // namespace HillExplorer