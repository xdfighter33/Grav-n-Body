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

    // Legacy OOP methods
    void add_particle_oop(glm::vec3 pos, glm::vec3 vel) {
        Particle p;
        p.position = pos;
        p.mass = 1.0f;
        p.velocity = vel;
        p_objects.push_back(p);
    }

    // DOD methods
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

    void add_particles_in_sphere_dod(int num_particles, float radius, const glm::vec3& center) {
        m_particle_data.reserve(num_particles);

        size_t current_size = m_particle_data.count;
        m_particle_data.allocate(current_size + num_particles);

 
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(0.0, 1.0);

        for (int i = 0; i < num_particles; ++i) {
            size_t idx = current_size + i;

            float theta = 2.0f * M_PI * dis(gen);
            float phi = acos(2.0f * dis(gen) - 1.0f);
            float r = radius * std::cbrt(dis(gen));

            float x = r * sin(phi) * cos(theta);
            float y = r * sin(phi) * sin(theta);
            float z = r * cos(phi);

            m_particle_data.positions[idx] = center + glm::vec3(x, y, z);
            m_particle_data.velocities[idx] = glm::vec3(2.0f);
            m_particle_data.masses[idx] = 0.05f;
        }
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
        const float G = 1.5f;

        // DOD gravity calculation
        for (size_t i = 0; i < m_particle_data.count; i++) {
            for (size_t j = i + 1; j < m_particle_data.count; j++) {
                glm::vec3 direction = m_particle_data.positions[j] - m_particle_data.positions[i];
                float distance = glm::length(direction);

                if (distance < 1e-6f) continue;

                glm::vec3 unit_direction = direction / distance;
                float force_magnitude = G * (m_particle_data.masses[i] * m_particle_data.masses[j])
                    / (distance * distance);
                glm::vec3 force = force_magnitude * unit_direction;

                m_particle_data.forces[i] += force;
                m_particle_data.forces[j] -= force;
            }
        }
    }
};