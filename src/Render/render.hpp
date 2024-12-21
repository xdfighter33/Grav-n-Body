#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Particle_renders.hpp"
#include "../Physics/simulation.hpp"
#include "../settings/window.hpp"

class Render {
private:
    P_Renderer* renderer;
    glm::mat4 projection;
    glm::mat4 view;

public:
    Render(const std::string& vertexShaderPath, const std::string& fragmentShaderPath) {
        renderer = new P_Renderer(vertexShaderPath, fragmentShaderPath);

        projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
        view = glm::lookAt(glm::vec3(0.0f, 0.0f, 10.0f),
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, 1.0f, 0.0f));
    }

    ~Render() {
        delete renderer;
    }

    void render_particles_oop(const std::vector<Particle>& particles) {
        for (const auto& particle : particles) {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), particle.position);
            model = glm::scale(model, glm::vec3(0.01f));
            renderer->setMat4("model", model);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        }
    }

    void render_particles_dod(const ParticleData& particle_data) {
        for (size_t i = 0; i < particle_data.count; ++i) {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), particle_data.positions[i]);
            model = glm::scale(model, glm::vec3(0.01f));
            renderer->setMat4("model", model);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        }
    }


    //Debugged render_instnace_particles 
    void render_instance_particles_dod_debug(const ParticleData& particle_data) {
        // Debug prints
        std::cout << "Total particle count: " << particle_data.count << std::endl;

        // Print first few particle positions
        for (size_t i = 0; i < std::min(size_t(5), particle_data.count); i++) {
            std::cout << "Particle " << i << " position: "
                << particle_data.positions[i].x << ", "
                << particle_data.positions[i].y << ", "
                << particle_data.positions[i].z << std::endl;
        }

        if (particle_data.count == 0) return;

        // Check if updateInstanceData is working
        std::cout << "Updating instance data..." << std::endl;
        renderer->updateInstanceData(particle_data);

        // Verify VAO binding
        GLint currentVAO;
        glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &currentVAO);
        std::cout << "Current VAO: " << currentVAO << ", Expected VAO: " << renderer->getVAO() << std::endl;

        glBindVertexArray(renderer->getVAO());

        // Verify draw call parameters
        std::cout << "Drawing " << particle_data.count << " instances" << std::endl;
        glDrawElementsInstanced(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0, particle_data.count);
    }

    void render_instance_particles_dod(const ParticleData& particle_data) {


        if (particle_data.count == 0) return;

        renderer->updateInstanceData(particle_data);



        glBindVertexArray(renderer->getVAO());



        glDrawElementsInstanced(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0, particle_data.count);
    }


    void render_particles(const Simulation& sim) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        renderer->useShader();
        renderer->setMat4("projection", projection);
        renderer->setMat4("view", view);

        // Bind texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, renderer->getParticleTexture());
        renderer->setInt("particleTexture", 0);

        // Enable blending
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Render both OOP and DOD particles
        glBindVertexArray(renderer->getTextureVAO());

        // Render OOP particles
        render_particles_oop(sim.p_objects);

        // Render DOD particles
        render_particles_dod(sim.m_particle_data);

        glDisable(GL_BLEND);
    }


    void render_particles_instance(const Simulation& sim) {
        glEnable(GL_DEPTH_TEST);  // Add this

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        renderer->useShader();
        renderer->setMat4("projection", projection);
        renderer->setMat4("view", view);
        glDisable(GL_BLEND);

        // Bind texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, renderer->getParticleTexture());
        renderer->setInt("particleTexture", 0);

        // Enable blending
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Render both OOP and DOD particles
        glBindVertexArray(renderer->getTextureVAO());

        // Render OOP particles
       
        //render_particles_oop(sim.p_objects);

        // Render Instance DOD  particles



        render_instance_particles_dod(sim.m_particle_data);


        glDepthMask(GL_TRUE);
    }
    void update_projection(float aspectRatio) {
        projection = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);
    }

    void update_view(const glm::vec3& camera_pos, const glm::vec3& camera_target, const glm::vec3& camera_up) {
        view = glm::lookAt(camera_pos, camera_target, camera_up);
    }

    void render_loop(window& window_obj, Simulation& sim) {
        while (!glfwWindowShouldClose(window_obj.getWindow())) {
            sim.update();
            render_particles_instance(sim);

            glfwSwapBuffers(window_obj.getWindow());
            glfwPollEvents();
        }
    }

    void render(window& window_obj, Simulation& sim) {
        render_particles(sim);
    }
};