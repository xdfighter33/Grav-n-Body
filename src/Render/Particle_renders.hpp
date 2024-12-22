#pragma once 
#include <iostream>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../Shaders/shader_loading.h"
#include "../Physics/particle.hpp"


struct Texture {
    glm::vec3 position;
    glm::vec2 texCoords;
};



class P_Renderer {

    float vertices[24] = {

            -0.5f, -0.5f, -0.5f,

             0.5f, -0.5f, -0.5f,

             0.5f,  0.5f, -0.5f,

            -0.5f,  0.5f, -0.5f,

            -0.5f, -0.5f,  0.5f,

             0.5f, -0.5f,  0.5f,

             0.5f,  0.5f,  0.5f,

            -0.5f,  0.5f,  0.5f

    };  
    float Instanced_vertices[40] = {
        // positions         // texture coords
        -0.5f, -0.5f, 0.0f,  0.0f, 0.0f,
         0.5f, -0.5f, 0.0f,  1.0f, 0.0f,
         0.5f,  0.5f, 0.0f,  1.0f, 1.0f,
        -0.5f,  0.5f, 0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, 1.0f,  0.0f, 0.0f,
         0.5f, -0.5f, 1.0f,  1.0f, 0.0f,
         0.5f,  0.5f, 1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f, 1.0f,  0.0f, 1.0f
    };

    unsigned int indices[36] = {
        0, 1, 2, 2, 3, 0,
        1, 5, 6, 6, 2, 1,
        5, 4, 7, 7, 6, 5,
        4, 0, 3, 3, 7, 4,
        3, 2, 6, 6, 7, 3,
        4, 5, 1, 1, 0, 4
    };
    unsigned int VAO, VBO, EBO, instanceVBO, InstanceMassVBO, InstanceVelocityVBO;
    unsigned int shaderProgram;
    unsigned int textureVAO, textureVBO;
    unsigned int particleTexture;

    Shader* shader;
Texture frameBufferTexture[4] = {
        {glm::vec3(-1.0f,  1.0f, 0.0f), glm::vec2(0.0f, 1.0f)},
        {glm::vec3(-1.0f, -1.0f, 0.0f), glm::vec2(0.0f, 0.0f)},
        {glm::vec3(1.0f,  1.0f, 0.0f), glm::vec2(1.0f, 1.0f)},
        {glm::vec3(1.0f, -1.0f, 0.0f), glm::vec2(1.0f, 0.0f)}
    };
    
    void setupInstanceBuffer() {
    // Generate instance VBOs
    glGenBuffers(1, &instanceVBO);
    glGenBuffers(1, &InstanceMassVBO);
    glGenBuffers(1, &InstanceVelocityVBO);
    
    // Bind the VAO
    glBindVertexArray(VAO);
    
    // Setup position instance attributes
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * 4000, nullptr, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glVertexAttribDivisor(2, 1);
    
    // Setup mass instance attributes
    glBindBuffer(GL_ARRAY_BUFFER, InstanceMassVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 4000, nullptr, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(float), (void*)0);
    glVertexAttribDivisor(3, 1);
    
    // Setup velocity instance attributes
    glBindBuffer(GL_ARRAY_BUFFER, InstanceVelocityVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * 4000, nullptr, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glVertexAttribDivisor(4, 1);
    
    // Cleanup
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

    void setupBuffers() {
        // Setup for cube
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Instanced_vertices), Instanced_vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        // Position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // Texture coordinate attribute
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        // Bind default VAO for texture quad setup
        glBindVertexArray(textureVAO);
        glBindBuffer(GL_ARRAY_BUFFER, textureVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(frameBufferTexture), frameBufferTexture, GL_STATIC_DRAW);

        // Position attribute for texture quad
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Texture), (void*)0);
        glEnableVertexAttribArray(0);

        // Texture coordinates for texture quad
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Texture), (void*)(sizeof(glm::vec3)));
        glEnableVertexAttribArray(1);

        // Cleanup
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    void setupTextures() {
        glGenTextures(1, &particleTexture);
        glBindTexture(GL_TEXTURE_2D, particleTexture);

        // Create a simple circular particle texture
        int texWidth = 32, texHeight = 32;
        std::vector<unsigned char> textureData(texWidth * texHeight * 4, 0);
        for (int y = 0; y < texHeight; ++y) {
            for (int x = 0; x < texWidth; ++x) {
                float dx = (x + 0.5f) / texWidth - 0.5f;
                float dy = (y + 0.5f) / texHeight - 0.5f;
                float distance = std::sqrt(dx * dx + dy * dy);
                unsigned char intensity = (distance < 0.5f) ? 255 : 0;
                textureData[(y * texWidth + x) * 4 + 0] = intensity;  // R
                textureData[(y * texWidth + x) * 4 + 1] = intensity;  // G
                textureData[(y * texWidth + x) * 4 + 2] = intensity;  // B
                textureData[(y * texWidth + x) * 4 + 3] = intensity;  // A
            }
        }

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texWidth, texHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureData.data());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    }


public: 




    P_Renderer(const std::string VertexPath, const std::string FragmentPath) {
        initialize();

        shader = new Shader(VertexPath,FragmentPath);
    }

    ~P_Renderer() {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
        glDeleteBuffers(1, &instanceVBO);
        glDeleteBuffers(1, &InstanceMassVBO);
        glDeleteBuffers(1, &InstanceVelocityVBO);
        glDeleteVertexArrays(1, &textureVAO);
        glDeleteBuffers(1, &textureVBO);
        glDeleteTextures(1, &particleTexture);
       
        delete shader;
    }




    void initialize() {
        setupBuffers();
        setupInstanceBuffer();
        setupTextures();
    }



    void updateInstanceData(const ParticleData& particles) {
        glBindVertexArray(VAO);  
        glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
        glBufferData(GL_ARRAY_BUFFER,
            particles.count * sizeof(glm::vec3),
            particles.positions.data(),
            GL_DYNAMIC_DRAW);



        glBindBuffer(GL_ARRAY_BUFFER, InstanceMassVBO);
        glBufferData(GL_ARRAY_BUFFER,
            particles.count * sizeof(float),
            particles.masses.data(),
            GL_DYNAMIC_DRAW);
            


        glBindBuffer(GL_ARRAY_BUFFER, InstanceVelocityVBO);
        glBufferData(GL_ARRAY_BUFFER,
            particles.count * sizeof(glm::vec3),
            particles.velocities.data(),
            GL_DYNAMIC_DRAW);


        glBindBuffer(GL_ARRAY_BUFFER, 0); 
    //    glBindVertexArray(0);  // Add this line

        
    }



        void useShader() const {
        shader->use();
    }

    void setMat4(const std::string &name, const glm::mat4 &mat) const {
        shader->setMat4(name, mat);
    }

    void setInt(const std::string &name, int value) const {
        shader->setInt(name, value);
    }


    unsigned int getVAO() {
        return VAO;
    }
    unsigned int getParticleTexture() const {
        return particleTexture;
    }

    unsigned int getTextureVAO() const {
        return textureVAO;
    }
};