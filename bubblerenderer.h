#pragma once
#ifndef BUBBLE_RENDERER_H
#define BUBBLE_RENDERER_H

#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>              // For glm::mat4, glm::vec3
#include <glm/gtc/matrix_transform.hpp> // For glm::translate, glm::scale
#include "Shader.h" // Your updated Shader class
#include "Bubble.h"

// Renders a collection of bubbles as textured quads.
class BubbleRenderer {
public:
    // Constructor:
    //   shader: The shader program to use for rendering bubbles.
    //   bubbleTextureID: The OpenGL texture ID for the bubble image.
    BubbleRenderer(Shader& shader, GLuint bubbleTextureID);

    // Destructor to clean up OpenGL resources.
    ~BubbleRenderer();

    // Renders all bubbles in the provided vector.
    //   bubbles: A vector of Bubble objects to render.
    //   projection: The orthographic projection matrix.
    //               (The shader should already have this set from main)
    void renderBubbles(const std::vector<Bubble>& bubbles);

private:
    Shader& shader;              // Reference to the shader program.
    GLuint bubbleTextureID;      // Texture ID for the bubbles.
    GLuint quadVAO;              // Vertex Array Object for the quad used to draw bubbles.
    GLuint quadVBO;              // Vertex Buffer Object for the quad.

    // Initializes the VAO and VBO for a unit quad.
    void initRenderData();
};

#endif // BUBBLE_RENDERER_H