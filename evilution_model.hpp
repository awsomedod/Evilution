#pragma once

#include "evilution_device.hpp"
#include <cstdint>
#include <vector>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace evilution {
class EvilutionModel {
  public:
    struct Vertex {
        glm::vec3 position;
        glm::vec3 color;

        static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
        static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
    };


    struct Builder {
        std::vector<Vertex> vertices{};
        std::vector<uint32_t> indices{};

        void loadModel(const std::string& filepath);
    };

    EvilutionModel(EvilutionDevice& device, const Builder& builder);
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

    bool hasIndexBuffer = false;
    VkDeviceMemory indexBufferMemory;
    uint32_t vertexCount;
    uint32_t indexCount;
};

} // namespace evilution
