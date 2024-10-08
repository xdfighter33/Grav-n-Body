#pragma once
#include <iostream>
#include <glm/glm.hpp>

struct Particle {   

    // Velocity Verlet 
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 accel;
    glm::vec3 force = glm::vec3(0);
    float mass;




    ///Velocity Verlet prob gonna update this to use simd instructions
    void update(float dt){

           // Step 1: Update position
        position += velocity * dt + 0.5f *  accel *  dt * dt;
        



        glm::vec3 old_accel = accel;

        
        // Step 3: Compute new acceleration
        accel = force / mass;
        
        // Step 4: Compute new velocity
        velocity +=  0.5f * accel * dt;
        

        
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

