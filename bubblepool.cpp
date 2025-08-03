#include "BubblePool.h"

BubblePool::BubblePool() {
    // Pre-allocate and resize the vector to hold all possible bubbles.
    // The default Bubble constructor ensures they all start as inactive.
    bubbles.resize(MAX_BUBBLES);
}

Bubble* BubblePool::getInactiveBubble() {
    // Find the first bubble in the pool that is not currently in use.
    for (auto& bubble : bubbles) {
        if (!bubble.isActive) {
            return &bubble; // Return a pointer to it.
        }
    }
    // If all bubbles are active, we can't create a new one.
    return nullptr;
}