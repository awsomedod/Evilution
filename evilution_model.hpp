#pragma once

#include "evilution_device.hpp"
#include <cstdint>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace evilution {
class EvilutionModel {
  public:
    struct Vertex {
        glm::vec2 position;
        glm::vec3 color;

        static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
        static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
    };

    EvilutionModel(EvilutionDevice& device, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);
    ~EvilutionModel();

    EvilutionModel(const EvilutionModel&) = delete;
    EvilutionModel& operator=(const EvilutionModel&) = delete;

    void bind(VkCommandBuffer commandBuffer);
    void draw(VkCommandBuffer commandBuffer);

  private:
    void createVertexBuffers(const std::vector<Vertex>& vertices);
    void createIndexBuffers(const std::vector<uint32_t>& indices);
    EvilutionDevice& evilutionDevice;
    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;
    VkBuffer indexBuffer;
    VkDeviceMemory indexBufferMemory;
    uint32_t vertexCount;
    uint32_t indexCount;
};

} // namespace evilution
