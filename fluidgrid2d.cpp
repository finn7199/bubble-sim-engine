#include "fluidgrid2d.h"
#include "bubble.h"
#include <algorithm>

FluidGrid2D::FluidGrid2D(int screenWidth, int screenHeight) {
    width_cells = screenWidth / GRID_CELL_SIZE;
    height_cells = screenHeight / GRID_CELL_SIZE;
    velocities.resize(width_cells * height_cells, glm::vec2(0.0f, 0.0f));
}

void FluidGrid2D::update(float dt) {
    for (glm::vec2& vel : velocities) {
        vel *= (1.0f - 0.1f * dt); // Simple fluid damping
    }
}

glm::ivec2 FluidGrid2D::getCellIndex(glm::vec2 position) const {
    int x_idx = static_cast<int>(position.x / GRID_CELL_SIZE);
    int y_idx = static_cast<int>(position.y / GRID_CELL_SIZE);
    return glm::ivec2(
        std::max(0, std::min(x_idx, width_cells - 1)),
        std::max(0, std::min(y_idx, height_cells - 1))
    );
}

glm::vec2 FluidGrid2D::getVelocityAt(glm::vec2 position) const {
    // Simple nearest neighbor for now
    // Can be bi-linear interpolation for smoother results
    glm::ivec2 cell_idx = getCellIndex(position);
    if (cell_idx.x >= 0 && cell_idx.x < width_cells && cell_idx.y >= 0 && cell_idx.y < height_cells) {
        return velocities[cell_idx.y * width_cells + cell_idx.x];
    }
    return glm::vec2(0.0f, 0.0f); // Out of bounds
}

float FluidGrid2D::getVorticityAt(glm::vec2 position) const {
    // Simplified vorticity (central differencing of velocity)
    // Vorticity (2D scalar) = d(vy)/dx - d(vx)/dy
    glm::ivec2 P = getCellIndex(position);

    // Get velocities of neighboring cells
    glm::vec2 v_L = getVelocityAt(glm::vec2((P.x - 1) * GRID_CELL_SIZE + GRID_CELL_SIZE / 2.0f, P.y * GRID_CELL_SIZE + GRID_CELL_SIZE / 2.0f));
    glm::vec2 v_R = getVelocityAt(glm::vec2((P.x + 1) * GRID_CELL_SIZE + GRID_CELL_SIZE / 2.0f, P.y * GRID_CELL_SIZE + GRID_CELL_SIZE / 2.0f));
    glm::vec2 v_B = getVelocityAt(glm::vec2(P.x * GRID_CELL_SIZE + GRID_CELL_SIZE / 2.0f, (P.y - 1) * GRID_CELL_SIZE + GRID_CELL_SIZE / 2.0f));
    glm::vec2 v_T = getVelocityAt(glm::vec2(P.x * GRID_CELL_SIZE + GRID_CELL_SIZE / 2.0f, (P.y + 1) * GRID_CELL_SIZE + GRID_CELL_SIZE / 2.0f));

    float dvx_dy = (v_T.x - v_B.x) / (4.0f * GRID_CELL_SIZE);
    float dvy_dx = (v_R.y - v_L.y) / (4.0f * GRID_CELL_SIZE);

    // Can be noisy without a proper fluid solver. Add boundary checks?
    // This is simpler to run 
    return dvy_dx - dvx_dy; 
}


void FluidGrid2D::applyBubbleForce(const Bubble& bubble, float dt) {
// Simplified: bubble "pushes" fluid in its direction of motion

    glm::ivec2 cell_idx = getCellIndex(bubble.position);
    if (cell_idx.x >= 0 && cell_idx.x < width_cells && cell_idx.y >= 0 && cell_idx.y < height_cells) {
        float influence_radius = bubble.radius * 1.5f;
        float bubble_force_magnitude = bubble.mass * glm::length(bubble.velocity) / dt; // Estimate with F = m*v / delta t

        // Spread influence to nearby cells
        for (int y_offset = -1; y_offset <= 1; ++y_offset) {
            for (int x_offset = -1; x_offset <= 1; ++x_offset) {
                glm::ivec2 current_cell_idx = cell_idx + glm::ivec2(x_offset, y_offset);
                if (current_cell_idx.x >= 0 && current_cell_idx.x < width_cells &&
                    current_cell_idx.y >= 0 && current_cell_idx.y < height_cells) {

                    glm::vec2 cell_center_pos = glm::vec2(
                        (current_cell_idx.x + 0.5f) * GRID_CELL_SIZE,
                        (current_cell_idx.y + 0.5f) * GRID_CELL_SIZE
                    );
                    float dist_sq = glm::length2(bubble.position - cell_center_pos);
                    if (dist_sq < influence_radius * influence_radius) {
                        float weight = 1.0f - glm::sqrt(dist_sq) / influence_radius; // Simple falloff
                        // Add a portion of the bubble's force to the fluid cell's velocity
                        // For now, let's just use bubble's velocity to "stir" the fluid.
                        glm::vec2 force_on_fluid = bubble.velocity * bubble.mass * weight * 0.1f; // Factor for tuning
                        velocities[current_cell_idx.y * width_cells + current_cell_idx.x] += force_on_fluid * dt / (float)GRID_CELL_SIZE; // Divide by cell "mass" conceptually
                    }
                }
            }
        }
    }
}


void FluidGrid2D::drawGridVelocities(/* Shader& debugShader, glm::mat4 projection */) {
    // Placeholder for debug drawing grid cell velocities
    // Could draw lines from cell centers indicating velocity vectors
}
