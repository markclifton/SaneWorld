#pragma once

#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/ext.hpp>
#include <glm/glm.hpp>

static void error_callback(int error, const char* description) {
  fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action,
  int mods) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GLFW_TRUE);
}

namespace OGL {
  class OGLCore {
  public:
    OGLCore(const char* Name, int Width, int Height);
    ~OGLCore();

    void update();
    inline bool isRunning() { return !glfwWindowShouldClose(window); }

    glm::mat4 getOrthoProjection();
    glm::mat4 getPersProjection();

    operator GLFWwindow* () { return window; }

  private:
    GLFWwindow* window;
    GLuint vao;
  };
} // namespace OGL