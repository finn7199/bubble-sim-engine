#include "BubblePool.h"
#include <utility>

BubblePool::BubblePool() {
    bubbles.resize(MAX_BUBBLES);
    active_bubble_count = 0; // No bubbles are active at the start
}

Bubble* BubblePool::activateBubble() {
    // If the pool is full, we can't activate a new bubble.
    if (active_bubble_count >= MAX_BUBBLES) {
        return nullptr;
    }

    // The next inactive bubble is always at the end of the active block.
    Bubble* bubble = &bubbles[active_bubble_count];
    active_bubble_count++; // Increment the count of active bubbles.
    return bubble;
}

void BubblePool::deactivateBubble(size_t index) {
    // To deactivate a bubble, we swap it with the last active bubble
    // and then decrease the active count. This keeps all active bubbles
    // packed contiguously at the front of the vector.

    if (index >= active_bubble_count) return;

    // The bubble at this index is no longer active.
    bubbles[index].isActive = false;

    // Decrement the active count.
    active_bubble_count--;

    // Swap the deactivated bubble with the one that was at the end of the
    // active list. No need to swap if it was already the last one.
    if (index != active_bubble_count) {
        std::swap(bubbles[index], bubbles[active_bubble_count]);
    }
}
