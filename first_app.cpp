#include "first_app.hpp"
#include "meta_ball_system.hpp"
#include "simple_render_system.hpp"



#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// std
#include <chrono>
#include <random>

namespace evilution {

FirstApp::FirstApp() { loadGameObjects(); }

FirstApp::~FirstApp() {}

void FirstApp::run() {
    SimpleRenderSystem simpleRenderSystem{evilutionDevice, evilutionRenderer.getSwapChainRenderPass()};
    
    float deltaTime = 0.0f;
    auto currentTime = std::chrono::high_resolution_clock::now();
    
    // Sample rate oscillation parameters
    float totalTime = 0.0f;
    const float cycleDuration = 60.0f;  // Complete cycle takes 10 seconds
    const float minSampleRate = 5.0f;
    const float maxSampleRate = 100.0f;
    const float baseRate = (minSampleRate + maxSampleRate) / 2.0f;
    const float amplitude = (maxSampleRate - minSampleRate) / 2.0f;

    while (!evilutionWindow.shouldClose()) {
        auto newTime = std::chrono::high_resolution_clock::now();
        deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
        currentTime = newTime;

        // Update total time
        totalTime += deltaTime;
        
        // Calculate current sample rate using asymmetric sine wave
        float phase = (totalTime / cycleDuration) * 2.0f * glm::pi<float>();
        float sineValue = std::sin(phase);
        float asymmetricFactor = std::pow(std::abs(sineValue), 0.5f) * (sineValue >= 0 ? 1.0f : -1.0f);
        int currentSampleRate = static_cast<int>(baseRate + amplitude * asymmetricFactor);
        
        // Update marching squares parameters with new sample rate
        mbSystem.updateMarchingSquaresParameters(1, currentSampleRate, currentSampleRate);

        glfwPollEvents();

        if (auto commandBuffer = evilutionRenderer.beginFrame()) {
            // Update and render
            mbSystem.updateMetaBalls(deltaTime);
            
            evilutionRenderer.beginSwapChainRenderPass(commandBuffer);
            simpleRenderSystem.renderGameObjects(commandBuffer, evilutionRegistry);
            evilutionRenderer.endSwapChainRenderPass(commandBuffer);
            evilutionRenderer.endFrame();
        }
    }

    vkDeviceWaitIdle(evilutionDevice.device());
}

void FirstApp::loadGameObjects() {

    std::random_device rd;
    std::mt19937 gen(rd());

    // Define ranges for random centers and velocities
    std::uniform_real_distribution<float> centerDist(-0.5f, 0.5f); // Random centers in range [-0.5, 0.5]
    std::uniform_real_distribution<float> velocityDist(-0.2f, 0.2f); // Random velocities in range [-0.2, 0.2]

    // Radii for the metaballs (keeping these the same as in the original code)
    std::vector<float> radii = {0.06f, 0.07f, 0.1f, 0.12f, 0.1f};

    for (float radius : radii) {
        // Generate random center and velocity for each metaball
        glm::vec2 randomCenter = {centerDist(gen), centerDist(gen)};
        glm::vec2 randomVelocity = {velocityDist(gen), velocityDist(gen)};

        // Add the metaball to the system
        mbSystem.addMetaBall(randomCenter, randomVelocity, radius);
    }
    mbSystem.createInitialMesh();

}
} // namespace evilution