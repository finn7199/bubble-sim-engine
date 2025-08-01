#ifndef BUBBLE_SIMULATOR_H
#define BUBBLE_SIMULATOR_H

#include <vector>
#include <random>
#include "Bubble.h"
#include "Surface2D.h"
#include "FluidGrid2D.h"
#include "SimulationConstants.h"

class BubbleGenerator;

class BubbleSimulator {
public:
    BubbleSimulator(int screenWidth, int screenHeight);

    void update(float dt, std::vector<Bubble>& bubbles);

    void addSurface(const Surface2D& surface);
    const std::vector<Surface2D>& getSurfaces() const { return surfaces; }

    FluidGrid2D& getFluidGrid() { return fluid_grid; }

private:
    // Force Calculation
    void applyGravity(Bubble& bubble);
    void applyBuoyancy(Bubble& bubble);
    void applyDrag(Bubble& bubble);
    void applyAdhesionForces(Bubble& bubble, float dt);

    // Collision Handling
    void handleBubbleCollisions(std::vector<Bubble>& bubbles);
    void handleSurfaceCollisions(std::vector<Bubble>& bubbles, float dt);

    // Other Bubble Processes
    void growBubbles(std::vector<Bubble>& bubbles, float dt);
    void fuseBubbles(Bubble& b1, Bubble& b2, std::vector<Bubble>& bubbles);
    void cleanupRemovedBubbles(std::vector<Bubble>& bubbles);

    // Simulation State
    FluidGrid2D fluid_grid;
    std::vector<Surface2D> surfaces;
    float screen_width;
    float screen_height;

    // Random number generation
    std::mt19937 random_engine;
    std::uniform_real_distribution<float> random_dist;

    // For adhesion, we need normal force from surface.
    float normalForceOnSurface(const Bubble& bubble, const Surface2D& surface, float dt);
};

#endif
