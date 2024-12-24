#include <glad/glad.h>
#include <GLFW/glfw3.h>
// 
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#define GLM_ENABLE_EXPERIMENTAL
#define GL_SILENCE_DEPRECATION
#if defined(IMGUI_IMPL_OPENGL_ES2)
#endif
#include "settings/window.hpp"
#include "settings/settings.hpp"
#include "Render/render.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>


#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif


 void errorCallback(int error, const char* description) {
    std::cerr << "GLFW Error " << error << ": " << description << std::endl;
}


std::string Model_fragmentShaderPath = std::string(SHADER_DIR) + "/fragment.glsl";
std::string Model_VertexShaderPath   = std::string(SHADER_DIR) + "/vertex.glsl";


 int main(){

    //Checking GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }


    glfwSetErrorCallback(errorCallback);

    window mainWindow;

    Render renderer(Model_VertexShaderPath,Model_fragmentShaderPath);

    Simulation sim;

    // Add some particles to the simulation
    // sim.add_particles_in_sphere_dod(1000,10.0f,glm::vec3(0.0f,0.0f,0.0f));
    sim.add_spiral_galaxy_dod(1000,5.0,glm::vec3(0,0,0));
    // sim.initializeGalaxy(200,200,200);
    sim.set_sub_steps_count(10);
    sim.set_simulation_update_rate(60);


    // Render Loop
    //Need to get inputs from the window
    renderer.render_loop(mainWindow,sim);
 }