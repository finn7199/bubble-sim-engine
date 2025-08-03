#include "BubbleSimulator.h"
#include "BubbleGenerator.h" 
#include <glm/gtx/norm.hpp> 
#include <algorithm>

// Constructor
BubbleSimulator::BubbleSimulator(int screenWidth, int screenHeight)
    : fluid_grid(screenWidth, screenHeight),
    screen_width(static_cast<float>(screenWidth)),
    screen_height(static_cast<float>(screenHeight)),
    random_engine(std::random_device{}()), 
    random_dist(0.0f, 1.0f) {
}

void BubbleSimulator::addSurface(const Surface2D& surface) {
    surfaces.push_back(surface);
}

void BubbleSimulator::update(float dt, std::vector<Bubble>& bubbles) {
    if (dt <= 0.0f) return;
    fluid_grid.update(dt);

    //Apply forces to bubbles & update them
    for (Bubble& bubble : bubbles) {
        if (!bubble.isActive) continue;

        bubble.force_accumulator = glm::vec2(0.0f, 0.0f); // Reset forces

        applyGravity(bubble);
        applyBuoyancy(bubble);
        applyDrag(bubble);
        //applyLift(bubble) // needs better vorticity calc
        // Adhesion forces are handled after surface collision and normal force estimation
    }

    // Handle Collisions
    handleBubbleCollisions(bubbles);
    handleSurfaceCollisions(bubbles, dt); 

    // Integrate motion for bubbles not stuck by static adhesion
    for (Bubble& bubble : bubbles) {
        if (!bubble.isActive) continue;

        if (bubble.on_surface) {
            // If on surface, static adhesion might prevent motion or dynamic adhesion applies
            // The decision to move or the effect of dynamic adhesion is handled within applyAdhesionForces
            // For now, assume applyAdhesionForces modifies force_accumulator or directly velocity
        }

        // If not fully constrained by static adhesion:
        glm::vec2 acceleration = bubble.force_accumulator / bubble.mass;
        bubble.velocity += acceleration * dt;
        bubble.position += bubble.velocity * dt;

        // Boundary check
        if (bubble.position.x - bubble.radius < 0) {
            bubble.position.x = bubble.radius;
            bubble.velocity.x *= -0.5f; // Damping
        }
        else if (bubble.position.x + bubble.radius > screen_width) {
            bubble.position.x = screen_width - bubble.radius;
            bubble.velocity.x *= -0.5f;
        }
        if (bubble.position.y - bubble.radius < 0) { // Bottom
            bubble.position.y = bubble.radius;
            bubble.velocity.y *= -0.5f;
            // Potentially stick to bottom surface or pop
        }
        else if (bubble.position.y + bubble.radius > screen_height) { // Top
            bubble.isActive = false;
        }
    }

    growBubbles(bubbles, dt);

    // Two-way coupling - Bubbles affect fluid (after their forces are calculated)
    for (const Bubble& bubble : bubbles) {
        if (!bubble.isActive) continue;
        fluid_grid.applyBubbleForce(bubble, dt);
    }
}

void BubbleSimulator::applyGravity(Bubble& bubble) {
    bubble.force_accumulator += GRAVITY * bubble.mass;
}

void BubbleSimulator::applyBuoyancy(Bubble& bubble) {
    // Buoyancy F_b = (m_bubble - rho_liq * V_bubble) * g_vec  (from paper)
    // This means buoyant force opposes gravity effectively if m_bubble is small
    // Or, simpler: Buoyant force = rho_liq * V _bubble * (-g_unit_vec) * |g|
    // Small bubble mass is negligible compared to displaced water mass.
    float displaced_fluid_mass = WATER_DENSITY * bubble.getArea();
    glm::vec2 buoyancy_force = -GRAVITY * displaced_fluid_mass;
    bubble.force_accumulator += buoyancy_force;
}

