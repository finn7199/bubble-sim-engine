#ifndef FLUID_GRID_2D_H
#define FLUID_GRID_2D_H

#include <vector>
#include <glm/glm.hpp>
#include "SimulationConstants.h"

struct Bubble;

// Simplified 2D grid to store fluid simulation data.
class FluidGrid2D {
public:
    FluidGrid2D(int screenWidth, int screenHeight);

    void update(float dt);

    // Get interpolated fluid velocity at a given world position
    glm::vec2 getVelocityAt(glm::vec2 position) const;

    // Get interpolated vorticity at a given world position (simplified for now)
    float getVorticityAt(glm::vec2 position) const;

    // Apply force from bubbles to the fluid (simplified)
    void applyBubbleForce(const Bubble& bubble, float dt);

    // Debug draw the grid velocities
    void drawGridVelocities(/* add some rendering context or shader (future work) */);

private:
    int width_cells;  // Number of cells horizontally
    int height_cells; // Number of cells vertically
    std::vector<glm::vec2> velocities; // Stores velocity for each cell center

    // Function to get cell index from world pos
    glm::ivec2 getCellIndex(glm::vec2 position) const;
};

#endif 
