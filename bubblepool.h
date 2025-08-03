#pragma once
#ifndef BUBBLE_POOL_H
#define BUBBLE_POOL_H

#include "Bubble.h"
#include "SimulationConstants.h"
#include <vector>

class BubblePool {
public:
    std::vector<Bubble> bubbles;
    size_t active_bubble_count; // Tracks the number of active bubbles

    BubblePool();

    // Gets the next available bubble from the pool
    Bubble* activateBubble();

    // Efficiently deactivates a bubble by swapping it with the last active one
    void deactivateBubble(size_t index);
};

#endif
