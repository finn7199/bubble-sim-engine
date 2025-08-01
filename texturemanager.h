#pragma once
#ifndef TEXTURE_MANAGER_H
#define TEXTURE_MANAGER_H

#include <string>
#include <glad/glad.h> /

// Manages loading and storing textures.
class TextureManager {
public:
    // Loads a texture from a file.
    // Returns the texture ID, or 0 if loading failed.
    // Parameters:
    //   path: The file path to the texture image.
    //   alpha: True if the texture has an alpha channel (e.g., PNG), false for RGB (e.g., JPG).
    static GLuint loadTexture(const std::string& path, bool alpha = true);
};

#endif