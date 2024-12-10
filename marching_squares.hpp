#pragma once

#include "evilution_model.hpp"

#include <functional>
#include <memory>

namespace evilution {
class MarchingSquares {
  public:

    MarchingSquares(EvilutionDevice& device, float threshold, int samplesX, int samplesY)
        : device{device}, threshold{threshold}, samplesX{samplesX}, samplesY{samplesY} {}

    
    std::shared_ptr<EvilutionModel> marchingSquares(std::function<float(float, float)> implicitFn);

  private:
    void addLineSegment(std::vector<EvilutionModel::Vertex>& vertices, std::vector<uint32_t>& indices,
                               const glm::vec2& p1, const glm::vec2& p2);

    EvilutionDevice& device;
    float threshold;
    int samplesX;
    int samplesY;
};
} // namespace evilution