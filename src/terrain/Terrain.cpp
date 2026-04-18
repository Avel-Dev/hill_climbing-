#include "terrain/Terrain.hpp"
#include <cmath>

namespace HillExplorer {

Terrain::Terrain(int size, float scale, std::unique_ptr<IHeightFunction> heightFunc)
    : size(size), scale(scale), heightFunc(std::move(heightFunc)) {
    heightCache.resize(size * size);
    for (int z = 0; z < size; ++z) {
        for (int x = 0; x < size; ++x) {
            heightCache[GetIndex(x, z)] = this->heightFunc->GetHeight(x, z);
        }
    }
}

float Terrain::GetHeight(int x, int z) const {
    if (!IsInBounds(x, z)) return 0.0f;
    return heightCache[GetIndex(x, z)];
}

float Terrain::GetHeightInterpolated(float x, float z) const {
    int x0 = static_cast<int>(std::floor(x));
    int z0 = static_cast<int>(std::floor(z));
    float tx = x - x0;
    float tz = z - z0;

    float h00 = GetHeight(x0, z0);
    float h10 = GetHeight(x0 + 1, z0);
    float h01 = GetHeight(x0, z0 + 1);
    float h11 = GetHeight(x0 + 1, z0 + 1);

    float h0 = h00 + (h10 - h00) * tx;
    float h1 = h01 + (h11 - h01) * tx;
    return h0 + (h1 - h0) * tz;
}

std::vector<Position2D> Terrain::GetNeighbors(int x, int z, int stepSize) const {
    std::vector<Position2D> neighbors;
    const int dirs[8][2] = {
        {0, 1}, {0, -1}, {1, 0}, {-1, 0},
        {1, 1}, {1, -1}, {-1, 1}, {-1, -1}
    };

    for (const auto& dir : dirs) {
        int nx = x + dir[0] * stepSize;
        int nz = z + dir[1] * stepSize;
        if (IsInBounds(nx, nz)) {
            neighbors.push_back({nx, nz});
        }
    }
    return neighbors;
}

Vector3 Terrain::GridToWorld(int x, int z) const {
    return {
        static_cast<float>(x) * scale,
        GetHeight(x, z),
        static_cast<float>(z) * scale
    };
}

Position2D Terrain::WorldToGrid(float x, float z) const {
    return {
        static_cast<int>(std::round(x / scale)),
        static_cast<int>(std::round(z / scale))
    };
}

float Terrain::GetMaxHeight() const {
    return heightFunc ? heightFunc->GetMaxHeight() : 0.0f;
}

float Terrain::GetMinHeight() const {
    return heightFunc ? heightFunc->GetMinHeight() : 0.0f;
}

bool Terrain::IsInBounds(int x, int z) const {
    return x >= 0 && x < size && z >= 0 && z < size;
}

int Terrain::GetIndex(int x, int z) const {
    return z * size + x;
}

} // namespace HillExplorer