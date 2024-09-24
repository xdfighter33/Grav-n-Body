#include <iostream>
#include <glm/glm.hpp>

struct Particle {   

    // Velocity Verlet 
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 accel;
    glm::vec3 force;
    float mass;




    ///Velocity Verlet prob gonna update this to use simd instructions
    void update(float dt){

           // Step 1: Update position
        position += velocity * dt + 0.5f * accel * dt * dt;
        
        // Step 2: Compute velocity at mid-point
        glm::vec3 vel_half = velocity + 0.5f * accel * dt;
        
        // Step 3: Compute new acceleration
        accel = glm::vec3(0);
        
        // Step 4: Compute new velocity
        velocity = vel_half + 0.5f * accel * dt;
        
        // Reset force for the next frame
        force = glm::vec3(0.0f);



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

