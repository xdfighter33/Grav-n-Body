#pragma once
#define _USE_MATH_DEFINES
#include <glm/glm.hpp>
#include "particle.hpp"
#include <glm/gtc/random.hpp>
#include <random>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif



class Simulation {
public:
    // Legacy OOP data
    std::vector<Particle> p_objects;

    // DOD data
    ParticleData m_particle_data;
    uint32_t m_sub_steps;
    float m_time = 0.0f;
    float m_frame_dt = 0.0f;
    float G = .01f;
    // Legacy OOP methods
    void add_particle_oop(glm::vec3 pos, glm::vec3 vel) {
        Particle p;
        p.position = pos;
        p.mass = 1.0f;
        p.velocity = vel;
        p_objects.push_back(p);
    }

    // DOD method
    void add_particle_dod(const glm::vec3& position, const glm::vec3& velocity, float mass = 1.0f) {
        size_t new_index = m_particle_data.count;
        size_t new_size = m_particle_data.count + 1;

        m_particle_data.positions.resize(new_size);
        m_particle_data.velocities.resize(new_size);
        m_particle_data.accelerations.resize(new_size);
        m_particle_data.forces.resize(new_size, glm::vec3(0.0f));
        m_particle_data.masses.resize(new_size);

        m_particle_data.positions[new_index] = position;
        m_particle_data.velocities[new_index] = velocity;
        m_particle_data.accelerations[new_index] = glm::vec3(0.0f);
        m_particle_data.forces[new_index] = glm::vec3(0.0f);
        m_particle_data.masses[new_index] = mass;

        m_particle_data.count = new_size;
    }


    void add_spiral_galaxy_dod(int num_particles, float radius, const glm::vec3& center) {
    m_particle_data.reserve(num_particles);
    size_t current_size = m_particle_data.count;
    m_particle_data.allocate(current_size + num_particles);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);

    // Galaxy parameters - tuned for stability
    const float spiral_tightness = 0.1f;    // Reduced for gentler spiral
    const float velocity_factor = 0.4f;     // Reduced for more stable orbits
    const int num_arms = 2;                 // Number of spiral arms
    const float arm_width = 0.15f;          // Thinner arms for better definition
    const float central_mass = 1000.0f;      // Reduced central mass
    const float disk_height = 0.01f;        // Flatter galaxy

    // Add central black hole first
    m_particle_data.positions[current_size] = center;
    m_particle_data.velocities[current_size] = glm::vec3(0.0f);
    m_particle_data.masses[current_size] = central_mass;
    m_particle_data.accelerations[current_size] = glm::vec3(0.0f);
    m_particle_data.forces[current_size] = glm::vec3(0.0f);
    
