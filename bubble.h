#ifndef BUBBLE_H
#define BUBBLE_H
#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/norm.hpp>
#include "SimulationConstants.h"

// Defines the properties of a single bubble in the 2D simulation.
struct Bubble {
    int id;                 // Unique id for the bubble
    glm::vec2 position;     // Current position of the bubble
    glm::vec2 velocity;     // Current velocity of the bubble
    float radius;           // Radius of the bubble
    float mass;             // Mass of the bubble.
    glm::vec2 force_accumulator; // Sum forces acting on the bubble

    bool on_surface;        // If the bubble is currently on a surface.
    int surface_id;         // Surface id for it's properties
    float time_on_surface; 

    bool marked_for_removal; // Flag for bubbles that have fused or should be removed

    // Calculates initial mass based on radius and gas density
    static float calculateMass(float r) {
        // This is 2D approximation
        // mass = density * volume (area in 2D)
        return GAS_DENSITY * glm::pi<float>() * r * r;
    }

    // Default constructor
    Bubble(int id_val = 0, glm::vec2 pos_val = glm::vec2(0.0f, 0.0f), float rad_val = 10.0f, glm::vec2 vel_val = glm::vec2(0.0f, 0.0f))
        : id(id_val), position(pos_val), velocity(vel_val), radius(rad_val),
        mass(calculateMass(rad_val)), force_accumulator(0.0f, 0.0f),
        on_surface(false), surface_id(-1), time_on_surface(0.0f),
        marked_for_removal(false) {
    }

    void updateMass() {
        mass = calculateMass(radius);
    }

    // For 2D, "volume" is area
    float getArea() const {
        return glm::pi<float>() * radius * radius;
    }

    // For 2D, "surface area" is circumference
    float getCircumference() const {
        return 2.0f * glm::pi<float>() * radius;
    }
};

#endif
