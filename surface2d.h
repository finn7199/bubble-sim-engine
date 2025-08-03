#ifndef SURFACE2D_H
#define SURFACE2D_H

#include <glm/glm.hpp>
#include "SimulationConstants.h" // For default adhesion values

// Represents a 2D surface
struct Surface2D {
    int id;
    glm::vec2 start_point;
    glm::vec2 end_point;
    glm::vec2 normal; // Normal of the surface (to the fluid)
    // For a horizontal surface at y=H, normal might be (0,1)

    float static_adhesion;
    float dynamic_adhesion;
    bool allows_generation;
    glm::vec3 color; // Added color property for rendering

    Surface2D(int id_val, glm::vec2 start, glm::vec2 end, float static_adh, float dynamic_adh, bool gen, glm::vec3 col = glm::vec3(0.7f))
        : id(id_val), start_point(start), end_point(end),
        static_adhesion(static_adh), dynamic_adhesion(dynamic_adh),
        allows_generation(gen), color(col) {

        glm::vec2 dir = end_point - start_point;
        normal = glm::normalize(glm::vec2(-dir.y, dir.x));
    }
};

#endif
