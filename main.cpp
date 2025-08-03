#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h" 
#include <iostream>
#include <vector>
#include <chrono>

// Simulation components
#include "Bubble.h"
#include "TextureManager.h"
#include "BubbleRenderer.h"
#include "BubbleGenerator.h"
#include "BubbleSimulator.h"
#include "Surface2D.h" 
#include "SimulationConstants.h"
#include "BubblePool.h"
#define GLM_ENABLE_EXPERIMENTAL

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

glm::mat4 projection;
BubbleSimulator* simulator_ptr = nullptr;

double lastUpdateTime = 0.0;
double lastRenderTime = 0.0;

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Bubble Simulation with Surfaces & Better Pooling", NULL, NULL);
    if (window == NULL) { return -1; }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    //glfwSwapInterval(1); //VSYNC
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) { return -1; }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT); // Set initial viewport

    Shader bubbleShader("vertex.vs", "fragment.frag");
    Shader surfaceShader("surface.vs", "surface.frag"); // Shader for surfaces
    GLuint bubbleTexID = TextureManager::loadTexture("bubble.png", true);
    if (bubbleTexID == 0) { return -1; }

    BubblePool bubblePool;
    BubbleRenderer renderer(bubbleShader, surfaceShader, bubbleTexID);
    BubbleGenerator generator;
    BubbleSimulator simulator(SCR_WIDTH, SCR_HEIGHT);
    simulator_ptr = &simulator; // For callback if needed to update fluid grid size

    // --- Define Surfaces with Different Properties and Colors ---
    // Glass container (low adhesion)
    Surface2D bottom_wall(0, glm::vec2(10, 10), glm::vec2(SCR_WIDTH - 10, 10), GLASS_STATIC_ADHESION, GLASS_DYNAMIC_ADHESION, true, glm::vec3(0.7f, 0.7f, 0.75f));
    Surface2D left_wall(1, glm::vec2(10, 10), glm::vec2(10, SCR_HEIGHT - 10), GLASS_STATIC_ADHESION, GLASS_DYNAMIC_ADHESION, false, glm::vec3(0.7f, 0.7f, 0.75f));
    Surface2D right_wall(2, glm::vec2(SCR_WIDTH - 10, 10), glm::vec2(SCR_WIDTH - 10, SCR_HEIGHT - 10), GLASS_STATIC_ADHESION, GLASS_DYNAMIC_ADHESION, false, glm::vec3(0.7f, 0.7f, 0.75f));

    // Plastic Stirrer (medium adhesion)
    Surface2D plastic_stirrer(3, glm::vec2(200, 100), glm::vec2(220, 450), PLASTIC_STATIC_ADHESION, PLASTIC_DYNAMIC_ADHESION, true, glm::vec3(0.9f, 0.9f, 0.85f));

    // Wooden Spoon (high adhesion)
    Surface2D wooden_spoon(4, glm::vec2(550, 100), glm::vec2(500, 450), WOOD_STATIC_ADHESION, WOOD_DYNAMIC_ADHESION, true, glm::vec3(0.6f, 0.4f, 0.2f));

    // Add all surfaces to the simulator
    simulator.addSurface(bottom_wall);
    simulator.addSurface(left_wall);
    simulator.addSurface(right_wall);
    simulator.addSurface(plastic_stirrer);
    simulator.addSurface(wooden_spoon);

    projection = glm::ortho(0.0f, static_cast<float>(SCR_WIDTH), 0.0f, static_cast<float>(SCR_HEIGHT), -1.0f, 1.0f);

    double lastTime = glfwGetTime();
    double lastFpsTime = lastTime; // Separate timer for FPS

    // For frame time, FPS, render time
    const int FRAME_HISTORY_SIZE = 60;
    float frameTimes[FRAME_HISTORY_SIZE] = { 0 };
    int frameTimeIndex = 0;
    double totalFrameTime = 0.0;
    double totalSimTime = 0.0;
    double totalRenderTime = 0.0;
    int totalFrames = 0;


    while (!glfwWindowShouldClose(window)) {

        // --- Frame Timing ---
        double currentTime = glfwGetTime();
        float dt = static_cast<float>(currentTime - lastTime);
        if (dt == 0.0f) dt = 1.0f / 60.0f; // Avoid dt = 0
        lastTime = currentTime;

        // Store frame time for averaging (optional)
        frameTimes[frameTimeIndex] = dt * 1000.0f; // Convert to milliseconds
        frameTimeIndex = (frameTimeIndex + 1) % FRAME_HISTORY_SIZE;
        totalFrameTime += dt * 1000.0;       // in milliseconds
        totalSimTime += lastUpdateTime * 1000.0;
        totalRenderTime += lastRenderTime * 1000.0;
        totalFrames++;

        // --- Reporting ---
        if (currentTime - lastFpsTime >= 1.0) {
            double avgFrameTime = totalFrameTime / totalFrames;
            double avgSimTime = totalSimTime / totalFrames;
            double avgRenderTime = totalRenderTime / totalFrames;
            system("cls");
            printf("--- Averages (since start, updated every 1 sec) ---\n");
            printf("Frames: %d\n", totalFrames);
            printf("FPS: %.1f  |  Frame Time: %.2f ms\n", 1000.0 / avgFrameTime, avgFrameTime);
            printf("-> Simulation: %.2f ms (%.1f%%)\n", avgSimTime, (avgSimTime / avgFrameTime) * 100.0);
            printf("-> Rendering:  %.2f ms (%.1f%%)\n", avgRenderTime, (avgRenderTime / avgFrameTime) * 100.0);
            printf("-> Other/Overhead: %.2f ms\n", avgFrameTime - avgSimTime - avgRenderTime);

            lastFpsTime = currentTime;
        }

        auto inputStart = glfwGetTime();
        processInput(window);
        float inputTime = glfwGetTime() - inputStart;

        // --- Simulation ---
        auto simStart = glfwGetTime();
        generator.tryGenerateBubbles(bubblePool, simulator.getSurfaces(), dt, static_cast<float>(SCR_WIDTH), static_cast<float>(SCR_HEIGHT));
        simulator.update(dt, bubblePool);

        lastUpdateTime = glfwGetTime() - simStart;

        // --- Rendering ---
        auto renderStart = glfwGetTime();
        bubbleShader.use();
        bubbleShader.setMat4("projection", projection);

        glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Render the surfaces first(so they are behind the bubbles)
        renderer.renderSurfaces(simulator.getSurfaces(), projection);

        // Render the bubbles
        renderer.renderBubbles(bubblePool);
        lastRenderTime = glfwGetTime() - renderStart;

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    if (width == 0 || height == 0) return; // Avoid division by zero if window minimized
    glViewport(0, 0, width, height);
    projection = glm::ortho(0.0f, static_cast<float>(width),
        0.0f, static_cast<float>(height),
        -1.0f, 1.0f);
    // If simulator or fluid grid needs to be resized based on new screen dimensions:
    // if (simulator_ptr) {
    //    simulator_ptr->getFluidGrid() = FluidGrid2D(width, height); // Recreate or resize
    // }
}
