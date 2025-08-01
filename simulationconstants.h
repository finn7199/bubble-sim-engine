#ifndef SIMULATION_CONSTANTS_H
#define SIMULATION_CONSTANTS_H

#include <glm/glm.hpp>

// --- Physics Constants ---
const glm::vec2 GRAVITY(0.0f, -50.0f);  // Adjusted for screen simulation (pixels/s^2)
const float WATER_DENSITY = 1.0f;       // Simplified density for 2D area calculations
const float GAS_DENSITY = 0.1f;         // Simplified density of gas in bubbles

// --- Bubble Properties & Behavior ---
const float BUBBLE_MIN_RADIUS = 8.0f;
const float BUBBLE_MAX_RADIUS = 40.0f;
const float BUBBLE_GROWTH_RATE = 1.5f; // Radius increment per second (dr/dt = const)

// --- Collision ---
const float BUBBLE_COLLISION_STIFFNESS = 900.0f; // Increase for stronger repulsion
const float BUBBLE_COLLISION_DAMPING = 15.0f;   // Adjusted damping
const float BUBBLE_FUSION_PROBABILITY = 0.01; // Lowered to see more repulsions, or set to 0.0 to test repulsion only, or 1.0 to test fusion only.

// --- Surface Interaction & Adhesion (Coefficients from paper, needs tuning) ---
const float STATIC_ADHESION_COEFFICIENT = 0.5f;
const float DYNAMIC_ADHESION_COEFFICIENT = 0.2f;

// --- Fluid Interaction ---
const float FLUID_DRAG_COEFFICIENT = 0.1f;

// --- Simulation Grid ---
const int GRID_CELL_SIZE = 20; // Pixels

#endif 
