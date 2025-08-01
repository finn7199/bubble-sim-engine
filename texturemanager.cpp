#include "TextureManager.h"
#define STB_IMAGE_IMPLEMENTATION 
#include "stb_image.h"        
#include <iostream>

GLuint TextureManager::loadTexture(const std::string& path, bool alpha) {
    unsigned int textureID;
    glGenTextures(1, &textureID); // Generate texture ID

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // Flip texture vertically on load
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0); // Load image data
    if (data) {
        GLenum format;
        if (nrChannels == 1)
            format = GL_RED;
        else if (nrChannels == 3 || !alpha) // If user says no alpha, treat as RGB
            format = GL_RGB;
        else // nrChannels == 4 or alpha requested and available
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID); // Bind the texture 
        // Create texture and generate mipmaps
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        // Set texture wrapping and filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data); // Free image memory
    }
    else {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
        textureID = 0;
    }

    return textureID;
}