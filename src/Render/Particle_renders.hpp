#include <iostream>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../Shaders/shader_loading.h"


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
    unsigned int indices[36] = {
        0, 1, 2, 2, 3, 0,
        1, 5, 6, 6, 2, 1,
        5, 4, 7, 7, 6, 5,
        4, 0, 3, 3, 7, 4,
        3, 2, 6, 6, 7, 3,
        4, 5, 1, 1, 0, 4
    };
    unsigned int VAO, VBO, EBO;
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
    

    void setupBuffers() {
        // Setup for cube
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        
        // Setup for texture quad
        glGenVertexArrays(1, &textureVAO);
        glGenBuffers(1, &textureVBO);
        glBindVertexArray(textureVAO);
        glBindBuffer(GL_ARRAY_BUFFER, textureVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(frameBufferTexture), frameBufferTexture, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Texture), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Texture), (void*)(sizeof(glm::vec3)));
        glEnableVertexAttribArray(1);

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
    }

public: 
    P_Renderer(const std::string VertexPath, const std::string FragmentPath) {
        setupBuffers();
        setupTextures();
        shader = new Shader(VertexPath,FragmentPath);
    }

    ~P_Renderer() {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
        glDeleteVertexArrays(1, &textureVAO);
        glDeleteBuffers(1, &textureVBO);
        glDeleteTextures(1, &particleTexture);
        delete shader;
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

    unsigned int getParticleTexture() const {
        return particleTexture;
    }

    unsigned int getTextureVAO() const {
        return textureVAO;
    }
};