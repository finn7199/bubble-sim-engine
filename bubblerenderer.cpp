#include "BubbleRenderer.h"
#include <iostream> // For std::cout


// Constructor
BubbleRenderer::BubbleRenderer(Shader& shader, GLuint bubbleTextureID)
    : shader(shader), bubbleTextureID(bubbleTextureID), quadVAO(0), quadVBO(0) {
    initRenderData();
}

// Destructor
BubbleRenderer::~BubbleRenderer() {
    // Clean up OpenGL resources
    if (quadVAO != 0) {
        glDeleteVertexArrays(1, &quadVAO);
    }
    if (quadVBO != 0) {
        glDeleteBuffers(1, &quadVBO);
    }
}

// Initializes the VAO and VBO for a unit quad.
// This quad will be translated and scaled for each bubble.
void BubbleRenderer::initRenderData() {
    // A simple quad (positions and texture coordinates).
    // The quad is centered at (0,0) and has a size of 1x1.
    // It will be scaled by bubble.radius*2 and translated to bubble.position
    // by the model matrix in the vertex shader.
    float vertices[] = {
        // positions      // texture coords
        -0.5f, -0.5f,     0.0f, 0.0f, // Bottom-left
         0.5f, -0.5f,     1.0f, 0.0f, // Bottom-right
         0.5f,  0.5f,     1.0f, 1.0f, // Top-right

        -0.5f, -0.5f,     0.0f, 0.0f, // Bottom-left
         0.5f,  0.5f,     1.0f, 1.0f, // Top-right
        -0.5f,  0.5f,     0.0f, 1.0f  // Top-left
    };

    glGenVertexArrays(1, &this->quadVAO);
    glGenBuffers(1, &this->quadVBO);

    glBindVertexArray(this->quadVAO);

    glBindBuffer(GL_ARRAY_BUFFER, this->quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Position attribute (location 0)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

    // Texture coordinate attribute (location 1)
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

// Renders all bubbles
void BubbleRenderer::renderBubbles(const std::vector<Bubble>& bubbles) {
    this->shader.use(); // Activate the shader program

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, this->bubbleTextureID);
    this->shader.setInt("bubbleTexture", 0); // Tell shader sampler to use texture unit 0

    glBindVertexArray(this->quadVAO); // Bind the quad VAO

    for (const Bubble& bubble : bubbles) {
        // Calculate model matrix for this bubble
        glm::mat4 model = glm::mat4(1.0f); // Start with identity matrix

        // 1. Translate to the bubble's position
        model = glm::translate(model, glm::vec3(bubble.position.x, bubble.position.y, 0.0f));

        // 2. Scale the unit quad by the bubble's diameter
        float diameter = bubble.radius * 2.0f;
        model = glm::scale(model, glm::vec3(diameter, diameter, 1.0f));

        // Set the model matrix uniform in the shader
        this->shader.setMat4("model", model);

        // Draw the quad (6 vertices for 2 triangles)
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}