void BubbleSimulator::applyDrag(Bubble& bubble) {
    // F_d = -k_drag * (m_i / r_i) * |v_rel| * v_rel
    glm::vec2 fluid_vel_at_bubble = fluid_grid.getVelocityAt(bubble.position);
    glm::vec2 relative_velocity = bubble.velocity - fluid_vel_at_bubble;
    float relative_speed_sq = glm::length2(relative_velocity);

    if (relative_speed_sq > 0.000001f) {
        float relative_speed = glm::sqrt(relative_speed_sq);

        if (bubble.radius > 0.01f) { // Avoid division by zero
            float paper_factor = bubble.mass / bubble.radius;
            glm::vec2 drag_force = -FLUID_DRAG_COEFFICIENT * paper_factor * relative_speed * relative_velocity;
            bubble.force_accumulator += drag_force;
        }
    }
}

 //void BubbleSimulator::applyLift(Bubble& bubble) {
 //    // F_l = k_lift * m_i * (v_i - u_i) x Omega_i
 //    // Cross product in 2D: (Ax, Ay) x Oz = (Ay*Oz, -Ax*Oz)
 //    glm::vec2 fluid_vel_at_bubble = fluid_grid.getVelocityAt(bubble.position);
 //    glm::vec2 relative_velocity = bubble.velocity - fluid_vel_at_bubble;
 //   float vorticity = fluid_grid.getVorticityAt(bubble.position); // Scalar in 2D

 //    if (glm::abs(vorticity) > 0.001f) {
 //        glm::vec2 lift_force_dir(relative_velocity.y * vorticity, -relative_velocity.x * vorticity);
 //        bubble.force_accumulator += FLUID_LIFT_COEFFICIENT * bubble.mass * lift_force_dir;
 //    }
 //}

// --- Adhesion ---

float BubbleSimulator::normalForceOnSurface(const Bubble& bubble, const Surface2D& surface, float dt) {
    // Static term: buoyancy contribution
    glm::vec2 buoyancy_comp = -GRAVITY * (WATER_DENSITY * bubble.getArea());
    float N_static = glm::abs(glm::dot(buoyancy_comp, surface.normal));

    // Dynamic term: impact force (if colliding)
    float velocity_impact = glm::dot(bubble.velocity, surface.normal);
    float N_dynamic = bubble.mass * glm::max(0.0f, -velocity_impact) / dt;

    // Safety threshold
    return glm::max(1.0f, N_static + N_dynamic);
}


void BubbleSimulator::applyAdhesionForces(Bubble& bubble, float dt) {
    if (!bubble.on_surface || bubble.surface_id < 0 || bubble.surface_id >= surfaces.size()) {
        return;
    }
    const Surface2D& surface = surfaces[bubble.surface_id];
    float N = normalForceOnSurface(bubble, surface, dt); // Magnitude of normal force

    // Calculate tangential forces (sum of all forces parallel to surface)
    glm::vec2 surface_tangent = glm::vec2(surface.normal.y, -surface.normal.x); 
    glm::vec2 net_force_on_bubble = bubble.force_accumulator; // All accumulated forces

    // Component of net_force_on_bubble trying to move it along the surface
    float tangential_force_scalar = glm::dot(net_force_on_bubble, surface_tangent);
    glm::vec2 tangential_force_vec = surface_tangent * tangential_force_scalar;

    // Static Adhesion (F_st = mu * |N| * g/|g|
    float max_static_adhesion_mag = surface.static_adhesion * N;

    if (glm::length2(bubble.velocity) < 0.01f * 0.01f) { // If bubble is nearly stationary on surface
        if (glm::length2(tangential_force_vec) < max_static_adhesion_mag * max_static_adhesion_mag) {
            // Static adhesion holds, counteract tangential forces
            bubble.force_accumulator -= tangential_force_vec;
            bubble.velocity = glm::vec2(0.0f);
        }
        else {
            // Static adhesion broken, switches to dynamic (detachment not handled yet)
            // The force that broke static friction is already in force_accumulator.
            glm::vec2 dynamic_adhesion_force(0.0f);
            if (glm::length2(tangential_force_vec) > 0.0001f) {
                dynamic_adhesion_force = -glm::normalize(tangential_force_vec) * surface.dynamic_adhesion * N;
                bubble.force_accumulator += dynamic_adhesion_force;
            }
        }
    }
    else { // Bubble is already moving on the surface (gliding)
        // Dynamic Adhesion (Paper: F_dy = -mu' * |N| * v_surface / |v_surface|)
        glm::vec2 bubble_velocity_tangential_component = surface_tangent * glm::dot(bubble.velocity, surface_tangent);
        if (glm::length2(bubble_velocity_tangential_component) > 0.0001f) {
            glm::vec2 dynamic_adhesion_force = -glm::normalize(bubble_velocity_tangential_component) * surface.dynamic_adhesion * N;
            bubble.force_accumulator += dynamic_adhesion_force;
        }
    }
    bubble.time_on_surface += dt;
}


