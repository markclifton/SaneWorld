#include "ogl_core.hpp"

namespace OGL {

    OGLCore::OGLCore()
    {
        glfwSetErrorCallback(error_callback);
 
        if (!glfwInit())
            exit(EXIT_FAILURE);
    
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    
        window = glfwCreateWindow(640, 480, "Simple example", NULL, NULL);
        if (!window)
        {
            glfwTerminate();
            exit(EXIT_FAILURE);
        }

        glfwSetKeyCallback(window, key_callback);
 
        glfwMakeContextCurrent(window);
        gladLoadGL();
        glfwSwapInterval(1);

        valid = true;
    }

    OGLCore::~OGLCore()
    {
        if(valid)
        {
            glfwDestroyWindow(window);
            glfwTerminate();
        }

        exit(EXIT_SUCCESS);
    }

    void OGLCore::update()
    {
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}