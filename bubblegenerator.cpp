// BubbleGenerator.cpp
#include "BubbleGenerator.h"
#include "SimulationConstants.h" // For BUBBLE_MIN_RADIUS, BUBBLE_MAX_RADIUS (or define specific initial ranges)
#include <iostream>

BubbleGenerator::BubbleGenerator()
    : next_bubble_id(0),
    random_engine(std::random_device{}()),
    random_dist_prob(0.0f, 1.0f)
{
}

void BubbleGenerator::tryGenerateBubbles(BubblePool& pool, const std::vector<Surface2D>& surfaces, float dt, float screenWidth, float screenHeight) {
    const float generation_rate_multiplier = 20.0f;
    const float INITIAL_SPAWN_RADIUS_MIN = 8.0f;
    const float INITIAL_SPAWN_RADIUS_MAX = 15.0f;

    for (const Surface2D& surface : surfaces) {
        if (surface.allows_generation) {
            if (random_dist_prob(random_engine) < 0.5f * dt * generation_rate_multiplier) {
                Bubble* newBubble = pool.activateBubble();
                if (newBubble) {
                    float t = random_dist_prob(random_engine);
                    glm::vec2 gen_pos_on_line = surface.start_point + t * (surface.end_point - surface.start_point);
                    float radius = random_dist_prob(random_engine) * (INITIAL_SPAWN_RADIUS_MAX - INITIAL_SPAWN_RADIUS_MIN) + INITIAL_SPAWN_RADIUS_MIN;
                    glm::vec2 gen_pos = gen_pos_on_line + surface.normal * (radius * 1.1f);

                    newBubble->init(getNextBubbleID(), gen_pos, radius);
                    newBubble->on_surface = true;
                    newBubble->surface_id = surface.id;
                }
            }
        }
    }
}
