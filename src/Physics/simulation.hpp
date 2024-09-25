#include <glm/glm.hpp>
#include "particle.hpp"
#include <glm/gtc/random.hpp>
#include <random>
#include <cmath>

class Simulation{




    public:


    void add_Particle(glm::vec3 pos,glm::vec3 vel){
        Particle p;
        p.position = pos;  
        p.mass = 1.0f;
        p.velocity = vel;
        p_objects.push_back(p);

         
    }




        void addParticlesInSphere(int numParticles, float radius, const glm::vec3& center) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(0.0, 1.0);

        for (int i = 0; i < numParticles; ++i) {
            // Generate random spherical coordinates
            float theta = 2.0f * M_PI * dis(gen);
            float phi = acos(2.0f * dis(gen) - 1.0f);
            float r = radius * std::cbrt(dis(gen)); // Cube root for uniform distribution in sphere

            // Convert to Cartesian coordinates
            float x = r * sin(phi) * cos(theta);
            float y = r * sin(phi) * sin(theta);
            float z = r * cos(phi);

            // Create particle and add to simulation
            glm::vec3 position = center + glm::vec3(x, y, z);

            // Optionally, set initial velocity
            // You could add random velocity or velocity based on position
            auto temp = glm::ballRand(1.0f); // Random velocity
            auto temp_zero = glm::vec3(0);
            add_Particle(position,temp_zero);

  
        }
    }
    std::vector<Particle> p_objects;

    uint32_t m_sub_steps;
    float m_time = 0.0f;
    float m_frame_dt = 0.0f;


    void update()
    {
        m_time += m_frame_dt;
        const float step_dt = getStepDt();
        for (uint32_t i{m_sub_steps}; i--;) {
            calculateGravityOrbit();
            updateMotion(step_dt);
        }
        // debug();
    }




    void updateMotion(float dt){

        for(auto& particles : p_objects){
            particles.update(dt);
        }        



    }

    void calculateGravity(){
        const glm::vec3 gravity(0.0f, 0.0f,-9.81f);

        for(auto& p_obj : p_objects){
           p_obj.addForce(gravity);
        }
    }


       void calculateGravityOrbit() {
        const float G = 1.5; // Gravitational constant
        
        for (int i = 0; i < p_objects.size(); i++) {
            for (int j = i + 1; j < p_objects.size(); j++) {
                glm::vec3 direction = p_objects[j].position - p_objects[i].position;
                float distance = glm::length(direction);
                
                // Avoid division by zero
                if (distance < 1e-6f) continue;
                
                // Normalize the direction vector
                glm::vec3 unit_direction = glm::normalize(direction);
                
                // Calculate the magnitude of the gravitational force
                float force_magnitude = G * (p_objects[i].mass * p_objects[j].mass) / (distance * distance);
                 
                // Calculate the force vector
                glm::vec3 force = force_magnitude * unit_direction;
                // Apply the force to both particles (action-reaction)
                p_objects[i].addForce(force);
                p_objects[j].addForce(-force); // Opposite direction for the other particle
            }
        }
    }


    void setSimulationUpdateRate(uint32_t rate)
    {
        m_frame_dt = 1.0f / static_cast<float>(rate);
    }

    void setSubsStepscount(uint32_t sub_substeps)
    {
        m_sub_steps = sub_substeps;
    }

    
    float getStepDt() const
    {
        return m_frame_dt / static_cast<float>(m_sub_steps);
    }



    
    
    //Debug Commands 
    void debug(){
        for(auto& p_obj : p_objects){
            std::cout << "Current Particle Force  x " << p_obj.force.x << std::endl;
            std::cout << "Current Particle Force  y " << p_obj.force.y << std::endl;
            std::cout << "Current Particle Force  z " << p_obj.force.z << std::endl;
        }
    }
    void temp_function(){

        for(auto particles : p_objects){
            std::cout << particles.position.x << std::endl; 
            std::cout << particles.position.y << std::endl;
            std::cout << particles.position.z << std::endl;
        }
}

};
