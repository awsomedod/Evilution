#include "evilution_model.hpp"
#include "vulkan/vulkan_core.h"
#include <vector>

namespace evilution {

EvilutionModel::EvilutionModel(EvilutionDevice& device, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices)
    : evilutionDevice{device} {
    createVertexBuffers(vertices);
    createIndexBuffers(indices);
}

EvilutionModel::~EvilutionModel() {
    vkDestroyBuffer(evilutionDevice.device(), vertexBuffer, nullptr);
    vkFreeMemory(evilutionDevice.device(), vertexBufferMemory, nullptr);

    vkDestroyBuffer(evilutionDevice.device(), indexBuffer, nullptr);
    vkFreeMemory(evilutionDevice.device(), indexBufferMemory, nullptr);
}

void EvilutionModel::createIndexBuffers(const std::vector<uint32_t>& indices) {
    indexCount = static_cast<uint32_t>(indices.size());
    assert(indexCount % 2 == 0 && "Index count must be a multiple of 2 for line rendering");
    VkDeviceSize bufferSize = sizeof(indices[0]) * indexCount;

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    evilutionDevice.createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                 stagingBuffer, stagingBufferMemory);

    void* data;
    vkMapMemory(evilutionDevice.device(), stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, indices.data(), static_cast<size_t>(bufferSize));
    vkUnmapMemory(evilutionDevice.device(), stagingBufferMemory);

    evilutionDevice.createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer, indexBufferMemory);

    evilutionDevice.copyBuffer(stagingBuffer, indexBuffer, bufferSize);

    vkDestroyBuffer(evilutionDevice.device(), stagingBuffer, nullptr);
    vkFreeMemory(evilutionDevice.device(), stagingBufferMemory, nullptr);
}

void EvilutionModel::createVertexBuffers(const std::vector<Vertex>& vertices) {
    vertexCount = static_cast<uint32_t>(vertices.size());
    assert(vertexCount >= 2 && "Vertex count must be at least 2 for line rendering");
    VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount;

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    evilutionDevice.createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                 stagingBuffer, stagingBufferMemory);

    void* data;
    vkMapMemory(evilutionDevice.device(), stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, vertices.data(), static_cast<size_t>(bufferSize));
    vkUnmapMemory(evilutionDevice.device(), stagingBufferMemory);

    evilutionDevice.createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer, vertexBufferMemory);

    evilutionDevice.copyBuffer(stagingBuffer, vertexBuffer, bufferSize);

    vkDestroyBuffer(evilutionDevice.device(), stagingBuffer, nullptr);
    vkFreeMemory(evilutionDevice.device(), stagingBufferMemory, nullptr);
}

void EvilutionModel::draw(VkCommandBuffer commandBuffer) { vkCmdDrawIndexed(commandBuffer, vertexCount, 1, 0, 0, 0); }

void EvilutionModel::bind(VkCommandBuffer commandBuffer) {
    VkBuffer buffers[] = {vertexBuffer};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
    vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT32);
}

std::vector<VkVertexInputBindingDescription> EvilutionModel::Vertex::getBindingDescriptions() {
    std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
    bindingDescriptions[0].binding = 0;
    bindingDescriptions[0].stride = sizeof(Vertex);
    bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    return bindingDescriptions;
}

std::vector<VkVertexInputAttributeDescription> EvilutionModel::Vertex::getAttributeDescriptions() {
    std::vector<VkVertexInputAttributeDescription> attributeDescriptions(2);
    attributeDescriptions[0].binding = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[0].offset = offsetof(Vertex, position);

    attributeDescriptions[1].binding = 0;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[1].offset = offsetof(Vertex, color);

    return attributeDescriptions;
}

} // namespace evilution
