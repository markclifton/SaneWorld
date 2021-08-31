#include "ogl_core.hpp"

namespace OGL {

    OGLCore::OGLCore(const char* Name, int Width, int Height)
    {
        glfwSetErrorCallback(error_callback);
 
        if (!glfwInit())
            exit(EXIT_FAILURE);
    
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    
        window = glfwCreateWindow(Width, Height, Name, NULL, NULL);
        if (!window)
        {
            glfwTerminate();
            exit(EXIT_FAILURE);
        }

        glfwSetKeyCallback(window, key_callback);
 
        glfwMakeContextCurrent(window);
        gladLoadGL();
        glfwSwapInterval(1);
    }

    OGLCore::~OGLCore()
    {
        glfwDestroyWindow(window);
        glfwTerminate();

        exit(EXIT_SUCCESS);
    }

    void OGLCore::update()
    {
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}