#pragma once
#define BUBBLE_POOL_H

#include "Bubble.h"
#include "SimulationConstants.h"
#include <vector>

class BubblePool {
public:
    std::vector<Bubble> bubbles;

    BubblePool();
    Bubble* getInactiveBubble();
};