    // Create spiral galaxy distribution
    for (int i = 1; i < num_particles; ++i) {
        size_t idx = current_size + i;
        
        // Generate radius with exponential falloff for density
        float r = radius * (0.1f + 0.9f * std::pow(dis(gen), 2.0f));
        
        // Calculate spiral pattern
        float base_angle = dis(gen) * 2.0f * M_PI;
        float arm = std::floor(base_angle * num_arms / (2.0f * M_PI));
        float arm_angle = base_angle + spiral_tightness * std::log(r/radius);
        
        // Add controlled randomness for arm width
        float angle_dispersion = arm_width * (dis(gen) - 0.5f) * (r / radius);
        float theta = arm_angle + angle_dispersion;

        // Calculate position
        float x = r * std::cos(theta);
        float y = r * std::sin(theta);
        float z = radius * disk_height * (dis(gen) - 0.5f);

        glm::vec3 position = center + glm::vec3(x, y, z);
        
        // Calculate orbital velocity based on total mass within radius
        float mass_within_r = central_mass + i * (0.1f) * (r / radius);
        float orbital_speed = std::sqrt(G * mass_within_r / r) * velocity_factor;
        
        // Calculate velocity direction perpendicular to radius vector
        glm::vec3 radius_vec = position - center;
        glm::vec3 velocity_dir = glm::normalize(glm::cross(radius_vec, glm::vec3(0, 0, 1)));
        
        // Add small radial velocity component for stability
        glm::vec3 radial_dir = glm::normalize(radius_vec);
        glm::vec3 velocity = velocity_dir * orbital_speed;
        
        // Add tiny random perturbations
        velocity += glm::vec3(
            0.01f * orbital_speed * (dis(gen) - 0.5f),
            0.01f * orbital_speed * (dis(gen) - 0.5f),
            0.005f * orbital_speed * (dis(gen) - 0.5f)
        );

        // Store particle data
        m_particle_data.positions[idx] = position;
        m_particle_data.velocities[idx] = velocity;
        m_particle_data.masses[idx] = 0.1f;  // Constant mass for stability
        m_particle_data.accelerations[idx] = glm::vec3(0.0f);
        m_particle_data.forces[idx] = glm::vec3(0.0f);
    }
}   

    void add_spiral_pattern_dod(int num_rings, int dots_per_ring, float max_radius, const glm::vec3& center) {
        // Debug prints to verify particle creation
        std::cout << "Creating spiral with rings: " << num_rings << ", dots per ring: " << dots_per_ring << std::endl;

        int total_particles = 0;
        for (int ring = 0; ring < num_rings; ring++) {
            int ring_dots = (ring + 1) * dots_per_ring;
            total_particles += ring_dots;
        }

        std::cout << "Total particles to create: " << total_particles << std::endl;

        m_particle_data.reserve(total_particles);
        size_t current_size = m_particle_data.count;
        m_particle_data.allocate(current_size + total_particles);

        size_t particle_index = current_size;

        // Create particles ring by ring
        for (int ring = 0; ring < num_rings; ring++) {
            // Scale down the radius significantly to match camera view
            float ring_radius = (max_radius * ((float)(ring + 1) / num_rings)) * 0.5f;
            int ring_dots = (ring + 1) * dots_per_ring;

            // Debug first and last particle of each ring
            if (ring == 0 || ring == num_rings - 1) {
    //            std::cout << "Ring " << ring << " radius: " << ring_radius << std::endl;
            }

            for (int dot = 0; dot < ring_dots; dot++) {
                float angle = (2.0f * M_PI * dot) / ring_dots;

                glm::vec3 position = center + glm::vec3(
                    ring_radius * cos(angle),
                    0.0f,
                    ring_radius * sin(angle)
                );

                // Debug first particle position of first and last ring
                if (dot == 0 && (ring == 0 || ring == num_rings - 1)) {
  //                  std::cout << "Particle pos: " << position.x << ", " << position.y << ", " << position.z << std::endl;
                }

                m_particle_data.positions[particle_index] = position;
                m_particle_data.velocities[particle_index] = glm::vec3(0.0f);
                m_particle_data.masses[particle_index] = 1.0f;
                m_particle_data.accelerations[particle_index] = glm::vec3(0.0f);
                m_particle_data.forces[particle_index] = glm::vec3(0.0f);

                particle_index++;
            }
        }

//        std::cout << "Final particle count: " << m_particle_data.count << std::endl;
    }


    void add_particles_in_sphere_oop(int num_particles, float radius, const glm::vec3& center) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(0.0, 1.0);

        for (int i = 0; i < num_particles; ++i) {
            float theta = 2.0f * M_PI * dis(gen);
            float phi = acos(2.0f * dis(gen) - 1.0f);
            float r = radius * std::cbrt(dis(gen));

            float x = r * sin(phi) * cos(theta);
            float y = r * sin(phi) * sin(theta);
            float z = r * cos(phi);

            glm::vec3 position = center + glm::vec3(x, y, z);
            auto velocity = glm::vec3(0);
            add_particle_oop(position, velocity);
        }
    }

    void update() {
        m_time += m_frame_dt;
        const float step_dt = get_step_dt();
        for (uint32_t i = 0; i < m_sub_steps; ++i) {
            calculate_gravity_orbit();
            update_motion(step_dt);
        }
    }

    void set_simulation_update_rate(uint32_t rate) {
        m_frame_dt = 1.0f / static_cast<float>(rate);
    }

    void set_sub_steps_count(uint32_t sub_steps) {
        m_sub_steps = sub_steps;
    }

private:
    float get_step_dt() const {
        return m_frame_dt / static_cast<float>(m_sub_steps);
    }

    void update_motion(float dt) {
        // Update DOD particles
        integrate_particles(m_particle_data, dt);

        // Update OOP particles
        for (auto& particle : p_objects) {
            particle.update(dt);
        }
    }

    void calculate_gravity_orbit() {
        const float angular_velocity = 0.005f;  // Controls rotation speed
        const glm::vec3 center(0.0f, 0.0f, 0.0f);

        // Reset forces since Verlet uses forces to update velocities and positions
        m_particle_data.reset_forces();

        for (size_t i = 0; i < m_particle_data.count; ++i) {
            glm::vec3 r = m_particle_data.positions[i] - center;
            float radius = glm::length(r);

            if (radius < 0.001f) continue;

            // For uniform circular motion with Verlet, we need centripetal force
            // F = mv²/r where v = ω * r
            float v = angular_velocity * radius;  // Tangential velocity
            float force_magnitude = m_particle_data.masses[i] * v * v / radius;

            // Force points toward center for circular motion
            glm::vec3 force = -normalize(r) * force_magnitude;
            m_particle_data.forces[i] = force;
        }
    }





};