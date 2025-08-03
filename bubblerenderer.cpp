#include "BubbleRenderer.h"
#include <iostream> // For std::cout


// Constructor
BubbleRenderer::BubbleRenderer(Shader& bShader, Shader& sShader, GLuint bubbleTextureID)
    : bubbleShader(bShader), surfaceShader(sShader), bubbleTextureID(bubbleTextureID),
    bubbleVAO(0), bubbleVBO(0), surfaceVAO(0), surfaceVBO(0) {
    initRenderData();
}

// Destructor
BubbleRenderer::~BubbleRenderer() {
    if (bubbleVAO != 0) glDeleteVertexArrays(1, &bubbleVAO);
    if (bubbleVBO != 0) glDeleteBuffers(1, &bubbleVBO);
    if (surfaceVAO != 0) glDeleteVertexArrays(1, &surfaceVAO);
    if (surfaceVBO != 0) glDeleteBuffers(1, &surfaceVBO);
}

// Initializes the VAO and VBO for a unit quad.
// This quad will be translated and scaled for each bubble.
void BubbleRenderer::initRenderData() {
    // A simple quad (positions and texture coordinates).
    // The quad is centered at (0,0) and has a size of 1x1.
    // It will be scaled by bubble.radius*2 and translated to bubble.position
    // by the model matrix in the vertex shader.
    float bubble_vertices[] = {
        // positions      // texture coords
        -0.5f, -0.5f,     0.0f, 0.0f, // Bottom-left
         0.5f, -0.5f,     1.0f, 0.0f, // Bottom-right
         0.5f,  0.5f,     1.0f, 1.0f, // Top-right

        -0.5f, -0.5f,     0.0f, 0.0f, // Bottom-left
         0.5f,  0.5f,     1.0f, 1.0f, // Top-right
        -0.5f,  0.5f,     0.0f, 1.0f  // Top-left
    };

    glGenVertexArrays(1, &this->bubbleVAO);
    glGenBuffers(1, &this->bubbleVBO);
    glBindVertexArray(this->bubbleVAO);
    glBindBuffer(GL_ARRAY_BUFFER, this->bubbleVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(bubble_vertices), bubble_vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    // Setup for Surfaces
    glGenVertexArrays(1, &this->surfaceVAO);
    glGenBuffers(1, &this->surfaceVBO);
    glBindVertexArray(this->surfaceVAO);
    glBindBuffer(GL_ARRAY_BUFFER, this->surfaceVBO);
    // Allocate buffer for 2 points (a line segment). Data will be streamed.
    glBufferData(GL_ARRAY_BUFFER, 2 * sizeof(glm::vec2), NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

// Renders all bubbles
void BubbleRenderer::renderBubbles(const BubblePool& pool) {
    this->bubbleShader.use(); // Activate the shader program

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, this->bubbleTextureID);
    this->bubbleShader.setInt("bubbleTexture", 0); // Tell shader sampler to use texture unit 0

    glBindVertexArray(this->bubbleVAO); // Bind the VAO

    for (size_t i = 0; i < pool.active_bubble_count; ++i) {
        const Bubble& bubble = pool.bubbles[i];

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(bubble.position.x, bubble.position.y, 0.0f));
        float diameter = bubble.radius * 2.0f;
        model = glm::scale(model, glm::vec3(diameter, diameter, 1.0f));
        this->bubbleShader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

// Render surfaces
void BubbleRenderer::renderSurfaces(const std::vector<Surface2D>& surfaces, const glm::mat4& projection) {
    this->surfaceShader.use();
    this->surfaceShader.setMat4("projection", projection);

    glBindVertexArray(this->surfaceVAO);
    glLineWidth(10.0f); // Make lines thicker and more visible

    for (const auto& surface : surfaces) {
        // Update the VBO with the start and end points of the current line
        glm::vec2 vertices[] = { surface.start_point, surface.end_point };
        glBindBuffer(GL_ARRAY_BUFFER, this->surfaceVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

        // Set the color for this specific surface
        this->surfaceShader.setVec3("lineColor", surface.color);

        glDrawArrays(GL_LINES, 0, 2);
    }
    glBindVertexArray(0);
}