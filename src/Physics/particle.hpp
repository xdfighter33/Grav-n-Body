#pragma once
#include <iostream>
#include <vector>
#include <memory>
#include <glm/glm.hpp>


struct ParticleData {
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> velocities;
    std::vector<glm::vec3> accelerations;
    std::vector<glm::vec3> forces;
    std::vector<float> masses;

    size_t count = 0;


    void allocate(size_t size) {
        positions.resize(size);
        velocities.resize(size);
        accelerations.resize(size);
        forces.resize(size, glm::vec3(0.0f));
        masses.resize(size);
        count = size;

    }


    void reserve(size_t capacity) {
        positions.reserve(capacity);
        velocities.reserve(capacity);
        accelerations.reserve(capacity);
        forces.reserve(capacity);
        masses.reserve(capacity);
    }


    void reset_forces() {
        std::fill(forces.begin(), forces.end(), glm::vec3(0.0f));
    }



}; 




void integrate_particles(ParticleData& particles, float dt) {
    for (size_t i = 0; i < particles.count; ++i) {
        particles.positions[i] += particles.velocities[i] * dt +
            0.5f * particles.accelerations[i] * dt * dt;
    }

    std::vector<glm::vec3> old_accelerations = particles.accelerations;

    particles.reset_forces();

    for (size_t i = 0; i < particles.count; ++i) {
        particles.accelerations[i] = particles.forces[i] / particles.masses[i];
    }

    for (size_t i = 0; i < particles.count; ++i) {
        particles.velocities[i] += 0.5f * (old_accelerations[i] + particles.accelerations[i]) * dt;
    }
}




struct Particle {   

    // Velocity Verlet 
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 accel = glm::vec3(0);
    glm::vec3 force = glm::vec3(0);
    float mass;




    ///Velocity Verlet prob gonna update this to use simd instructions
    void update(float dt) {

        // Step 1: Update position
        position += velocity * dt + 0.5f * accel * dt * dt;




        glm::vec3 old_accel = accel;


        // Step 3: Compute new acceleration
        accel = force / mass;

        // Step 4: Compute new velocity
        velocity += 0.5f * accel * dt;



        // Reset force for the next frame
        force = glm::vec3(0.0f);
        //  velocity = glm::vec3(0.0f);


    };


       void addForce(const glm::vec3& f) {
        force += f;
    }
    void AddVelocity(glm::vec3 Force,float dt){

        velocity += Force * (dt * (float)0.5 /  mass );;
    }

    void AddAccerlation(glm::vec3 Force){
            accel += Force;
    }

};

