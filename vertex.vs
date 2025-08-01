#version 330 core
layout (location = 0) in vec2 aPos;      // Vertex position of the unit quad (-0.5 to 0.5)
layout (location = 1) in vec2 aTexCoord; // Texture coordinates (0.0 to 1.0)

out vec2 TexCoord;

uniform mat4 model;       // Transforms the unit quad to bubble's position and size
uniform mat4 projection;  // Orthographic projection matrix

void main()
{
    // Transform vertex position:
    // 1. Apply model transformation (scale and translate the unit quad)
    // 2. Apply projection transformation
    gl_Position = projection * model * vec4(aPos.x, aPos.y, 0.0, 1.0);
    
    TexCoord = aTexCoord; // Pass texture coordinates to fragment shader
}