// --- Collision Handling ---
void BubbleSimulator::handleBubbleCollisions(std::vector<Bubble>& bubbles) {
    for (size_t i = 0; i < bubbles.size(); ++i) {
        if (!bubbles[i].isActive) continue;
        for (size_t j = i + 1; j < bubbles.size(); ++j) {
            if (!bubbles[j].isActive) continue;

            Bubble& b1 = bubbles[i];
            Bubble& b2 = bubbles[j];

            glm::vec2 delta_pos = b2.position - b1.position;
            float dist_sq = glm::length2(delta_pos);
            float sum_radii = b1.radius + b2.radius;

            if (dist_sq < sum_radii * sum_radii && dist_sq > 0.0001f) {
                // Try fusion first based on probability
                if (random_dist(random_engine) < BUBBLE_FUSION_PROBABILITY) {
                    fuseBubbles(b1, b2, bubbles); // b1 becomes the new bubble
                    //continue; // b2 is now inactive, the inner loop will skip it.
                }

                // Repulsion
                float dist = glm::sqrt(dist_sq);
                glm::vec2 normal_ij = delta_pos / dist; // Normal from b1 to b2

                // Penetration depth scalar
                float penetration = sum_radii - dist;
                glm::vec2 penetration_vec = normal_ij * penetration;

                // Relative velocity
                glm::vec2 relative_velocity_ji = b2.velocity - b1.velocity; 
                float v_n_scalar = glm::dot(relative_velocity_ji, normal_ij); 

                // Spring force (acts to separate them)
                glm::vec2 spring_force_on_b2 = penetration_vec * BUBBLE_COLLISION_STIFFNESS;

                // Damping force (opposes relative motion in normal direction)
                // Positive when bubbles separating (pulls them back)
                // Negative when approaching (pushes them apart)
                glm::vec2 damping_force_on_b2 = normal_ij * (-BUBBLE_COLLISION_DAMPING * v_n_scalar);

                // Paper F_c = m_i * (k_col * delta_x_vec + k_damp * v_n_vec) (Force on i)
                // Delta_x_vec for b1 from b2: normal_ji * penetration = -normal_ij * penetration
                // v_n_vec for b1 from b2: proj(v1-v2, normal_ji)
                // For b1:
                glm::vec2 spring_f1 = -normal_ij * penetration * BUBBLE_COLLISION_STIFFNESS;
                glm::vec2 rel_vel_b1_vs_b2 = b1.velocity - b2.velocity;
                float v_n_b1 = glm::dot(rel_vel_b1_vs_b2, -normal_ij);
                glm::vec2 damp_f1 = -normal_ij * (-BUBBLE_COLLISION_DAMPING * v_n_b1);


                // Apply forces (scaled by mass as per paper)
                // Apply forces directly, next step will handle mass.
                b1.force_accumulator += (spring_f1 + damp_f1);
                b2.force_accumulator -= (spring_f1 + damp_f1);

                // Simple position correction to avoid prolonged overlap (can make simulation jittery if too aggressive)
                float correction_factor = 0.5f;
                glm::vec2 correction_vec = normal_ij * penetration * correction_factor;
                b1.position -= correction_vec * (b2.mass / (b1.mass + b2.mass)); // Distribute correction by mass
                b2.position += correction_vec * (b1.mass / (b1.mass + b2.mass));
            }
        }
    }
}


