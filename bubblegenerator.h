#ifndef BUBBLE_GENERATOR_H
#define BUBBLE_GENERATOR_H

#include <vector>
#include <random>
#include "Bubble.h"
#include "BubblePool.h"
#include "Surface2D.h"

// Manages the creation and storage of bubble instances.
class BubbleGenerator {
public:
    BubbleGenerator();
    void tryGenerateBubbles(BubblePool& pool, const std::vector<Surface2D>& surfaces, float dt, float screenWidth, float screenHeight);
    int getNextBubbleID() { return next_bubble_id++; }

private:
    int next_bubble_id; // unique IDs for bubbles

    // For generation timing/probability
    std::mt19937 random_engine;
    std::uniform_real_distribution<float> random_dist_prob;

    const float GENERATION_PROBABILITY_PER_SECOND_PER_SURFACE = 0.5f; // Bubbles per second from a surface
    const float INITIAL_BUBBLE_RADIUS_MIN = 2.0f;
    const float INITIAL_BUBBLE_RADIUS_MAX = 5.0f;
};

#endif