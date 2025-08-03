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

    bool isActive;          //Replaced 'marked_for_removal' with 'isActive' for pooling.

    // Calculates initial mass based on radius and gas density
    static float calculateMass(float r) {
        // This is 2D approximation
        // mass = density * volume (area in 2D)
        return GAS_DENSITY * glm::pi<float>() * r * r;
    }

    // Default constructor
    Bubble() : id(-1), position(0.0f), velocity(0.0f), radius(0.0f), mass(0.0f),
        force_accumulator(0.0f), on_surface(false), surface_id(-1),
        time_on_surface(0.0f), isActive(false) {
    }

    // init() function to reset a recycled bubble.
    void init(int new_id, glm::vec2 pos, float r) {
        id = new_id;
        position = pos;
        radius = r;
        velocity = glm::vec2(0.0f);
        force_accumulator = glm::vec2(0.0f);
        on_surface = false;
        surface_id = -1;
        time_on_surface = 0.0f;
        isActive = true; // Activate the bubble.
        updateMass();
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
