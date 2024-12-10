#include "marching_squares.hpp"

#include <memory>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace evilution {

std::shared_ptr<EvilutionModel> MarchingSquares::marchingSquares(std::function<float(float, float)> implicitFn) {

    std::vector<EvilutionModel::Vertex> vertices;
    std::vector<uint32_t> indices;

    // Sample the implicit function - MODIFIED to match Vulkan coordinates
    std::vector<std::vector<float>> field(samplesX + 1, std::vector<float>(samplesY + 1));
    std::vector<std::vector<glm::vec2>> positions(samplesX + 1, std::vector<glm::vec2>(samplesY + 1));
    for (int x = 0; x <= samplesX; x++) {
        for (int y = 0; y <= samplesY; y++) {
            float nx = static_cast<float>(x) / samplesX * 2.0f - 1.0f;    // [-1,1] left to right
            float ny = -(static_cast<float>(y) / samplesY * 2.0f - 1.0f); // [1,-1] bottom to top
            field[x][y] = implicitFn(nx, ny);
            positions[x][y] = {nx, ny}; // Store the normalized position
        }
    }

    // Process each cell using precomputed positions
    for (int x = 0; x < samplesX; x++) {
        for (int y = 0; y < samplesY; y++) {
            // Get cell corner values
            float v00 = field[x][y];
            float v10 = field[x + 1][y];
            float v11 = field[x + 1][y + 1];
            float v01 = field[x][y + 1];

            // Use precomputed positions
            glm::vec2 p00 = positions[x][y];
            glm::vec2 p10 = positions[x + 1][y];
            glm::vec2 p11 = positions[x + 1][y + 1];
            glm::vec2 p01 = positions[x][y + 1];

            float x0 = p00.x;
            float x1 = p10.x;
            float y0 = p00.y;
            float y1 = p01.y;

            // can delete after testing
            assert(x0 == p01.x);
            assert(x1 == p11.x);
            assert(y0 == p10.y);
            assert(y1 == p11.y);

            // Calculate case index using threshold
            int caseIndex = 0;
            if (v00 > threshold)
                caseIndex |= 1;
            if (v10 > threshold)
                caseIndex |= 2;
            if (v11 > threshold)
                caseIndex |= 4;
            if (v01 > threshold)
                caseIndex |= 8;

            // Skip empty cells
            if (caseIndex == 0 || caseIndex == 15)
                continue;

            // Linear interpolation helper
            auto lerp = [this](float a, float b, float v0, float v1) {
                return a + (b - a) * (threshold - v0) / (v1 - v0);
            };

            // Calculate intersection points
            glm::vec2 points[4];
            if ((v00 - threshold) * (v10 - threshold) < 0.0f)
                points[0] = {lerp(x0, x1, v00, v10), y0}; // Bottom edge
            if ((v10 - threshold) * (v11 - threshold) < 0.0f)
                points[1] = {x1, lerp(y0, y1, v10, v11)}; // Right edge
            if ((v01 - threshold) * (v11 - threshold) < 0.0f)
                points[2] = {lerp(x0, x1, v01, v11), y1}; // Top edge
            if ((v00 - threshold) * (v01 - threshold) < 0.0f)
                points[3] = {x0, lerp(y0, y1, v00, v01)}; // Left edge

            // Add line segments based on case
            switch (caseIndex) {
            case 1:
            case 14:
                addLineSegment(vertices, indices, points[0], points[3]);
                break;
            case 2:
            case 13:
                addLineSegment(vertices, indices, points[0], points[1]);
                break;
            case 3:
            case 12:
                addLineSegment(vertices, indices, points[1], points[3]);
                break;
            case 4:
            case 11:
                addLineSegment(vertices, indices, points[1], points[2]);
                break;
            case 6:
            case 9:
                addLineSegment(vertices, indices, points[0], points[2]);
                break;
            case 7:
            case 8:
                addLineSegment(vertices, indices, points[2], points[3]);
                break;
            case 5: {
                // Ambiguous case - use center point to determine connectivity
                float centerValue = implicitFn((x0 + x1) * 0.5f, (y0 + y1) * 0.5f);
                if (centerValue > threshold) {
                    addLineSegment(vertices, indices, points[0], points[3]);
                    addLineSegment(vertices, indices, points[1], points[2]);
                } else {
                    addLineSegment(vertices, indices, points[0], points[1]);
                    addLineSegment(vertices, indices, points[2], points[3]);
                }
                break;
            }
            case 10: {
                // Ambiguous case - use center point to determine connectivity
                float centerValue = implicitFn((x0 + x1) * 0.5f, (y0 + y1) * 0.5f);
                if (centerValue > threshold) {
                    addLineSegment(vertices, indices, points[0], points[1]);
                    addLineSegment(vertices, indices, points[2], points[3]);
                } else {
                    addLineSegment(vertices, indices, points[0], points[3]);
                    addLineSegment(vertices, indices, points[1], points[2]);
                }
                break;
            }
            }
        }
    }

    return std::make_shared<EvilutionModel>(device, vertices, indices);
}

void MarchingSquares::addLineSegment(std::vector<EvilutionModel::Vertex>& vertices, std::vector<uint32_t>& indices,
                                     const glm::vec2& p1, const glm::vec2& p2) {
    uint32_t index = static_cast<uint32_t>(vertices.size());
    vertices.push_back({p1});
    vertices.push_back({p2});
    indices.push_back(index);
    indices.push_back(index + 1);
}
} // namespace evilution