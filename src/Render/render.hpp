#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Particle_renders.hpp" // Include your P_Renderer class
#include "../Physics/simulation.hpp"// Include your Simulation class
#include "../settings/window.hpp"


class Render {
private:
    P_Renderer* renderer;
    glm::mat4 projection;
    glm::mat4 view;

public:
    //VertexShader Path then FragmentShaderPath
    Render(const std::string& vertexShaderPath, const std::string& fragmentShaderPath) {
        renderer = new P_Renderer(vertexShaderPath, fragmentShaderPath);
        
        // Set up projection matrix
        projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
        
        // Set up view matrix
        view = glm::lookAt(glm::vec3(0.0f, 0.0f, 10.0f), 
                           glm::vec3(0.0f, 0.0f, 0.0f), 
                           glm::vec3(0.0f, 1.0f, 0.0f));
    }

    ~Render() {
        delete renderer;
    }

    void renderParticles(const Simulation& sim) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //Prob should change Rendere name lokl
        renderer->useShader();

        // Set projection and view matrices
        renderer->setMat4("projection", projection);
        renderer->setMat4("view", view);

        // Bind the particle texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, renderer->getParticleTexture());
        renderer->setInt("particleTexture", 0);

        // Enable blending
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Render particles
        glBindVertexArray(renderer->getTextureVAO());
        for (const auto& particle : sim.p_objects) {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), particle.position);
            model = glm::scale(model, glm::vec3(0.01f)); // Adjust particle size as needed
            renderer->setMat4("model", model);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        }

        glDisable(GL_BLEND);
    }
    void updateProjection(float aspectRatio) {
        projection = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);
    }

    void updateView(const glm::vec3& cameraPos, const glm::vec3& cameraTarget, const glm::vec3& cameraUp) {
        view = glm::lookAt(cameraPos, cameraTarget, cameraUp);
    }


    void render_loop(window& Window,Simulation& sim){
        while(!glfwWindowShouldClose(Window.getWindow())){


        sim.update();  
        renderParticles(sim);

  
        glfwSwapBuffers(Window.getWindow());
        glfwPollEvents();


        }
       

        
    }

    void render(window& Window,Simulation& sim){

        renderParticles(sim);

    }
    
};