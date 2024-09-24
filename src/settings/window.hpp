#pragma once


#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "settings.hpp"

class window{
     public: 
    SystemSettings settings;

    
        window(){
        Window_size = settings.WindowSize;
        Window_name = settings.Window_Name;

        intiGLFW();
        createWindow();
        intiGLAD();
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_PROGRAM_POINT_SIZE);

    }

      GLFWwindow* getWindow() {
    return this->Window;
    }

 

    void intiGLFW(){
        glfwInit();

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,1);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
        #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        #endif
    }
    void intiGLAD(){
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        std::cout << "Failed to initialize GLAD" << std::endl;
        exit(-1);
        }
    }
    

    void createWindow(){
        this->Window = glfwCreateWindow(Window_size.x,Window_size.y,Window_name.c_str(),NULL,NULL);

        if (Window == NULL){
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        }

        glfwMakeContextCurrent(Window);
    }




    private:
    GLFWwindow* Window;
    glm::vec2 Window_size;
    std::string Window_name;
};