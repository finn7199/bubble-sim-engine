#pragma once
#ifndef BUBBLE_RENDERER_H
#define BUBBLE_RENDERER_H

#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Shader.h"
#include "Bubble.h"
#include "Surface2D.h"
#include "BubblePool.h"

// Renders a collection of bubbles as textured quads.
class BubbleRenderer {
public:
    // Constructor takes shaders for both bubbles and surfaces.
    BubbleRenderer(Shader& bubbleShader, Shader& surfaceShader, GLuint bubbleTextureID);
    ~BubbleRenderer();

    void renderBubbles(const BubblePool& pool);

    // Renders the surfaces as colored lines.
    void renderSurfaces(const std::vector<Surface2D>& surfaces, const glm::mat4& projection);

private:
    Shader& bubbleShader;
    Shader& surfaceShader;
    GLuint bubbleTextureID;

    // Separate VAOs for bubbles (quads) and surfaces (lines)
    GLuint bubbleVAO, bubbleVBO;
    GLuint surfaceVAO, surfaceVBO;

    void initRenderData();
};

#endif