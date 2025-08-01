// BubbleGenerator.cpp
#include "BubbleGenerator.h"
#include "SimulationConstants.h" // For BUBBLE_MIN_RADIUS, BUBBLE_MAX_RADIUS (or define specific initial ranges)
#include <iostream>

// Define initial spawn radius separately if desired, or use general bubble min/max
const float INITIAL_SPAWN_RADIUS_MIN = 8.0f;  // Example: aligned with new BUBBLE_MIN_RADIUS
const float INITIAL_SPAWN_RADIUS_MAX = 15.0f; // Example: slightly larger than min, but not full max

BubbleGenerator::BubbleGenerator()
    : next_bubble_id(0),
    random_engine(std::random_device{}()),
    random_dist_prob(0.0f, 1.0f),
    random_dist_pos_offset(-1.0f, 1.0f)
{
}

void BubbleGenerator::generateInitialRandomBubbles(int count, float screenWidth, float screenHeight) {
    bubbles.clear();
    for (int i = 0; i < count; ++i) {
        float x = random_dist_prob(random_engine) * (screenWidth - 50.0f) + 25.0f;
        float y = random_dist_prob(random_engine) * (screenHeight - 50.0f) + 25.0f;
        float radius = random_dist_prob(random_engine) * (INITIAL_SPAWN_RADIUS_MAX - INITIAL_SPAWN_RADIUS_MIN) + INITIAL_SPAWN_RADIUS_MIN;

        bubbles.emplace_back(getNextBubbleID(), glm::vec2(x, y), radius);
    }
}

void BubbleGenerator::tryGenerateBubbles(const std::vector<Surface2D>& surfaces, float dt, float screenWidth, float screenHeight) {
    // --- Increased Generation Rate ---
    // You can adjust this multiplier. A higher value means more bubbles per second.
    // The original GENERATION_PROBABILITY_PER_SECOND_PER_SURFACE is in BubbleGenerator.h (0.5f by default)
    // Let's make them generate much more frequently for testing collisions.
    const float generation_rate_multiplier = 20.0f; // Generate 10x more frequently than the base rate

    for (const Surface2D& surface : surfaces) {
        if (surface.allows_generation) {
            // Check multiple times per surface per frame to simulate more generation points or higher density
            // Or simply increase the probability. Let's increase the effective probability.
            if (random_dist_prob(random_engine) < GENERATION_PROBABILITY_PER_SECOND_PER_SURFACE * dt * generation_rate_multiplier) {
                float t = random_dist_prob(random_engine); // Parameter from 0 to 1 along the line
                glm::vec2 gen_pos_on_line = surface.start_point + t * (surface.end_point - surface.start_point);

                // Use the defined initial spawn radius constants
                float radius = random_dist_prob(random_engine) * (INITIAL_SPAWN_RADIUS_MAX - INITIAL_SPAWN_RADIUS_MIN) + INITIAL_SPAWN_RADIUS_MIN;
                // Ensure bubble starts slightly offset from the surface, using its actual radius for the offset
                float offset_dist = radius * 1.1f; // Start slightly outside the surface
                glm::vec2 gen_pos = gen_pos_on_line + surface.normal * offset_dist;


                if (gen_pos.x > radius && gen_pos.x < screenWidth - radius &&
                    gen_pos.y > radius && gen_pos.y < screenHeight - radius) {

                    Bubble new_bubble(getNextBubbleID(), gen_pos, radius);
                    new_bubble.on_surface = true; // Starts on the surface
                    new_bubble.surface_id = surface.id;
                    new_bubble.time_on_surface = 0.0f;
                    // Bubbles generated on a surface initially have zero velocity or a tiny push.
                    // The simulation will then apply buoyancy, etc.
                    bubbles.push_back(new_bubble);
                    // std::cout << "Generated bubble " << new_bubble.id << " at (" << gen_pos.x << "," << gen_pos.y << ") on surface " << surface.id << std::endl;
                }
            }
        }
    }
    // Limit total number of bubbles for performance if needed
   // const size_t MAX_BUBBLES = 200;
   // if (bubbles.size() > MAX_BUBBLES) {
   //    bubbles.erase(bubbles.begin(), bubbles.begin() + (bubbles.size() - MAX_BUBBLES));
   // }
}

