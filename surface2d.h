#ifndef SURFACE2D_H
#define SURFACE2D_H

#include <glm/glm.hpp>

// Represents a 2D surface
struct Surface2D {
    int id;
    glm::vec2 start_point;
    glm::vec2 end_point;
    glm::vec2 normal; // Normal of the surface (to the fluid)
    // For a horizontal surface at y=H, normal might be (0,1)

// Properties of surface interaction
    float static_adhesion;  
    float dynamic_adhesion;
    bool allows_generation;

    Surface2D(int id, glm::vec2 start, glm::vec2 end, float static_adh = STATIC_ADHESION_COEFFICIENT, float dynamic_adh = DYNAMIC_ADHESION_COEFFICIENT, bool gen = false)
        : id(id), start_point(start), end_point(end), static_adhesion(static_adh), dynamic_adhesion(dynamic_adh), allows_generation(gen) {
        // For a line, a normal can be (dy, -dx) or (-dy, dx)
        glm::vec2 dir = end_point - start_point;
        // Normal pointing left. Adjust as needed
        normal = glm::normalize(glm::vec2(-dir.y, dir.x));
    }
};

#endif
