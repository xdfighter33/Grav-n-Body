#include <glm/glm.hpp>
#include "particle.hpp"
#include <random>
#include <cmath>

class Simulation{




    public:
    void add_Particle(glm::vec3 pos){
         Particle p;
        p.position = pos;  
        p.mass = 1.0f;

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
            add_Particle(position);

            // Optionally, set initial velocity
            // You could add random velocity or velocity based on position
            // p_objects.back().velocity = glm::ballRand(1.0f); // Random velocity
        }
    }
    std::vector<Particle> p_objects;

    uint32_t m_sub_steps = 10;
    float m_time = 0.0f;
    float m_frame_dt = 0.0f;


    void update()
    {
        m_time += m_frame_dt;
        const float step_dt = getStepDt();
        for (uint32_t i{m_sub_steps}; i--;) {
            calculateGravity();
            updateMotion(step_dt);
        }
    }


    float getStepDt() const
    {
        return m_frame_dt / static_cast<float>(m_sub_steps);
    }

    void updateMotion(float dt){

        for(auto& particles : p_objects){
            particles.update(dt);
            
        }        



    }

    void calculateGravity(){
                const glm::vec3 gravity(0.0f, 0.0f, 0.0f);

        for(auto& p_obj : p_objects){
           p_obj.addForce(gravity * p_obj.mass);
        }
    };



    void setSimulationUpdateRate(uint32_t rate)
    {
        m_frame_dt = 1.0f / static_cast<float>(rate);
    }

    void setSubsStepscount(uint32_t sub_substeps)
    {
        m_sub_steps = sub_substeps;
    }

    

    void temp_function(){

        for(auto particles : p_objects){
            std::cout << particles.position.x << std::endl; 
            std::cout << particles.position.y << std::endl;
            std::cout << particles.position.z << std::endl;
        }
}

};