void BubbleSimulator::handleSurfaceCollisions(std::vector<Bubble>& bubbles, float dt) {
    for (Bubble& bubble : bubbles) {
        if (!bubble.isActive) continue;

        bool was_on_surface = bubble.on_surface;
        bubble.on_surface = false; // Reset, will be set if collision detected

        for (const Surface2D& surface : surfaces) {
            // Project bubble center onto the line defined by the surface
            glm::vec2 line_vec = surface.end_point - surface.start_point;
            glm::vec2 bubble_to_start = bubble.position - surface.start_point;

            float t = glm::dot(bubble_to_start, line_vec) / glm::dot(line_vec, line_vec);

            // Clamp t to the line segment [0, 1] to find the closest point
            t = glm::clamp(t, 0.0f, 1.0f);
            glm::vec2 closest_point_on_segment = surface.start_point + t * line_vec;

            glm::vec2 vec_to_closest = bubble.position - closest_point_on_segment;
            float dist_sq = glm::length2(vec_to_closest);

            // Check for collision
            if (dist_sq < bubble.radius * bubble.radius) {
                bubble.on_surface = true;
                bubble.surface_id = surface.id;
                if (!was_on_surface) bubble.time_on_surface = 0.0f;

                // More robust penetration resolution to prevent tunneling.
                float dist = (dist_sq > 0.0001f) ? glm::sqrt(dist_sq) : 0.0f;
                float penetration = bubble.radius - dist;

                // If the bubble is not exactly on the line, push it out along the vector
                // from the closest point. Otherwise, push it out along the surface normal.
                if (dist > 0.0001f) {
                    bubble.position += glm::normalize(vec_to_closest) * penetration;
                }
                else {
                    bubble.position += surface.normal * penetration;
                }
                // Reflect velocity component normal to surface
                float vn = glm::dot(bubble.velocity, surface.normal);
                if (vn < 0) { // Moving towards surface
                    bubble.velocity -= (1.0f + 0.3f) * vn * surface.normal; // 0.3 is restitution
                }

                applyAdhesionForces(bubble, dt); // Apply adhesion now that we know it's on this surface
                break; // A bubble can only be on one surface at a time
            }
        }
        if (!bubble.on_surface) { // No longer on any surface
            bubble.time_on_surface = 0.0f;
            bubble.surface_id = -1;
        }
    }
}


// --- Other Processes ---
void BubbleSimulator::growBubbles(std::vector<Bubble>& bubbles, float dt) {
    for (Bubble& bubble : bubbles) {
        if (!bubble.isActive) continue;

        // Paper: "keeps growing by absorbing the resolved gas in the amount proportional to its surface area."
        // "Larger static adhesion forces let bubbles stay longer at the generation point and thus grow larger"
        // For 2D, dr/dt = constant_rate (derived if d(Area)/dt is proportional to Circumference)
        if (bubble.radius < BUBBLE_MAX_RADIUS) {
            bubble.radius += BUBBLE_GROWTH_RATE * dt;
            bubble.radius = glm::min(bubble.radius, BUBBLE_MAX_RADIUS);
            bubble.updateMass(); // Mass changes with radius
        }
    }
}

void BubbleSimulator::fuseBubbles(Bubble& b1, Bubble& b2, std::vector<Bubble>& bubbles) {
    // b1 absorbs b2. b2 will be marked for removal.
    float total_area = b1.getArea() + b2.getArea();

    // Centroid for new position (weighted by area/mass)
    b1.position = (b1.position * b1.mass + b2.position * b2.mass) / (b1.mass + b2.mass);
    // Conserve momentum
    b1.velocity = (b1.velocity * b1.mass + b2.velocity * b2.mass) / (b1.mass + b2.mass);

    b1.radius = glm::sqrt(total_area / glm::pi<float>()); // New radius from total area
    b1.updateMass(); // Update mass for new radius

    // Cap radius after fusion
    if (b1.radius > BUBBLE_MAX_RADIUS) {
        b1.radius = BUBBLE_MAX_RADIUS;
        b1.updateMass();
    }
    b2.isActive = false;
}
